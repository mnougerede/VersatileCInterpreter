//
// Created by max on 04/02/25.
//

#include "CInterpreterVisitor.h"

#include <CLexer.h>

#include "CParser.h"
#include <stdexcept>
#include <string>
#include "antlr4-runtime.h"
#include "EnvScopeGuard.h"
#include "Utils.h"
#include "ReturnException.h"

// Single-arg ctor: no token stream available
CInterpreterVisitor::CInterpreterVisitor(Environment* environment)
  : env(environment), tokens(nullptr) {}

// Two-arg ctor: store the pointer so later you can call tokens->getText(...)
CInterpreterVisitor::CInterpreterVisitor(Environment* environment,
                                         antlr4::CommonTokenStream* toks)
  : env(environment), tokens(toks) {}


// Destructor definition
CInterpreterVisitor::~CInterpreterVisitor() {
    // Any cleanup if needed
}

#define LOG(x) std::clog << x << std::endl;



std::any CInterpreterVisitor::visitAddSubExpression(CParser::AddSubExpressionContext *ctx) {
    VarValue left = std::any_cast<VarValue>(visit(ctx->multiplicativeExpression(0))); // First term

    for (size_t i = 0; i < ctx->addOp().size(); i++) {
        VarValue right = std::any_cast<VarValue>(visit(ctx->multiplicativeExpression(i + 1))); // Next term
        std::string op = ctx->addOp(i)->getText();

        // Use std::visit to perform the operation based on the actual type.
        left = std::visit([op](auto a, auto b) -> VarValue {
            using T = std::common_type_t<decltype(a), decltype(b)>;
            if (op == "+") {
                return static_cast<T>(a) + static_cast<T>(b);
            } else { // op == "-"
                return static_cast<T>(a) - static_cast<T>(b);
            }
        }, left, right);
    }

    return std::any(left);
}




std::any CInterpreterVisitor::visitMulDivExpression(CParser::MulDivExpressionContext *ctx) {
    VarValue left = std::any_cast<VarValue>(visit(ctx->unaryExpression(0)));

    for (size_t i = 0; i < ctx->mulOp().size(); i++) {
        VarValue right = std::any_cast<VarValue>(visit(ctx->unaryExpression(i + 1)));
        std::string op = ctx->mulOp(i)->getText();

        left = std::visit([op](auto a, auto b) -> VarValue {
            using T = std::common_type_t<decltype(a), decltype(b)>;
            if (op == "*") {
                return static_cast<T>(a) * static_cast<T>(b);
            } else {
                if (b == 0)
                    throw std::runtime_error("Division by zero");
                return static_cast<T>(a) / static_cast<T>(b);
            }
        }, left, right);
    }
    return std::any(left);
}

std::any CInterpreterVisitor::visitUnaryMinusExpression(CParser::UnaryMinusExpressionContext *ctx) {
    VarValue value = std::any_cast<VarValue>(visit(ctx->unaryExpression()));
    value = std::visit([](auto a) -> VarValue {
        return -a;
    }, value);
    return std::any(value);
}



std::any CInterpreterVisitor::visitVariableReference(CParser::VariableReferenceContext *ctx) {
    std::string varName = ctx->getText();
    Variable var = env->get(varName);
    return std::any(var.value);
}

std::any CInterpreterVisitor::visitDeclareVariable(CParser::DeclareVariableContext *ctx) {
    // Use the helper function to process the declaration.
    VarValue value = processDeclaration(ctx->typeSpecifier(), ctx->declarator(), ctx->expression());
    return value;
}


std::any CInterpreterVisitor::visitNumberLiteral(CParser::NumberLiteralContext *ctx) {
    // Get the number literal text.
    std::string text = ctx->getText(); // For example, "42" or "3.14"

    VarValue result;
    // If there's no decimal point, treat it as an integer.
    if (text.find('.') == std::string::npos) {
        result = std::stoi(text);
    } else {
        result = std::stod(text);
    }

    return std::any(result);
}

