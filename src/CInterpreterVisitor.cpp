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
    std::any result = visit(ctx->expression());
    return result;
}

std::any CInterpreterVisitor::visitAddSubExpression(CParser::AddSubExpressionContext *ctx) {
    VarValue left = std::any_cast<VarValue>(visit(ctx->term(0))); // First term

    for (size_t i = 0; i < ctx->addOp().size(); i++) {
        VarValue right = std::any_cast<VarValue>(visit(ctx->term(i + 1))); // Next term
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
    VarValue left = std::any_cast<VarValue>(visit(ctx->factor(0)));

    for (size_t i = 0; i < ctx->mulOp().size(); i++) {
        VarValue right = std::any_cast<VarValue>(visit(ctx->factor(i + 1)));
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
    VarValue value = std::any_cast<VarValue>(visit(ctx->factor()));
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
        //TODO ?? implement float type ??
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


std::any CInterpreterVisitor::visitParenthesizedExpression(CParser::ParenthesizedExpressionContext *ctx) {
    return visit(ctx->expression());
}




