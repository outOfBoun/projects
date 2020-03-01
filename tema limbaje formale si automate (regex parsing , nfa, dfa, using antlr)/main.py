#!/usr/bin/env python

# VOICU Alex-Georgian

import sys
import pickle

from regex import RegEx
from regular_expression import RegularExpression
from nfa import NFA
from dfa import DFAm

from antlr4 import *
from MyRegExLexer import MyRegExLexer
from MyRegExListener import MyRegExListener
from MyRegExParser import MyRegExParser

alphabet = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789"

rgxEMPTY_STRING = 0
rgxSYMBOL_SIMPLE = 1
rgxSYMBOL_ANY = 2
rgxSYMBOL_SET = 3
rgxMAYBE = 4
rgxSTAR = 5
rgxPLUS = 6
rgxRANGE = 7
rgxCONCATENATION = 8
rgxALTERNATION = 9

reEMPTY_SET = 0
reEMPTY_STRING = 1
reSYMBOL = 2
reSTAR = 3
reCONCATENATION = 4
reALTERNATION = 5




#########################################################################################
# Regex to Regular Expression

def getREfromREGEX(rgx):
    if rgx.type == rgxEMPTY_STRING:
        return RegularExpression(reEMPTY_STRING)

    elif rgx.type == rgxSYMBOL_SIMPLE:
        return RegularExpression(reSYMBOL, rgx.symbol)

    elif rgx.type == rgxSYMBOL_ANY:
        re = RegularExpression(reSYMBOL, alphabet[0])
        for i in alphabet[1:]:
            re = re | i
        return re

    elif rgx.type == rgxSYMBOL_SET:
        re = None
        for el in rgx.symbol_set:
            if re == None:
                if isinstance(el, tuple):
                    re = RegularExpression(reSYMBOL, el[0])
                    for ch in range(ord(el[0]) + 1, ord(el[1]) + 1):
                        re = re | chr(ch)
                else:
                    re = RegularExpression(reSYMBOL, el)
            else:
                if isinstance(el, tuple):
                    for ch in range(ord(el[0]), ord(el[1]) + 1):
                        re = re | chr(ch)
                else:
                    re = re | el
        return re

    elif rgx.type == rgxMAYBE:
        re = RegularExpression(reEMPTY_STRING)
        re = re | getREfromREGEX(rgx.lhs)
        return re

    elif rgx.type == rgxSTAR:
        re = getREfromREGEX(rgx.lhs)
        re = re.star()
        return re

    elif rgx.type == rgxPLUS:
        re = getREfromREGEX(rgx.lhs)
        re = re * re.star()
        return re

    elif rgx.type == rgxRANGE:
        re = None
        if rgx.range[0] == rgx.range[1]:
            lhsre = getREfromREGEX(rgx.lhs)
            re = lhsre
            for i in range(2, rgx.range[0] + 1):
                re = re * lhsre

        elif rgx.range[0] == -1:
            lhsre = getREfromREGEX(rgx.lhs)
            concatlhsre = lhsre
            re = RegularExpression(reEMPTY_STRING) | concatlhsre
            for i in range(2, rgx.range[1] + 1):
                concatlhsre = concatlhsre * lhsre
                re = re | concatlhsre

        elif rgx.range[1] == -1:
            lhsre = getREfromREGEX(rgx.lhs)
            concatlhsre = lhsre
            for i in range(1, rgx.range[0]):
                concatlhsre = concatlhsre * lhsre
            re = concatlhsre * lhsre.star()

        else:
            lhsre = getREfromREGEX(rgx.lhs)
            concatlhsre = lhsre
            for i in range(1, rgx.range[0]):
                concatlhsre = concatlhsre * lhsre
            re = concatlhsre
            for i in range(rgx.range[0] + 1, rgx.range[1] + 1):
                concatlhsre = concatlhsre * lhsre
                re = re | concatlhsre
            
        return re

    elif rgx.type == rgxCONCATENATION:
        re = getREfromREGEX(rgx.lhs) * getREfromREGEX(rgx.rhs)
        return re

    elif rgx.type == rgxALTERNATION:
        re = getREfromREGEX(rgx.lhs) | getREfromREGEX(rgx.rhs)
        return re



#########################################################################################
# Regular Expression to NFA