std::any CInterpreterVisitor::visitCharLiteral(CParser::CharLiteralContext *ctx) {
    // Assume the literal is in the form 'a'
    std::string text = ctx->getText(); // e.g., "'a'"
    // Convert the char literal to an int (its ASCII code)
    char value = static_cast<char>(text[1]);
    // Return as a NumValue variant containing an int
    VarValue result = value;
    return std::any(result);
}

std::any CInterpreterVisitor::aggregateResult(std::any aggregate, std::any nextResult) {
    if (!nextResult.has_value()) {
        return aggregate;
    }
    return nextResult;
}

std::any CInterpreterVisitor::visitReturnStmt(CParser::ReturnStmtContext *ctx) {
    // If there's an expression, evaluate it; otherwise default to 0 (or whatever void→int you like).
    VarValue rv = ctx->expression()
        ? std::any_cast<VarValue>(visit(ctx->expression()))
        : VarValue(0);
    // Throw it so the call‐site stops here.
    throw ReturnException(rv);
}

std::any CInterpreterVisitor::visitLogicalOrExpression(CParser::LogicalOrExpressionContext *ctx) {
    // Evaluate the first operand.
    VarValue result = std::any_cast<VarValue>(visit(ctx->logicalAndExpression(0)));

    // For each additional operand, perform logical OR.
    for (size_t i = 1; i < ctx->logicalAndExpression().size(); ++i) {
        VarValue operand = std::any_cast<VarValue>(visit(ctx->logicalAndExpression(i)));

        // Use std::visit to extract a Boolean (as int) from each VarValue.
        int leftBool = std::visit([](auto v) -> int {
            return (v != 0) ? 1 : 0;
        }, result);
        int rightBool = std::visit([](auto v) -> int {
            return (v != 0) ? 1 : 0;
        }, operand);

        // Perform logical OR.
        int combined = (leftBool || rightBool) ? 1 : 0;

        // Store the result back as a VarValue (an int).
        result = combined;
    }
    return std::any(result);
}


std::any CInterpreterVisitor::visitLogicalAndExpression(CParser::LogicalAndExpressionContext *ctx) {
    // Evaluate the first operand.
    VarValue result = std::any_cast<VarValue>(visit(ctx->equalityExpression(0)));

    // For each additional operand, perform logical AND.
    for (size_t i = 1; i < ctx->equalityExpression().size(); ++i) {
        VarValue operand = std::any_cast<VarValue>(visit(ctx->equalityExpression(i)));

        int leftBool = std::visit([](auto v) -> int {
            return (v != 0) ? 1 : 0;
        }, result);
        int rightBool = std::visit([](auto v) -> int {
            return (v != 0) ? 1 : 0;
        }, operand);

        int combined = (leftBool && rightBool) ? 1 : 0;
        result = combined;
    }
    return std::any(result);
}

std::any CInterpreterVisitor::visitEqualityExpression(CParser::EqualityExpressionContext *ctx) {
    // Evaluate the first relational expression.
    VarValue left = std::any_cast<VarValue>(visit(ctx->relationalExpression(0)));

    // Loop over each equality operator and the subsequent relational expression.
    for (size_t i = 0; i < ctx->equalityOp().size(); ++i) {
        VarValue right = std::any_cast<VarValue>(visit(ctx->relationalExpression(i + 1)));
        std::string op = ctx->equalityOp(i)->getText();

        left = std::visit([op](auto a, auto b) -> VarValue {
            // Compare using the appropriate operator.
            if (op == "==") {
                return (a == b) ? 1 : 0;
            } else if (op == "!=") {
                return (a != b) ? 1 : 0;
            } else {
                throw std::runtime_error("Unknown equality operator: " + op);
            }
        }, left, right);
    }
    return std::any(left);
}


