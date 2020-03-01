grammar MyRegEx;

expr : possib_altern;

atom : '(' possib_altern ')' | symbol ;

possib_altern : altern | possib_concat ;
altern : possib_concat '|' possib_altern ;

possib_concat : concat | possib_star ;
concat : possib_star possib_concat ;

possib_star : atom | star | plus | maybe | anysymb | setsymb | rrange;

star : atom '*' | anysymb '*' | setsymb '*' | rrange '*' ;

maybe : atom '?' | anysymb '?' | setsymb '?' | rrange '?' ;

plus : atom '+' | anysymb '+' | setsymb '+' | rrange '+' ;

anysymb : '.' ;

setsymb : '[' setTerm ']' ;
setTerm : setTerm1 | setTerm2 ;
setTerm1 : symbol setTerm | symbol ;
setTerm2 : symbol '-' symbol setTerm | symbol '-' symbol ;

rrange : rrange1 | rrange2 | rrange3 | rrange4 ;
rrange1 : atom '{' number '}' ;
rrange2 : atom '{' ',' number '}' ;
rrange3 : atom '{' number ',' '}' ;
rrange4 : atom '{' number ',' number '}' ;

number : NUMBER+;
symbol : SYMBOL | NUMBER ;

NUMBER : [0-9] ;
SYMBOL : [A-Za-z] ;