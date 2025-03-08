//
// Created by max on 04/02/25.
//

#include "CInterpreterVisitor.h"
#include "CParser.h"
#include <stdexcept>
#include <string>

// Constructor definition
CInterpreterVisitor::CInterpreterVisitor(Environment* environment)
    : env(environment) {
}

// Destructor definition
CInterpreterVisitor::~CInterpreterVisitor() {
    // Any cleanup if needed
}

std::any CInterpreterVisitor::visitEvaluateExpression(CParser::EvaluateExpressionContext *ctx) {
    return visit(ctx->expression());
}

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
    std::string varName = ctx->declarator()->getText();
    std::string typeStr = ctx->typeSpecifier()->getText();
    VarType varType;
    if (typeStr == "int") {
        varType = VarType::INT;
    } else if (typeStr == "float") {
        varType = VarType::FLOAT;  // Treating float as double for storage.
    } else if (typeStr == "double") {
        varType = VarType::DOUBLE;
    } else if (typeStr == "char") {
        varType = VarType::CHAR;
    } else if (typeStr == "void") {
        throw std::runtime_error("Cannot declare variable of type void");
    } else {
        throw std::runtime_error("Unknown type: " + typeStr);
    }

    // Evaluate the initializer if provided.
    VarValue varValue;
    if (ctx->expression()) {
        std::any initResult = visit(ctx->expression());
        varValue = std::any_cast<VarValue>(initResult);
    } else {
        // Default initialization.
        if (varType == VarType::INT) {
            varValue = 0;
        } else if (varType == VarType::FLOAT || varType == VarType::DOUBLE) {
            varValue = 0.0;
        } else if (varType == VarType::CHAR) {
            varValue = '\0';
        }
    }

    env->set(varName, varType, varValue);

    return varValue;
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
    env->set(varName, varType, newValue);

    return std::any(newValue);
}

std::any CInterpreterVisitor::visitIfElseStatement(CParser::IfElseStatementContext *ctx) {
    return CBaseVisitor::visitIfElseStatement(ctx);
}


std::any CInterpreterVisitor::visitParenthesizedExpression(CParser::ParenthesizedExpressionContext *ctx) {
    return visit(ctx->expression());
}




