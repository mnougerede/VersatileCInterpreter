grammar C;

options { visitor=true; }

// Top-level rule for the REPL
replInput
    : expression EOF                # EvaluateExpression
    | statement+ EOF                # ExecuteStatements
    | functionDefinition+       # AddFunctionDefinitions
//    | translationUnit           # ExecuteFullProgram
    ;

// A minimal function definition rule.
functionDefinition
    : typeSpecifier IDENTIFIER '(' parameterList? ')' compoundStatement
    ;

// Minimal type specifiers. Add others as needed.
typeSpecifier
    : 'int'
    | 'float'
    | 'double'
    | 'void'
    ;

parameterList
    : parameter (',' parameter)*
    ;

parameter
    : typeSpecifier IDENTIFIER
    ;

compoundStatement
    : '{' declaration* statement* '}'
    ;

// A declaration is simplified: type specifier, declarator, and semicolon.
declaration
    : typeSpecifier declarator ';'
    ;

// A simple declarator is just an identifier.
declarator
    : IDENTIFIER
    ;

// A statement can be an expression statement or a compound statement - others to be added as needed.
statement
    : expressionStatement
    | compoundStatement
    | declaration
    ;

// An expression statement is an expression followed by a semicolon. It may be empty.
expressionStatement
    : expression? ';'
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