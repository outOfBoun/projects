-- Voicu Alex-Georgian

module ClassState
where

import qualified Data.Map.Strict as Map

-- Utilizat pentru a obține informații despre Variabile sau Funcții
data InstrType = Var | Func deriving (Show, Eq)

data ClassState = Class (Map.Map String String) (Map.Map [String] String) deriving (Show, Eq)

initEmptyClass :: ClassState
initEmptyClass = Class Map.empty Map.empty

insertIntoClass :: ClassState -> InstrType -> [String] -> ClassState
insertIntoClass (Class m1 m2) Var s = Class (insertIntoMapVar m1 s) m2
insertIntoClass (Class m1 m2) Func s = Class m1 (insertIntoMapFunc m2 s)

insertIntoMapVar m [] = m
insertIntoMapVar m (name:(vtype:[])) = Map.insert name vtype m

insertIntoMapFunc m [] = m
insertIntoMapFunc m (name:rettype:param) = Map.insert (name:param) rettype m

getValues :: ClassState -> InstrType -> [[String]]
getValues (Class m1 m2) Var = Map.foldrWithKey (\k a acc -> (k:a:[]):acc) [] m1
getValues (Class m1 m2) Func = Map.foldrWithKey (\(kname:kparam) a acc -> (kname:(a:kparam)):acc) [] m2