std::any CInterpreterVisitor::visitRelationalExpression(CParser::RelationalExpressionContext *ctx) {
    VarValue left = std::any_cast<VarValue>(visit(ctx->additiveExpression(0)));

    // For each relational operator and right-hand additive expression, apply the operator.
    for (size_t i = 0; i < ctx->relationalOp().size(); ++i) {
        VarValue right = std::any_cast<VarValue>(visit(ctx->additiveExpression(i + 1)));
        std::string op = ctx->relationalOp(i)->getText();

        // Use std::visit to evaluate the relational operation.
        left = std::visit([op](auto a, auto b) -> VarValue {
            using T = std::common_type_t<decltype(a), decltype(b)>;
            if (op == "<") {
                return (static_cast<T>(a) < static_cast<T>(b)) ? 1 : 0;
            } else if (op == ">") {
                return (static_cast<T>(a) > static_cast<T>(b)) ? 1 : 0;
            } else if (op == "<=") {
                return (static_cast<T>(a) <= static_cast<T>(b)) ? 1 : 0;
            } else if (op == ">=") {
                return (static_cast<T>(a) >= static_cast<T>(b)) ? 1 : 0;
            } else {
                throw std::runtime_error("Unknown relational operator: " + op);
            }
        }, left, right);
    }

    // Return the result wrapped in std::any.
    return std::any(left);
}



std::any CInterpreterVisitor::visitLogicalNotExpression(CParser::LogicalNotExpressionContext *ctx) {
    // Evaluate the operand of the '!' operator.
    VarValue operand = std::any_cast<VarValue>(visit(ctx->unaryExpression()));

    // Use std::visit to convert the operand to a Boolean value:
    // If the operand is nonzero, logical NOT yields 0; if it is 0, logical NOT yields 1.
    int boolResult = std::visit([](auto v) -> int {
        return (v != 0) ? 0 : 1;
    }, operand);

    // Return the Boolean result wrapped in a VarValue.
    return std::any(VarValue(boolResult));
}



std::any CInterpreterVisitor::visitAssignmentExpr(CParser::AssignmentExprContext *ctx) {

    std::string varName = ctx->unaryExpression()->getText();
    std::any rightResult = visit(ctx->assignmentExpression());

    Variable var = env->get(varName);
    VarType varType = var.type;

    VarValue newValue;
    if (varType == VarType::INT) {
        try {
            VarValue temp = std::any_cast<VarValue>(rightResult);
            if (std::holds_alternative<int>(temp)) {
                newValue = std::get<int>(temp);
            } else if (std::holds_alternative<double>(temp)) {
                newValue = static_cast<int>(std::get<double>(temp));
            } else if (std::holds_alternative<char>(temp)) {
                newValue = static_cast<int>(std::get<char>(temp));
            } else {
                throw std::runtime_error("Unexpected type in VarValue for int assignment");
            }
        } catch (const std::bad_any_cast&) {
            if (rightResult.type() == typeid(char)) {
                newValue = static_cast<int>(std::any_cast<char>(rightResult));
            } else if (rightResult.type() == typeid(int)) {
                newValue = std::any_cast<int>(rightResult);
            } else if (rightResult.type() == typeid(double)) {
                newValue = static_cast<int>(std::any_cast<double>(rightResult));
            } else {
                throw std::runtime_error("Assignment type mismatch for int");
            }
        }
    } else if (varType == VarType::FLOAT || varType == VarType::DOUBLE) {
        try {
            VarValue temp = std::any_cast<VarValue>(rightResult);
            if (std::holds_alternative<double>(temp))
                newValue = std::get<double>(temp);
            else if (std::holds_alternative<int>(temp)) {
                newValue = static_cast<double>(std::get<int>(temp));
            }
            else if (std::holds_alternative<char>(temp)) {
                newValue = static_cast<double>(std::get<char>(temp));
            } else {
                throw std::runtime_error("Unexpected type in VarValue for double assignment");
            }
        } catch (const std::bad_any_cast&) {
            if (rightResult.type() == typeid(char))
                newValue = static_cast<double>(std::any_cast<char>(rightResult));
            else if (rightResult.type() == typeid(int))
                newValue = static_cast<double>(std::any_cast<int>(rightResult));
            else if (rightResult.type() == typeid(double))
                newValue = std::any_cast<double>(rightResult);
            else
                throw std::runtime_error("Assignment type mismatch for double");
        }
    } else if (varType == VarType::CHAR) {
        try {
            VarValue temp = std::any_cast<VarValue>(rightResult);
            if (std::holds_alternative<char>(temp))
                newValue = std::get<char>(temp);
            else if (std::holds_alternative<int>(temp)) {
                newValue = static_cast<char>(std::get<int>(temp));
            }
            else if (std::holds_alternative<double>(temp)) {
                newValue = static_cast<char>(std::get<char>(temp));
            } else {
                throw std::runtime_error("Unexpected type in VarValue for char assignment");
            }
        } catch (const std::bad_any_cast&) {
            if (rightResult.type() == typeid(double))
                newValue = static_cast<char>(std::any_cast<double>(rightResult));
            else if (rightResult.type() == typeid(int))
                newValue = static_cast<char>(std::any_cast<int>(rightResult));
            else if (rightResult.type() == typeid(char))
                newValue = std::any_cast<char>(rightResult);
            else
                throw std::runtime_error("Assignment type mismatch for double");
        }


    } else {
        throw std::runtime_error("Unsupported variable type for assignment");
    }
    env->assign(varName, varType, newValue);

    return std::any(newValue);
}

