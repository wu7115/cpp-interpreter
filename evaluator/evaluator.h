#pragma once

#include <memory>
#include <vector>
#include "../ast/ast.h"
#include "../environment/environment.h"
#include "../object/object.h"

namespace evaluator {

std::shared_ptr<object::Object> eval(std::shared_ptr<ast::Node> node, std::shared_ptr<object::Environment> env);
std::shared_ptr<object::Object> evalProgram(std::vector<std::shared_ptr<ast::Statement>> stmts, std::shared_ptr<object::Environment> env);
std::shared_ptr<object::Boolean> nativeBoolToBooleanObject(bool input);
bool isError(std::shared_ptr<object::Object> obj);
std::shared_ptr<object::Object> evalPrefixExpression(std::string op, std::shared_ptr<object::Object> right);
std::shared_ptr<object::Object> evalInfixExpression(std::string op, std::shared_ptr<object::Object> left, std::shared_ptr<object::Object> right);
std::shared_ptr<object::Object> evalIfExpression(std::shared_ptr<ast::IfExpression> ie, std::shared_ptr<object::Environment> env);
bool isTruthy(std::shared_ptr<object::Object> obj);
std::shared_ptr<object::Object> evalBlockStatement(std::shared_ptr<ast::BlockStatement> block, std::shared_ptr<object::Environment> env);
std::shared_ptr<object::Object> evalIdentifier(std::shared_ptr<ast::Identifier> node, std::shared_ptr<object::Environment> env);
std::vector<std::shared_ptr<object::Object>> evalExpressions(std::vector<std::shared_ptr<ast::Expression>> exps, std::shared_ptr<object::Environment> env);
std::shared_ptr<object::Object> applyFunction(std::shared_ptr<object::Object> fn, std::vector<std::shared_ptr<object::Object>> args);
std::shared_ptr<object::Environment> extendFunctionEnv(std::shared_ptr<object::Function> fn, std::vector<std::shared_ptr<object::Object>> args);
std::shared_ptr<object::Object> unwrapReturnValue(std::shared_ptr<object::Object> obj);

}