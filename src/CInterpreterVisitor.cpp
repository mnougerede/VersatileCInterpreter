//
// Created by max on 04/02/25.
//

#include "CInterpreterVisitor.h"
#include "CParser.h"
#include <stdexcept>
#include <string>

std::any CInterpreterVisitor::visitEvaluateExpression(CParser::EvaluateExpressionContext *ctx) {
    return visit(ctx->expression());
}

std::any CInterpreterVisitor::visitAddSubExpression(CParser::AddSubExpressionContext *ctx) {
    double result = std::any_cast<double>(visit(ctx->term(0))); // First term

    for (size_t i = 0; i < ctx->addOp().size(); i++) {
        double nextValue = std::any_cast<double>(visit(ctx->term(i + 1))); // Next term
        std::string op = ctx->addOp(i)->getText();

        if (op == "+") {
            result += nextValue;
        } else if (op == "-") {
            result -= nextValue;
        }
    }
    return result;
}
std::any CInterpreterVisitor::visitMulDivExpression(CParser::MulDivExpressionContext *ctx) {
    double result = std::any_cast<double>(visit(ctx->factor(0))); // First factor

    for (size_t i = 0; i < ctx->mulOp().size(); i++) {
        double nextValue = std::any_cast<double>(visit(ctx->factor(i + 1))); // Next factor
        std::string op = ctx->mulOp(i)->getText();

        if (op == "*") {
            result *= nextValue;
        } else if (op == "/") {
            if (nextValue == 0) {
                throw std::runtime_error("Division by zero");
            }
            result /= nextValue;
        }
    }
    return result;
}

std::any CInterpreterVisitor::visitUnaryMinusExpression(CParser::UnaryMinusExpressionContext *ctx) {
    double value = std::any_cast<double>(visit(ctx->factor()));
    return -value;
}

std::any CInterpreterVisitor::visitParenthesizedExpression(CParser::ParenthesizedExpressionContext *ctx) {
    return visit(ctx->expression());
}

std::any CInterpreterVisitor::visitNumberExpression(CParser::NumberExpressionContext *ctx) {
    return std::stod(ctx->Number()->getText());
}