grammar bluefin;

program : (funcDef | varDef)+ ;

funcDef : type ID '(' ')' block
       | type ID '(' parameterList ')' block
       ;

type : 'int' | 'double' | 'string' ;

parameterList : parameter (',' parameter)* ;

parameter : type ID;

block : '{' stmt* '}' ;

stmt : varDef
     | stmtIf
     | stmtWhile
     | stmtAssign
     | stmtReturn
     | funcCall
     ;

varDef : type ID '=' expr ';' ;

/* ANTLR is left-associative by default and resolves ambiguity by selecting top-most alternative
It also resolves direct left recrusion so don't need to manually remove them
This means that precedence and associativeness ambiguities are avoided
TODO: handle negatives and power (right associative)
*/
expr : expr ('*' | '/') expr
     | expr ('+' | '-') expr
     | ID
     | INT
     | DOUBLE
     | STRING
     ;

stmtIf : 'if' '(' expr ')' block ('else' block)? ;

stmtWhile : 'while' '(' expr ')' block;

stmtAssign : ID '=' expr ';' ;

stmtReturn : 'return' expr ';' ;

funcCall : ID '(' ')'
         | ID '(' exprList ')'
         ;

exprList : expr (',' expr)* ;

ID : LETTER+;     // only characters

LETTER : [a-zA-Z];

INT : DIGIT+;       // even '007' and '000' are ints

DIGIT : [0-9];

DOUBLE : INT '.' INT ;           // bsically an INT with a fractional part

STRING : '"' .*? '"' ;                // no escaping " for now

WS : [ \t\r\n]+ -> skip;
