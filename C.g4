grammar C;

// Top-level rule for the REPL
replInput
    : expression                # EvaluateExpression
//    | statement+                # ExecuteStatements
//    | functionDefinition+       # AddFunctionDefinitions
//    | translationUnit           # ExecuteFullProgram
    ;

// Define expressions
expression
    : expression op=('*'|'/') expression   # MulDivExpression
    | expression op=('+'|'-') expression   # AddSubExpression
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
Number
    : [0-9]+ ('.' [0-9]+)?                 // Integer or floating-point number
    ;

// Ignore whitespace
WHITESPACE
    : [ \t\r\n]+ -> skip
    ;