std::any CInterpreterVisitor::visitIfElseStatement(CParser::IfElseStatementContext *ctx) {
    VarValue condVar = std::any_cast<VarValue>(visit(ctx->expression()));
    bool conditionTrue = convertToBool(condVar);  // Convert your VarValue to bool.

    if (conditionTrue) {
        // Evaluate and return the value from the 'then' branch.
        return visit(ctx->statement(0));
    } else if (ctx->ELSE() != nullptr) {
        // Evaluate and return the value from the 'else' branch.
        return visit(ctx->statement(1));
    }
    return std::any();
}

std::any CInterpreterVisitor::visitWhileStatement(CParser::WhileStatementContext *ctx) {
    // Evaluate the condition expression and convert to bool.
    while (convertToBool(std::any_cast<VarValue>(visit(ctx->expression())))) {
        // Execute the loop body.
        visit(ctx->statement());
    }
    // Return an empty std::any since the loop itself produces no value.
    return std::any();
}

std::any CInterpreterVisitor::visitDoWhileStatement(CParser::DoWhileStatementContext *ctx) {
    do {
        visit(ctx->statement());
    } while (convertToBool(std::any_cast<VarValue>(visit(ctx->expression()))));
    return std::any();
}

std::any CInterpreterVisitor::visitForStatement(CParser::ForStatementContext *ctx) {
    LOG("Entering for loop.");
    // Push a new scope for the for loop.
    Environment* loopEnv = env->pushScope();
    Environment* previousEnv = env;
    env = loopEnv;
    LOG("New loop scope pushed.");

    // Get the forLoop components.
    ForLoopComponents comps = std::any_cast<ForLoopComponents>(visit(ctx->forCondition()));

    // --- Initializer ---
    if (comps.initializer.has_value()) {
        if (comps.initializer.type() == typeid(CParser::ForDeclarationContext*)) {
            auto declCtx = std::any_cast<CParser::ForDeclarationContext*>(comps.initializer);
            LOG("Running for loop initializer (declaration): " << declCtx->getText());
            visit(declCtx);
        } else if (comps.initializer.type() == typeid(CParser::ExpressionContext*)) {
            auto exprCtx = std::any_cast<CParser::ExpressionContext*>(comps.initializer);
            LOG("Running for loop initializer (expression): " << exprCtx->getText());
            visit(exprCtx);
        } else {
            LOG("Unknown initializer type.");
        }
    } else {
        LOG("No initializer provided.");
    }


    // --- Condition ---
    bool condition = true;
    if (comps.condition.has_value()) {
        // Check if the condition is stored as a pointer.
        if (auto condCtx = std::any_cast<CParser::ForConditionExpressionContext*>(comps.condition); condCtx != nullptr) {
            std::any condResult = visit(condCtx);
            condition = convertToBool(std::any_cast<VarValue>(condResult));
            LOG("Initial condition evaluated from expression: " << (condition ? "true" : "false"));
        } else {
            // Otherwise, assume it's an explicit VarValue.
            condition = convertToBool(std::any_cast<VarValue>(comps.condition));
            LOG("Initial condition evaluated as explicit true value: " << (condition ? "true" : "false"));
        }
    } else {
        condition = true;
        LOG("No condition provided; defaulting to true.");
    }

    // --- Loop Body and Update ---
    while (condition) {
        LOG("Loop iteration begins. Condition is true.");
        // Execute the loop body.
        visit(ctx->statement());

        // Process the update part, if provided.
        if (comps.update.has_value()) {
            if (auto updateCtx = std::any_cast<CParser::ForUpdateExpressionContext*>(comps.update); updateCtx != nullptr) {
                LOG("Executing update expression: " << updateCtx->getText());
                visit(updateCtx);
            }
        } else {
            LOG("No update expression provided.");
        }

        // Reevaluate the condition.
        if (comps.condition.has_value()) {
            if (auto condCtx = std::any_cast<CParser::ForConditionExpressionContext*>(comps.condition); condCtx != nullptr) {
                std::any condResult = visit(condCtx);
                condition = convertToBool(std::any_cast<VarValue>(condResult));
                LOG("Reevaluated condition from expression: " << (condition ? "true" : "false"));
            } else {
                condition = convertToBool(std::any_cast<VarValue>(comps.condition));
                LOG("Reevaluated condition from explicit value: " << (condition ? "true" : "false"));
            }
        } else {
            condition = true;
            LOG("No condition; defaulting to true.");
        }
    }
    LOG("For loop finished; condition is false. Exiting loop.");

    // Pop the scope when done.
    env = env->popScope();
    delete loopEnv; // or handle via smart pointers
    LOG("Loop scope popped. Exiting for loop.");

    // The for loop itself does not produce a meaningful value.
    return std::any();
}

