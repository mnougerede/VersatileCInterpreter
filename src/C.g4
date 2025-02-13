grammar C;

options { visitor=true; }

// Top-level rule for the REPL
replInput
    : expression                # EvaluateExpression
//    | statement+                # ExecuteStatements
//    | functionDefinition+       # AddFunctionDefinitions
//    | translationUnit           # ExecuteFullProgram
    ;

// Define expressions
expression
    : term ( addOp term)*   # AddSubExpression
    ;
addOp
    : PLUS
    | MINUS
    ;
term
    : factor (mulOp factor)*          # MulDivExpression
    ;
mulOp
    : TIMES
    | DIV
    ;
factor
    : MINUS factor                         # UnaryMinusExpression
    | '(' expression ')'                   # ParenthesizedExpression
    | Number                               # NumberExpression
    ;

// Statements
//statement
//    : declaration
//    | expressionStatement
//    | compoundStatement
////    | ...
//    ;

// Program components
//translationUnit
//    : externalDeclaration+
//    ;

// Define other grammar rules here...



// Tokens
PLUS   : '+' ;
MINUS  : '-' ;
TIMES  : '*' ;
DIV    : '/' ;
IDENTIFIER : [a-zA-Z_][a-zA-Z0-9_]* ;
Number
    : [0-9]+ ('.' [0-9]+)?                 // Integer or floating-point number
    ;

// Ignore whitespace
WHITESPACE
    : [ \t\r\n]+ -> skip
    ;