grammar bluefin;

program : (funcDef | varDecl | structDef)+ ;

funcDef : type ID '(' paramList? ')' block ;

type : builtinType
     | ID ;

builtinType : INT | FLOAT | STRING | BOOL | VOID ;

paramList : param (',' param)* ;

param : type ID;

block : '{' stmt* '}' ;

stmt : stmtIf
     | stmtWhile
     | stmtReturn
     | stmtBreak
     | stmtContinue
     | block
     | stmtExpr
     | varDecl ;

stmtIf : 'if' '(' expr ')' block ('else' block)? ;

stmtWhile : 'while' '(' expr ')' block ;

stmtReturn : 'return' expr? ';' ;

stmtBreak : 'break' ';' ;

stmtContinue : 'continue' ';' ;

stmtExpr : expr ';' ;

structDef : 'struct' ID '{' varDecl* '}' ';' ;

varDecl : type ID ('=' expr)? ';' ;

/* ANTLR is left-associative by default and resolves ambiguity by selecting top-most alternative
It also resolves direct left recrusion so don't need to manually remove them
This means that precedence and associativeness ambiguities are solved
TODO: handle power (right associative)
*/
expr : primaryExpr                          # primaExpr
     | postfixExpr                          # postExpr
     | ('-' | '!') expr                     # unaryExpr
     | expr ('*' | '/') expr                # multiExpr
     | expr ('+' | '-') expr                # addExpr
     | expr ('<' | '<=' | '>' | '>=') expr  # relExpr
     | expr ('==' | '!=') expr              # equalityExpr
     | expr ('&&') expr                     # logicalANDExpr
     | expr ('||') expr                     # logicalORExpr
     |<assoc=right> expr '=' expr           # simpleAssignExpr
     ;

postfixExpr : postfixExpr '(' argList? ')'
            | postfixExpr '.' ID
            | primaryExpr
            ;

argList : expr (',' expr) ;

primaryExpr : ID
            | INT
            | FLOAT
            | STRING
            | '(' expr ')'
            ;


ID : LETTER+;     // only characters

INT : DIGIT+;       // even '007' and '000' are ints

FLOAT: DIGIT+ '.' DIGIT+ ;

BOOL: 'true' | 'false' ;

VOID: 'void' ;

STRING : '"' .*? '"' ;                // no escaping " for now

WS : [ \t\r\n]+ -> skip;

fragment
LETTER : [a-zA-Z];

fragment
DIGIT : [0-9];