std::any CInterpreterVisitor::visitForCondition(CParser::ForConditionContext *ctx) {
    LOG("Entered visitForCondition.");
    ForLoopComponents comps;

    // Initializer: either a forDeclaration or an expression.
    if (ctx->forDeclaration() != nullptr) {
        comps.initializer = static_cast<CParser::ForDeclarationContext*>(ctx->forDeclaration());
    } else if (ctx->expression() != nullptr) {
        comps.initializer = static_cast<CParser::ExpressionContext*>(ctx->expression());
    } else {
        comps.initializer = std::any();
    }

    // Condition: store pointer if provided; otherwise, store an explicit true value.
    if (ctx->forConditionExpression() != nullptr) {
        comps.condition = static_cast<CParser::ForConditionExpressionContext*>(ctx->forConditionExpression());
    } else {
        // Wrap an explicit true value (assuming VarValue(1) means true).
        comps.condition = VarValue(1);
    }

    // Update: store pointer if provided.
    if (ctx->forUpdateExpression() != nullptr) {
        comps.update = static_cast<CParser::ForUpdateExpressionContext*>(ctx->forUpdateExpression());
    } else {
        comps.update = std::any();
    }
    LOG("Exiting visitForCondition with components: "
            << "Initializer: " << (comps.initializer.has_value() ? "set" : "empty")
            << ", Condition: " << (comps.condition.has_value() ? "set" : "empty")
            << ", Update: " << (comps.update.has_value() ? "set" : "empty"));
    return comps;
}

std::any CInterpreterVisitor::visitForDeclaration(CParser::ForDeclarationContext *ctx) {
    VarValue value = processDeclaration(ctx->typeSpecifier(), ctx->declarator(), ctx->expression());
    return value;
}





std::any CInterpreterVisitor::visitParenthesizedExpression(CParser::ParenthesizedExpressionContext *ctx) {
    return visit(ctx->expression());
}