def rename_states(target, reference):
    off = max(reference.states) + 1
    target.start_state += off
    target.states = set(map(lambda s: s + off, target.states))
    target.final_states = set(map(lambda s: s + off, target.final_states))
    new_delta = {}
    for (state, symbol), next_states in target.delta.items():
        new_next_states = set(map(lambda s: s + off, next_states))
        new_delta[(state + off, symbol)] = new_next_states

    target.delta = new_delta


def new_states(*nfas):
    state = 0
    for nfa in nfas:
        m = max(nfa.states)
        if m >= state:
            state = m + 1

    return state, state + 1

def re_to_nfa(re):
    if re.type == reEMPTY_SET:
      return NFA("", {0, 1}, 0, {1}, {})
    elif re.type == reEMPTY_STRING:
      return NFA("", {0, 1}, 0, {1}, {(0, ""):{1}})
    elif re.type == reSYMBOL:
      return NFA(re.symbol, {0, 1}, 0, {1}, {(0, re.symbol):{1}})
    elif re.type == reCONCATENATION:
      l = re_to_nfa(re.lhs)
      r = re_to_nfa(re.rhs)
      ea = re_to_nfa(RegularExpression(reEMPTY_SET))
      
      rename_states(l, ea)
      ea.delta.update(l.delta)
      ea.delta.update({(ea.start_state, ""):{l.start_state}})
      ea.states = set(list(l.states) + list(ea.states))
      
      rename_states(r, ea)
      ea.alphabet = ''.join(list(set(ea.alphabet + r.alphabet + l.alphabet)))
      ea.delta.update(r.delta)
      ea.delta.update({(list(l.final_states)[0], ""):{r.start_state}})
      ea.delta.update({(list(r.final_states)[0], ""):{list(ea.final_states)[0]}})
      ea.states = set(list(r.states) + list(ea.states))
      return ea
    
    elif re.type == reSTAR:
      l = re_to_nfa(re.lhs)
      ea = re_to_nfa(RegularExpression(reEMPTY_SET))
      
      rename_states(l, ea)
      ea.delta.update(l.delta)
      ea.delta.update({(ea.start_state, ""):{l.start_state, list(ea.final_states)[0]}})
      
      ea.delta.update({(list(l.final_states)[0], ""):{list(ea.final_states)[0], l.start_state}})
      ea.alphabet = ''.join(list(set(ea.alphabet + l.alphabet)))
      ea.states = set(list(l.states) + list(ea.states))
      return ea
    
    elif re.type == reALTERNATION:
      l = re_to_nfa(re.lhs)
      r = re_to_nfa(re.rhs)
      ea = re_to_nfa(RegularExpression(reEMPTY_SET))
      
      rename_states(l, ea)
      ea.delta.update(l.delta)
      ea.delta.update({(list(l.final_states)[0], ""):{list(ea.final_states)[0]}})
      ea.states = set(list(l.states) + list(ea.states))
      
      rename_states(r, ea)
      ea.alphabet = ''.join(list(set(ea.alphabet + r.alphabet + l.alphabet)))
      ea.delta.update(r.delta)
      ea.delta.update({(ea.start_state, ""):{l.start_state, r.start_state}})
      ea.delta.update({(list(r.final_states)[0], ""):{list(ea.final_states)[0]}})
      ea.states = set(list(r.states) + list(ea.states))
      return ea



#########################################################################################
# NFA TO DFA

def computeEpsilonClosure(nfa):
    epsClosureMap = {}
    for state in nfa.states:
        qstates = [state]
        visited = [state]
        while qstates:
            crt = qstates[0]
            qstates = qstates[1:]
            if (crt, "") in nfa.delta:
                for vec in nfa.delta[(crt, "")]:
                    if not vec in visited:
                        visited.append(vec)
                        qstates.append(vec)
        visited.sort()
        epsClosureMap[state] = visited

    return epsClosureMap

