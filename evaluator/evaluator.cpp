#include "evaluator.h"
#include <memory>
#include <string>
#include <vector>
#include <iostream>
#include <sstream>

namespace evaluator {

using object::Object;
using object::Environment;
using object::Integer;
using object::Boolean;
using object::Null;
using object::ReturnValue;
using object::Error;
using object::Function;

static std::shared_ptr<Boolean> TRUE = std::make_shared<Boolean>(true);
static std::shared_ptr<Boolean> FALSE = std::make_shared<Boolean>(false);
static std::shared_ptr<Null> NULL_OBJ = std::make_shared<Null>();

std::shared_ptr<Object> eval(std::shared_ptr<ast::Node> node, std::shared_ptr<Environment> env) {
    if (auto program = std::dynamic_pointer_cast<ast::Program>(node)) {
        return evalProgram(program->statements, env);
    } else if (auto stmt = std::dynamic_pointer_cast<ast::ExpressionStatement>(node)) {
        return eval(stmt->expression, env);
    } else if (auto intLit = std::dynamic_pointer_cast<ast::IntegerLiteral>(node)) {
        return std::make_shared<Integer>(intLit->value);
    } else if (auto boolLit = std::dynamic_pointer_cast<ast::Boolean>(node)) {
        return nativeBoolToBooleanObject(boolLit->value);
    } else if (auto prefix = std::dynamic_pointer_cast<ast::PrefixExpression>(node)) {
        auto right = eval(prefix->right, env);
        if (isError(right)) {
            return right;
        }
        return evalPrefixExpression(prefix->op, right);
    } else if (auto infix = std::dynamic_pointer_cast<ast::InfixExpression>(node)) {
        auto left = eval(infix->left, env);
        if (isError(left)) {
            return left;
        }
        auto right = eval(infix->right, env);
        if (isError(right)) {
            return right;
        }
        return evalInfixExpression(infix->op, left, right);
    } else if (auto ifExp = std::dynamic_pointer_cast<ast::IfExpression>(node)) {
        return evalIfExpression(ifExp, env);
    } else if (auto blockStmt = std::dynamic_pointer_cast<ast::BlockStatement>(node)) {
        return evalBlockStatement(blockStmt, env);
    } else if (auto returnStmt = std::dynamic_pointer_cast<ast::ReturnStatement>(node)) {
        auto val = eval(returnStmt->returnValue, env);
        if (isError(val)) return val;
        return std::make_shared<ReturnValue>(val);
    } else if (auto letStmt = std::dynamic_pointer_cast<ast::LetStatement>(node)) {
        auto val = eval(letStmt->value, env);
        if (isError(val)) return val;
        env->set(letStmt->name->value, val);
        return val;
    } else if (auto ident = std::dynamic_pointer_cast<ast::Identifier>(node)) {
        return evalIdentifier(ident, env);
    } else if (auto funcLit = std::dynamic_pointer_cast<ast::FunctionLiteral>(node)) {
        auto params = funcLit->parameters;
        auto body = funcLit->body;
        return std::make_shared<Function>(params, body, env);
    } else if (auto callExp = std::dynamic_pointer_cast<ast::CallExpression>(node)) {
        auto function = eval(callExp->function, env);
        if (isError(function)) return function;
        auto args = evalExpressions(callExp->arguments, env);
        if (args.size() == 1 && isError(args[0])) return args[0];
        return applyFunction(function, args);
    } else {
        return nullptr;
    }
}

std::shared_ptr<Object> evalProgram(std::vector<std::shared_ptr<ast::Statement>> stmts, std::shared_ptr<Environment> env) {
    std::shared_ptr<Object> result;

    for (auto stmt : stmts) {
        result = eval(stmt, env);
        if (result && result->type() == object::ObjectType::RETURN_VALUE_OBJ) {
            return std::dynamic_pointer_cast<ReturnValue>(result)->value;
        } else if (result && result->type() == object::ObjectType::ERROR_OBJ) {
            return result;
        }
    }
    return result;
}

std::shared_ptr<Boolean> nativeBoolToBooleanObject(bool input) {
    if (input) {
        return TRUE;
    }
    return FALSE;
}

bool isError(std::shared_ptr<Object> obj) {
    if (obj != nullptr) {
        return obj->type() == object::ObjectType::ERROR_OBJ;
    }
    return false;
}

std::shared_ptr<Object> evalPrefixExpression(std::string op, std::shared_ptr<Object> right) {
    if (op == "!") {
        if (right == FALSE) {
            return TRUE;
        } else {
            return FALSE;
        }
    } else if (op == "-") {
        if (right->type() != object::ObjectType::INTEGER_OBJ) {
            return std::make_shared<Error>("unknown operator: -" + object::objectTypeToString(right->type()));
        }
        auto value = std::static_pointer_cast<Integer>(right)->value;
	    return std::make_shared<Integer>(-value);
    }
    return std::make_shared<Error>("unknown operator: " + op + object::objectTypeToString(right->type()));
}

std::shared_ptr<Object> evalInfixExpression(std::string op, std::shared_ptr<Object> left, std::shared_ptr<Object> right) {
    if (left->type() == object::ObjectType::INTEGER_OBJ && right->type() == object::ObjectType::INTEGER_OBJ) {
        auto leftVal = std::static_pointer_cast<Integer>(left)->value;
        auto rightVal = std::static_pointer_cast<Integer>(right)->value;

        if (op == "+") return std::make_shared<Integer>(leftVal + rightVal);
        if (op == "-") return std::make_shared<Integer>(leftVal - rightVal);
        if (op == "*") return std::make_shared<Integer>(leftVal * rightVal);
        if (op == "/") return std::make_shared<Integer>(leftVal / rightVal);
        if (op == "<") return nativeBoolToBooleanObject(leftVal < rightVal);
        if (op == ">") return nativeBoolToBooleanObject(leftVal > rightVal);
    }
    if (op == "==") return nativeBoolToBooleanObject(left == right);
    if (op == "!=") return nativeBoolToBooleanObject(left != right);
    if (left->type() != right->type()) return std::make_shared<Error>("type mismatch: " + object::objectTypeToString(left->type()) + " " + op + " " + object::objectTypeToString(left->type()));
    return std::make_shared<Error>("unknown operator: " + object::objectTypeToString(left->type()) + op + object::objectTypeToString(right->type()));
}

std::shared_ptr<Object> evalIfExpression(std::shared_ptr<ast::IfExpression> ie, std::shared_ptr<Environment> env) {
    auto condition = eval(ie->condition, env);
    if (isError(condition)) return condition;
    if (isTruthy(condition)) return eval(ie->consequence, env);
    else if (ie->alternative != nullptr) return eval(ie->alternative, env);
    else return NULL_OBJ;
}

bool isTruthy(std::shared_ptr<Object> obj) {
    if (obj == NULL_OBJ || obj == FALSE) return false;
    if (obj == TRUE) return true;
    return true;
}

std::shared_ptr<Object> evalBlockStatement(std::shared_ptr<ast::BlockStatement> block, std::shared_ptr<Environment> env) {
    std::shared_ptr<Object> result;

    for (const auto& stmt : block->statements) {
        result = eval(stmt, env);

        if (result != nullptr) {
            auto rt = result->type();
            if (rt == object::ObjectType::RETURN_VALUE_OBJ || rt == object::ObjectType::ERROR_OBJ) {
                return result;
            }
        }
    }
    return result;
}

std::shared_ptr<Object> evalIdentifier(std::shared_ptr<ast::Identifier> node, std::shared_ptr<Environment> env) {
    auto val = env->get(node->value);
    if (val) {
        return val;
    }
    return std::make_shared<Error>("identifier not found: " + node->value);
}

std::vector<std::shared_ptr<Object>> evalExpressions(std::vector<std::shared_ptr<ast::Expression>> exps, std::shared_ptr<Environment> env) {
    std::vector<std::shared_ptr<Object>> result;

    for (auto e : exps) {
        auto evaluated = eval(e, env);
        if (isError(evaluated)) return {evaluated};
        result.push_back(evaluated);
    }
    return result;
}

std::shared_ptr<Object> applyFunction(std::shared_ptr<Object> fn, std::vector<std::shared_ptr<Object>> args) {
    auto function = std::dynamic_pointer_cast<Function>(fn);
    if (!function) {
        return std::make_shared<Error>("not a function: " + object::objectTypeToString(fn->type()));
    }
    auto extendedEnv = extendFunctionEnv(function, args);
    auto evaluated = eval(function->body, extendedEnv);
    return unwrapReturnValue(evaluated);
}

std::shared_ptr<Environment> extendFunctionEnv(std::shared_ptr<Function> fn, std::vector<std::shared_ptr<Object>> args) {
    auto env = object::newEnclosedEnvironment(fn->env);

    for (int i=0;i<fn->parameters.size();i++) {
        env->set(fn->parameters[i]->value, args[i]);
    }
    return env;
}

std::shared_ptr<Object> unwrapReturnValue(std::shared_ptr<Object> obj) {
    if (obj->type() == object::ObjectType::RETURN_VALUE_OBJ) {
        return std::dynamic_pointer_cast<ReturnValue>(obj)->value;
    }
    return obj;
}

}