VarValue CInterpreterVisitor::processDeclaration(CParser::TypeSpecifierContext* typeCtx,
                                                   CParser::DeclaratorContext* declCtx,
                                                   CParser::ExpressionContext* exprCtx) {
    // Get variable name and type string.
    std::string varName = declCtx->getText();
    std::string typeStr = typeCtx->getText();
    VarType varType;

    // Determine the variable type.
    if (typeStr == "int") {
        varType = VarType::INT;
    } else if (typeStr == "float") {
        varType = VarType::FLOAT;
    } else if (typeStr == "double") {
        varType = VarType::DOUBLE;
    } else if (typeStr == "char") {
        varType = VarType::CHAR;
    } else if (typeStr == "void") {
        throw std::runtime_error("Cannot declare variable of type void");
    } else {
        throw std::runtime_error("Unknown type: " + typeStr);
    }

    // Evaluate the initializer expression if provided.
    VarValue varValue;
    if (exprCtx != nullptr) {
        std::any initResult = visit(exprCtx);
        varValue = std::any_cast<VarValue>(initResult);
    } else {
        // Default initialization based on type.
        if (varType == VarType::INT) {
            varValue = 0;
        } else if (varType == VarType::FLOAT || varType == VarType::DOUBLE) {
            varValue = 0.0;
        } else if (varType == VarType::CHAR) {
            varValue = '\0';
        }
    }

    // Add the variable to the current environment.
    env->define(varName, varType, varValue);
    return varValue;
}

std::any CInterpreterVisitor::visitFunctionDefinition(CParser::FunctionDefinitionContext *ctx) {
    // Extract function name and return type.
    std::string funcName = ctx->IDENTIFIER()->getText();
    std::string returnType = ctx->typeSpecifier()->getText();

    // Process the parameter list.
    std::vector<std::string> params;
    if (auto *pl = ctx->parameterList()) {
        for (auto *p : pl->parameter())
            params.push_back(p->IDENTIFIER()->getText());
    }

    auto *charStream = tokens->getTokenSource()->getInputStream();
    auto startIndex  = ctx->compoundStatement()->getStart()->getStartIndex();
    auto stopIndex   = ctx->compoundStatement()->getStop()->getStopIndex();
    antlr4::misc::Interval interval(startIndex, stopIndex);
    std::string bodyText = charStream->getText(interval);

    // Create a function object.
    Function func;
    func.returnType = returnType;
    func.parameterNames = std::move(params);
    func.bodyText      = std::move(bodyText);


    // Register the function in the current environment.
    env->defineFunction(funcName, func);

    // void return value
    return std::any();
}

std::any CInterpreterVisitor::visitPostfixExpression(CParser::PostfixExpressionContext *ctx) {
    // If it’s just a primary expression, delegate:
    if (ctx->children.size() == 1) {
        return visit(ctx->primaryExpression());
    }

    // Otherwise, treat it as a function call:
    std::string funcName = ctx->primaryExpression()->getText();

    Function* func = env->getFunction(funcName);
    if (!func) {
        throw std::runtime_error("Function '" + funcName + "' is not defined.");
    }

    // 1) Evaluate the arguments into VarValue
    std::vector<VarValue> args;
    if (!ctx->argumentExpressionList().empty()) {
        auto argListCtx = ctx->argumentExpressionList().front();
        for (auto exprCtx : argListCtx->assignmentExpression()) {
            args.push_back(std::any_cast<VarValue>(visit(exprCtx)));
        }
    }
    if (args.size() != func->parameterNames.size()) {
        throw std::runtime_error(
            "Function '" + funcName + "' expects " +
            std::to_string(func->parameterNames.size()) +
            " arguments but got " +
            std::to_string(args.size()));
    }

    // 2) Create a new scope, bind parameters, then re‐parse & invoke the stored body
    {
        EnvScopeGuard guard(env);

        // Bind parameters (assumes all int for now; adapt if you store types)
        for (size_t i = 0; i < args.size(); ++i) {
            env->define(func->parameterNames[i], VarType::INT, args[i]);
        }

        // Re‐lex and parse the saved compound statement text:
        antlr4::ANTLRInputStream bodyIn(func->bodyText);
        CLexer                   bodyLex(&bodyIn);
        antlr4::CommonTokenStream bodyTokens(&bodyLex);
        CParser                  bodyParser(&bodyTokens);
        auto bodyCtx = bodyParser.compoundStatement();

        // Visit it and catch a ReturnException
        try {
            return visit(bodyCtx);
        } catch (const ReturnException &retEx) {
            return retEx.getValue();
        }
    }
}