def getDFAfromNFA(nfa):
    epsClosureMap = computeEpsilonClosure(nfa)

    d_alphabet = nfa.alphabet
    d_states = {0}
    d_start_state = 0
    d_final_states = set()
    d_delta = {}

    last_added_state = 0;
    map_states_to_dfa = {tuple(epsClosureMap[0]) : 0}
    map_states_to_nfa = {0 : epsClosureMap[0]}

    for nfa_fin in nfa.final_states:
        if nfa_fin in epsClosureMap[0]:
            d_final_states.add(last_added_state)

    qstates = [0]
    visited = [0]
    while qstates:
        crt = qstates[0]
        qstates = qstates[1:]
        
        if len(map_states_to_nfa[crt]) == 0:
            continue

        for ch in d_alphabet:
            visitedsubvecs = set()
            for subcrt in map_states_to_nfa[crt]:
                if (subcrt, ch) in nfa.delta:
                    visitedsubvecs.update(nfa.delta[(subcrt, ch)])
            if len(visitedsubvecs) == 0:
                continue

            epsVec = set()
            for st in visitedsubvecs:
                epsVec.update(set(epsClosureMap[st]))

            visitedsubvecs_list = list(epsVec)
            visitedsubvecs_list.sort()
            visitedsubvecs_tuple = tuple(visitedsubvecs_list)

            if not visitedsubvecs_tuple in map_states_to_dfa:
                last_added_state = last_added_state + 1
                map_states_to_dfa[visitedsubvecs_tuple] = last_added_state
                map_states_to_nfa[last_added_state] = visitedsubvecs_tuple
                for nfa_fin in nfa.final_states:
                    if nfa_fin in visitedsubvecs_tuple:
                        d_final_states.add(last_added_state)
                        break
                qstates.append(last_added_state)
                d_states.add(last_added_state)

            d_delta[(crt, ch)] = map_states_to_dfa[visitedsubvecs_tuple]

    dfa = DFAm(d_alphabet, d_states, d_start_state, d_final_states, d_delta)

    return dfa



#########################################################################################
# Main steps called through functions

def getDFAfromREGEX(rgx):
    re = getREfromREGEX(rgx)
    nfa = re_to_nfa(re)
    dfa = getDFAfromNFA(nfa)

    return dfa



#########################################################################################
# Visitor for building RegEx from parsed input

class RegexBuilderListener(MyRegExListener):

    def __init__(self):
        self.Numstack = []
        self.Symbstack = []
        self.RegExstack = []

    # Exit a parse tree produced by MyRegExParser#atom.
    def exitAtom(self, ctx:MyRegExParser.AtomContext):
        if ctx.symbol() != None:
            self.RegExstack.append(RegEx(rgxSYMBOL_SIMPLE, self.Symbstack.pop()))

    # Exit a parse tree produced by MyRegExParser#concat.
    def exitConcat(self, ctx:MyRegExParser.ConcatContext):
        rhs = self.RegExstack.pop()
        lhs = self.RegExstack.pop()
        self.RegExstack.append(RegEx(rgxCONCATENATION, lhs, rhs))

    # Exit a parse tree produced by MyRegExParser#altern.
    def exitAltern(self, ctx:MyRegExParser.AlternContext):
        rhs = self.RegExstack.pop()
        lhs = self.RegExstack.pop()
        self.RegExstack.append(RegEx(rgxALTERNATION, lhs, rhs))

    # Exit a parse tree produced by MyRegExParser#star.
    def exitStar(self, ctx:MyRegExParser.StarContext):
        lhs = self.RegExstack.pop()
        self.RegExstack.append(RegEx(rgxSTAR, lhs))

    # Exit a parse tree produced by MyRegExParser#anysymb.
    def exitAnysymb(self, ctx:MyRegExParser.AnysymbContext):
        self.RegExstack.append(RegEx(rgxSYMBOL_ANY))

    # Enter a parse tree produced by MyRegExParser#setsymb.
    def enterSetsymb(self, ctx:MyRegExParser.SetsymbContext):
        self.setsymb = set()

    # Exit a parse tree produced by MyRegExParser#setsymb.
    def exitSetsymb(self, ctx:MyRegExParser.SetsymbContext):
        self.RegExstack.append(RegEx(rgxSYMBOL_SET, self.setsymb))

    # Exit a parse tree produced by MyRegExParser#setTerm1.
    def exitSetTerm1(self, ctx:MyRegExParser.SetTerm1Context):
        self.setsymb.add(self.Symbstack.pop())

    # Exit a parse tree produced by MyRegExParser#setTerm3.
    def exitSetTerm2(self, ctx:MyRegExParser.SetTerm2Context):
        rhs = self.Symbstack.pop()
        lhs = self.Symbstack.pop()
        self.setsymb.add((lhs, rhs))

    # Exit a parse tree produced by MyRegExParser#maybe.
    def exitMaybe(self, ctx:MyRegExParser.MaybeContext):
        lhs = self.RegExstack.pop()
        self.RegExstack.append(RegEx(rgxMAYBE, lhs))

    # Exit a parse tree produced by MyRegExParser#plus.
    def exitPlus(self, ctx:MyRegExParser.PlusContext):
        lhs = self.RegExstack.pop()
        self.RegExstack.append(RegEx(rgxPLUS, lhs))

    # Exit a parse tree produced by MyRegExParser#rrange1.
    def exitRrange1(self, ctx:MyRegExParser.Rrange1Context):
        lhs = self.RegExstack.pop()
        count = self.Numstack.pop()
        rhs = (count, count)
        self.RegExstack.append(RegEx(rgxRANGE, lhs, rhs))

    # Exit a parse tree produced by MyRegExParser#rrange2.
    def exitRrange2(self, ctx:MyRegExParser.Rrange2Context):
        lhs = self.RegExstack.pop()
        count = self.Numstack.pop()
        rhs = (-1, count)
        self.RegExstack.append(RegEx(rgxRANGE, lhs, rhs))

    # Exit a parse tree produced by MyRegExParser#rrange3.
    def exitRrange3(self, ctx:MyRegExParser.Rrange3Context):
        lhs = self.RegExstack.pop()
        count = self.Numstack.pop()
        rhs = (count, -1)
        self.RegExstack.append(RegEx(rgxRANGE, lhs, rhs))

    # Exit a parse tree produced by MyRegExParser#rrange4.
    def exitRrange4(self, ctx:MyRegExParser.Rrange4Context):
        lhs = self.RegExstack.pop()
        count2 = self.Numstack.pop()
        count1 = self.Numstack.pop()
        rhs = (count1, count2)
        self.RegExstack.append(RegEx(rgxRANGE, lhs, rhs))

    # Exit a parse tree produced by MyRegExParser#number.
    def exitNumber(self, ctx:MyRegExParser.NumberContext):
        num = 0
        for dec in ctx.NUMBER():
            decc = ord(str(dec)) - ord('0')
            num = num * 10
            num = num + decc
        self.Numstack.append(num)

    # Exit a parse tree produced by MyRegExParser#symbol.
    def exitSymbol(self, ctx:MyRegExParser.SymbolContext):
        if ctx.SYMBOL() != None:
            self.Symbstack.append(str(ctx.SYMBOL()))
        elif ctx.NUMBER() != None:
            self.Symbstack.append(str(ctx.NUMBER()))



