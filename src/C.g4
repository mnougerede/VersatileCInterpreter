grammar C;

options { visitor=true; }

// Top-level rule for the REPL
replInput
    : expression EOF                # EvaluateExpression
    | statement+ EOF                # ExecuteStatements
    | functionDefinition+           # AddFunctionDefinitions
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
    | 'char'
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

// A declaration with optional initialiser
declaration
    : typeSpecifier declarator ('=' expression)? ';'    # DeclareVariable
    ;

// A simple declarator is just an identifier.
declarator
    : IDENTIFIER
    ;
iterationStatement
    : WHILE '(' expression ')' statement                        #WhileStatement
    | DO statement WHILE '(' expression ')' ';'                  #DoWhileStatement
    | FOR '(' forCondition ')' statement                         #ForStatement
    ;
// A for loop header can either start with a declaration or an optional expression.
forCondition
    : (forDeclaration | expression?) ';' forExpression? ';' forExpression?
    ;

// For the case where you declare a variable.
forDeclaration
    : typeSpecifier declarator ('=' expression)?
    ;

// The expression parts can be a list of expressions separated by commas.
forExpression
    : assignmentExpression (',' assignmentExpression)*
    ;
// A statement can be an expression statement or a compound statement - others to be added as needed.
statement
    : expressionStatement
    | compoundStatement
    | declaration
    | selectionStatement
    | iterationStatement
    ;

// An expression statement is an expression followed by a semicolon. It may be empty.
expressionStatement
    : expression? ';'
    ;
selectionStatement
    : IF '(' expression ')' statement (ELSE statement)?         #IfElseStatement
    | SWITCH '(' expression ')' statement                       #SwitchStatment
    ;
// Define expressions using operator precedence

expression
    : assignmentExpression
    ;
assignmentExpression
    : logicalOrExpression                           # LogicalOrExpr
    | unaryExpression ASSIGN assignmentExpression   # AssignmentExpr
    ;

logicalOrExpression
    : logicalAndExpression ( OR logicalAndExpression )*
    ;
logicalAndExpression
    : equalityExpression ( AND equalityExpression )*
    ;
equalityExpression
    : relationalExpression ( equalityOp relationalExpression )*
    ;
equalityOp
    : EQ
    | NEQ
    ;
relationalExpression
    : additiveExpression ( relationalOp additiveExpression )*
    ;
relationalOp
    : LT
    | GT
    | LTE
    | GTE
    ;
additiveExpression
    : multiplicativeExpression ( addOp multiplicativeExpression)*   # AddSubExpression
    ;
addOp
    : PLUS
    | MINUS
    ;
multiplicativeExpression
    : unaryExpression (mulOp unaryExpression)*          # MulDivExpression
    ;
mulOp
    : TIMES
    | DIV
    ;
unaryExpression
    : MINUS unaryExpression                         # UnaryMinusExpression
    | NOT unaryExpression                           # LogicalNotExpression
    | primaryExpression                             # PrimaryExpr
    ;
primaryExpression
    : '(' expression ')'                   # ParenthesizedExpression
    | literal                              # LiteralExpression
    | IDENTIFIER                           # VariableReference
    ;

literal
    : Number                        # NumberLiteral
    | CharLiteral                   # CharLiteral
    ;

// Program components
//translationUnit
//    : externalDeclaration+
//    ;

// Define other grammar rules here...



// Tokens
WHILE : 'while';
DO    : 'do';
FOR   : 'for';

IF          : 'if';
ELSE        : 'else';
SWITCH      : 'switch';
PLUS        : '+' ;
MINUS       : '-' ;
TIMES       : '*' ;
DIV         : '/' ;

ASSIGN  : '=';

OR      : '||';
AND     : '&&';
EQ      : '==';
NEQ     : '!=';
LT      : '<';
GT      : '>';
LTE     : '<=';
GTE     : '>=';

NOT     : '!';

CharLiteral : '\'' . '\'' ;
IDENTIFIER  : [a-zA-Z_][a-zA-Z0-9_]* ;
Number
    : [0-9]+ ('.' [0-9]+)?                 // Integer or floating-point number
    ;

// Ignore whitespace
WHITESPACE
    : [ \t\r\n]+ -> skip
    ;