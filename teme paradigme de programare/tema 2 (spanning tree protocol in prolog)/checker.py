from os import listdir
from os.path import isdir, join
from subprocess import call, check_output
import subprocess
import sys
import argparse
import json
import os

# prolog goals
checkingScript = '\
    retea(R),\
    from(F),\
    to(T),\
    stp(R,Root,Edges),\
    write(Root), write(:), write(Edges), halt\
';
checkingScriptWithBonus = '\
    retea(R),\
    from(F),\
    to(T),\
    drum(R, F, T, Root, Edges, Path),\
    write(Root), write(:), write(Edges), write(:), write(Path), halt\
';


# grading
PERCENTAGE_ROOT = 0.10
PERCENTAGE_EDGES = 0.80
PERCENTAGE_BONUS = 0.25
NO_EASY_TESTS = 10
NO_HARD_TESTS = 70
EASY_TESTS_SCORE = 0.30
HARD_TESTS_SCORE = 0.70


def generate_output(swipl, tema, testin, goal, suppress_errors):
    # open test file
    with open(testin) as f:
        content = f.readlines()
        
    # remove endline characters
    content = [x.strip() for x in content] 
    
    # build prolog asserts
    for index in range(len(content)):
        line = content[index]
        assertline = 'assert(({0}))'.format(line[:-1])
        content[index] = assertline
        
    # concatenate assertions
    assertions = ','.join(content);
    
    # final prolog script
    prolog = assertions + ',' + goal + '.';
        
    # run for 10 seconds max
    command = 'timeout 15 {0} -t halt -l "{1}" -g "{2}"'.format(swipl, tema, prolog)
    #command = '{0} -t halt -l "{1}" -g "{2}"'.format(swipl, tema, prolog)
    
    try:
        if not suppress_errors:
            r = check_output(command, shell=True)
        else:
            with open(os.devnull, 'w') as devnull:
                r = check_output(command, shell=True, stderr=devnull)
        r = r.decode("utf-8")
    except subprocess.CalledProcessError as grepexc:
        # return code 1 means goal failed
        # return code 2 means raised exception
        r = "false" if grepexc.returncode is 1 else "error"
    return r if r else "false"

    
def run_test(swipl, hwfile, testfile, reffile, suppress_errors = False):
    output = generate_output(swipl, hwfile, testfile, checkingScript, suppress_errors)
    
    score = 0
    try:
        if output == "false" or output == "error":
            return output, 0
            
        outparts = output.split(':')
        outroot = json.loads(outparts[0])
        outedges = json.loads(outparts[1])
        
        with open(reffile, 'r') as file:
            reffile = file.read().replace('\n', '')
        
        refparts = reffile.split(':')
        refroot = json.loads(refparts[0])
        refedges = json.loads(refparts[1])
        refpath = json.loads(refparts[2])
        
        if outroot == refroot:
            score += PERCENTAGE_ROOT
            if sorted(outedges) == sorted(refedges):
                score += PERCENTAGE_EDGES
                
                output = generate_output(swipl, hwfile, testfile, checkingScriptWithBonus, suppress_errors)
                
                if output == "false" or output == "error":
                    return output, score
                
                outparts = output.split(':')
                outpath = json.loads(outparts[2])
                if outpath == refpath:
                    score += PERCENTAGE_BONUS
        
        return output, score
    except Exception as e:
         #print(e)
        return output, score
   
 
def run_all(swipl, hwfile):
    score_per_easy_test = EASY_TESTS_SCORE / NO_EASY_TESTS if NO_EASY_TESTS != 0 else 0
    score_per_hard_test = HARD_TESTS_SCORE / NO_HARD_TESTS if NO_HARD_TESTS != 0 else 0
    
    print("Type #no\tresult%\tpoints\ttotal")
    
    total = 0
    
    for i in range(NO_EASY_TESTS):
        testfile = "easy/in_easy" + str(i) + ".txt"
        reffile = "easy/out_easy" + str(i) + ".txt"
        
        output, score = run_test(swipl, hwfile, testfile, reffile, True)
        
        points = score * score_per_easy_test
        total += points
        
        print("Easy #{0}:\t{1:.0f}%\t{2:.2f}\t{3:.2f}".format(i, score*100, points*100, total*100))
        
    for i in range(NO_HARD_TESTS):
        testfile = "hard/in_hard" + str(i) + ".txt"
        reffile = "hard/out_hard" + str(i) + ".txt"
        
        output, score = run_test(swipl, hwfile, testfile, reffile, True)
        
        points = score * score_per_hard_test
        total += points
        
        print("Hard #{0}:\t{1:.0f}%\t{2:.2f}\t{3:.2f}".format(i, score*100, points*100, total*100))
 
    print("Your total score: {0} out of 115".format(int(round(total*100+0.01))))
    print("Another 10 points may be awarded for the README and for code readability")


def generate_reference(swipl, hwfile):
    for i in range(NO_EASY_TESTS):
        print("Generating EASY #" + str(i))
        testfile = "easy/in_easy" + str(i) + ".txt"
        reffile = "easy/out_easy" + str(i) + ".txt"
        output = generate_output(swipl, hwfile, testfile, checkingScriptWithBonus, True)
        with open(reffile, "w") as file:
            file.write(output)
        
    for i in range(60, NO_HARD_TESTS):
        print("Generating HARD #" + str(i))
        testfile = "hard/in_hard" + str(i) + ".txt"
        reffile = "hard/out_hard" + str(i) + ".txt"
        output = generate_output(swipl, hwfile, testfile, checkingScriptWithBonus, True)
        with open(reffile, "w") as file:
            file.write(output)
 
    
if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='Prolog Homework Checker')
    parser.add_argument('--swiplexe', default='swipl',
                    help='Absolute or relative path to swipl.exe; default \
                    "swipl" but you need to add its location to PATH')
    parser.add_argument('--hwfile', default="main.pl",
                    help='Path to prolog homework file. Example: main.pl')
    parser.add_argument('--testfile',
                    help='Path to test file. Example: easy/in_easy1.txt')
    parser.add_argument('--reffile',
                    help='Path to reference file. Example: easy/out_easy1.txt')
    parser.add_argument('--generateref', action="store_true",
                    help='Internal use only. Do not activate. WILL OVERWRITE REFERENCE FILES.')
    args = parser.parse_args()
    
    if (args.testfile and not args.reffile) or (args.reffile and not args.testfile):
        sys.exit('If you don\'t run all tests you must specify both --testfile and --reffile parameters')
    
    if args.testfile and args.reffile:
        output, score = run_test(args.swiplexe, args.hwfile, args.testfile, args.reffile)
        print("Your score on this test: {0}%".format(score*100))
        with open('output.txt', 'w') as f:
            f.write(output)
    elif args.generateref:
        generate_reference(args.swiplexe, args.hwfile)
    else:
        run_all(args.swiplexe, args.hwfile)
        