if __name__ == "__main__":
    valid = (len(sys.argv) == 4 and sys.argv[1] in ["RAW", "TDA"]) or \
            (len(sys.argv) == 3 and sys.argv[1] == "PARSE")
    if not valid:
        sys.stderr.write(
            "Usage:\n"
            "\tpython3 main.py RAW <regex-str> <words-file>\n"
            "\tOR\n"
            "\tpython3 main.py TDA <tda-file> <words-file>\n"
            "\tOR\n"
            "\tpython3 main.py PARSE <regex-str>\n"
        )
        sys.exit(1)

    if sys.argv[1] == "TDA":
        tda_file = sys.argv[2]
        with open(tda_file, "rb") as fin:
            parsed_regex = pickle.loads(fin.read())
    else:
        regex_string = sys.argv[2]
        
        stream = InputStream(regex_string)
        lexer = MyRegExLexer(stream)
        stream = CommonTokenStream(lexer)
        parser = MyRegExParser(stream)
        tree = parser.expr()
        regexbuilder = RegexBuilderListener()
        walker = ParseTreeWalker()
        walker.walk(regexbuilder, tree)

        parsed_regex = regexbuilder.RegExstack[0]

        if sys.argv[1] == "PARSE":
            print(str(parsed_regex))
            sys.exit(0)


    dfa_res = getDFAfromREGEX(parsed_regex)

    with open(sys.argv[3], "r") as fin:
        content = fin.readlines()

    for word in content:

        state = dfa_res.start_state
        noerror = True
        for ch in word:
            if ch in alphabet:
                if (state, ch) in dfa_res.delta:
                    state = dfa_res.delta[(state, ch)]
                else:
                    noerror = False
                    break
            else:
                break

        if noerror == True:
            if state in dfa_res.final_states:
                print("True")
            else:
                print("False")
        else:
            print("False")
