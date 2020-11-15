grammar bluefin;

program : (funcDef | varDecl | structDef)+ ;

funcDef : Virtual? type ID '(' paramList? ')' Override?  '{' stmt* '}' ;

type : builtinType
     | ID ;

builtinType : TInt | TFloat | TString | TBool | TVoid ;

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

structDef : 'struct' ID  (superClass)? '{' (varDecl | funcDef)* '}' ';' ;
// A varDecl in a struct is known as a 'field'
// A funcDef in a struct is known as a 'method'

superClass : 'extends' ID;

varDecl : type ID ('=' expr)? ';' ;

/* ANTLR is left-associative by default and resolves ambiguity by selecting top-most alternative
It also resolves direct left recrusion so don't need to manually remove them
This means that precedence and associativeness ambiguities are solved
TODO: handle power (right associative)
*/
expr : INT                                  # primaryInt
     | FLOAT                                # primaryFloat
     | STRING                               # primaryString
     | ID                                   # primaryId
     | BOOL                                 # primaryBool
     | '(' expr ')'                         # primaryParenth
     | ID '(' argList? ')'                  # funcCall
     | expr '.' ID '(' argList? ')'         # methodCall
     | expr '.' ID                          # memberAccess
     | op=('-' | '!') expr                  # unaryExpr
     | expr op=('*' | '/') expr                # multiExpr
     | expr op=('+' | '-') expr                # addExpr
     | expr op=('<' | '<=' | '>' | '>=') expr  # relExpr
     | expr op=('==' | '!=') expr              # equalityExpr
     | expr ('&&') expr                     # logicalANDExpr
     | expr ('||') expr                     # logicalORExpr
     |<assoc=right> expr '=' expr           # simpleAssignExpr
     ;

argList : expr (',' expr)* ;


TInt : 'int' ;

TFloat : 'float' ;

TString : 'string' ;

TBool : 'bool' ;

TVoid : 'void' ;

FLOAT: DIGIT+ '.' DIGIT+ ;

BOOL: 'true' | 'false' ;

STRING : '"' .*? '"' ;                // no escaping " for now

Override: 'override' ;

Virtual: 'virtual' ;

ID : LETTER+;     // only characters

INT : DIGIT+;       // even '007' and '000' are ints


WS : [ \t\r\n]+ -> skip;
LINE_COMMENT : '//' .*? '\n' -> skip;
MULTILINE_COMMENT : '/*'  .*? '*/' -> skip;

fragment
LETTER : [a-zA-Z];

fragment
DIGIT : [0-9];