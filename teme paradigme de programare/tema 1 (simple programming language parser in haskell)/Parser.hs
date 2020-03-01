-- Voicu Alex-Georgian

module Parser
where

import Util
import Data.Maybe
import InferenceDataType

import ClassState
import qualified Data.Map.Strict as Map

import Debug.Trace

-- Definire Program


data ClassObj = ClassObj String ClassState deriving (Show, Eq)
-- first = String name of the class it extends
-- second = ClassState data type that contains the functions and variables of a " Class "

data Program = Program (Map.Map String ClassObj) deriving (Show, Eq)
-- the program is a map of ClassObjs that maps " Class' "  names to their respective ClassObj

data Instruction = Instruction String deriving (Show, Eq)
-- the instruction is a well parsed trimmed String representing a line


initEmptyProgram :: Program
initEmptyProgram = Program (Map.insert "Global" (ClassObj "Global" initEmptyClass) (Map.empty))

getVars :: Program -> [[String]]
getVars (Program map) = getVarsaux (Map.lookup "Global" map)
getVarsaux Nothing = []
getVarsaux (Just (ClassObj _ classstate)) = getValues classstate Var

getClasses :: Program -> [String]
getClasses (Program map)= Map.foldrWithKey (\k a acc -> k:acc) [] map

getParentClass :: String -> Program -> String
getParentClass sfind (Program m) = getParentClassaux (Map.lookup sfind m)
getParentClassaux Nothing = []
getParentClassaux (Just (ClassObj ext _)) = ext

getFuncsForClass :: String -> Program -> [[String]]
getFuncsForClass classname (Program map) = getFuncsForClassaux (Map.lookup classname map)
getFuncsForClassaux Nothing = []
getFuncsForClassaux (Just (ClassObj _ classstate)) = getValues classstate Func

parse :: String -> [Instruction]
parse input = map (\i -> Instruction i) (splitBy '\n' input [] [])

-- split string by a single character
splitBy :: Char -> String -> String -> [String] -> [String]
splitBy chr [] crt acc
	| crt == [] = acc
	| otherwise = acc++(crt:[])

splitBy chr (x:str) crt acc
	| x /= chr = splitBy chr str (crt++(x:[])) acc
	| crt == [] = splitBy chr str [] acc
	| otherwise = splitBy chr str [] (acc++(crt:[]))

-- split string by list of characters
splitByMult :: String -> String -> String -> [String] -> [String]
splitByMult chrs [] crt acc
	| crt == [] = acc
	| otherwise = acc++(crt:[])

splitByMult chrs (x:str) crt acc
	| elem x chrs == False = splitByMult chrs str (crt++(x:[])) acc
	| crt == [] = splitByMult chrs str [] acc
	| otherwise = splitByMult chrs str [] (acc++(crt:[]))

interpret :: Instruction -> Program -> Program
interpret (Instruction ins) prog = interpretaux (splitByMult " :(),=" ins [] []) prog

interpretaux :: [String] -> Program -> Program
interpretaux ("class":name:"extends":ext:[]) (Program map)
	| (Map.member name map) == True = Program map
	| (Map.member ext map) == False = Program (Map.insert name (ClassObj "Global" initEmptyClass) map)
	| otherwise = Program (Map.insert name (ClassObj ext initEmptyClass) map)

interpretaux ("class":name:[]) (Program map) = interpretaux ("class":name:"extends":"Global":[]) (Program map)

interpretaux ("newvar":name:vtype:[]) (Program map)
	| (Map.member vtype map) == True = Program (Map.update insertval "Global" map)
	| otherwise = Program map
	where insertval = (\(ClassObj ext classstate) -> Just (ClassObj ext (insertIntoClass classstate Var (name:vtype:[]))))

interpretaux (return:classname:name:paramlist) (Program map)
	| (checktypes (return:classname:paramlist)) == True  = Program (Map.update insertfunc classname map)
	| otherwise = Program map
	where {insertfunc = (\(ClassObj ext classstate) -> Just (ClassObj ext (insertIntoClass classstate Func (name:return:paramlist))));
			checktypes = (\list -> foldl (\acc el -> (Map.member el map) && acc) True list)}

infer :: Expr -> Program -> Maybe String
infer (Va name) prog = inferVarAux name (getVars prog)

infer f@(FCall var fname listexpr) prog = inferVarClass (infer (Va var) prog) f prog

inferVarAux name [] = Nothing
inferVarAux name ((symb:vtype:[]):rest)
	| name == symb = Just vtype
	| otherwise = inferVarAux name rest

inferVarClass vtype f@(FCall var fname listexpr) prog
	| (vtype == Nothing) = Nothing
	| otherwise = inferSubexpression vtype f prog

inferGetSameNameFuncs (Just classname) fname prog
	| classname == "Global" = if gf /= [] then gf else []
	| otherwise = if gf == []
					then inferGetSameNameFuncs (Just (getParentClass classname prog)) fname prog
					else gf ++ (inferGetSameNameFuncs (Just (getParentClass classname prog)) fname prog)
	where gf = foldl (\acc (fsymb:rest) -> if (fsymb == fname) then (fsymb:rest):acc else acc) [] (getFuncsForClass classname prog)

inferSubexpression vtype f@(FCall var fname listexpr) prog
	| posibfuncs == [] = Nothing
	| ((foldl (\acc x -> (x == Nothing) || acc) False paramtypes) == True) = Nothing
	| otherwise = inferSubexpressionaux posibfuncs paramtypes
		where {posibfuncs = inferGetSameNameFuncs vtype fname prog;
			  paramtypes = map (\x -> infer x prog) listexpr}

inferSubexpressionaux [] paramtypes = Nothing
inferSubexpressionaux ((symb:ret:funcparam):posibfuncs) paramtypes = if ((map (\x -> Just x) funcparam) == paramtypes)
																	then Just ret
																	else inferSubexpressionaux posibfuncs paramtypes