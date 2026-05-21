#ifndef SEMANTIC_ANALYZER_HPP
#define SEMANTIC_ANALYZER_HPP

#include "tree/node.hpp"
#include "tree/ast_node.hpp"
#include "semantic/symbol_table.hpp"
#include "semantic/type_checker.hpp"
#include "semantic/semantic_error.hpp"
#include <string>
#include <memory>
#include <vector>

struct SemanticContext {
    SymbolTable           st;
    SemanticErrorCollector errors;
    SemanticContext() {}
};

void analyze(Node* root, SemanticContext& ctx);

std::string nodeTokenType(const Node* node);

std::string nodeTokenValue(const Node* node);

bool isNodeType(const Node* node, const std::string& label);

Node* findChild(Node* node, const std::string& label);

Node* findChildByTokenType(Node* node, const std::string& tokenType);

void visit_program(Node* node, SemanticContext& ctx);
void visit_program_header(Node* node, SemanticContext& ctx);
void visit_declaration_part(Node* node, SemanticContext& ctx);

void visit_block(Node* node, SemanticContext& ctx);

void visit_const_declaration(Node* node, SemanticContext& ctx);
void visit_type_declaration(Node* node, SemanticContext& ctx);
void visit_var_declaration(Node* node, SemanticContext& ctx);
void visit_subprogram_declaration(Node* node, SemanticContext& ctx);
void visit_procedure_declaration(Node* node, SemanticContext& ctx);
void visit_function_declaration(Node* node, SemanticContext& ctx);

void visit_formal_parameter_list(Node* node, SemanticContext& ctx, int btabIdx);

TypeInfo visit_type(Node* node, SemanticContext& ctx);
TypeInfo visit_array_type(Node* node, SemanticContext& ctx);

TypeInfo visit_record_type(Node* node, SemanticContext& ctx,
                            const std::string& namedId,
                            int* btabIdxOut = nullptr);

TypeInfo visit_range(Node* node, SemanticContext& ctx);
TypeInfo visit_enumerated(Node* node, SemanticContext& ctx);
TypeInfo visit_constant_value(Node* node, SemanticContext& ctx);

TypeInfo visit_expression(Node* node, SemanticContext& ctx);
TypeInfo visit_simple_expression(Node* node, SemanticContext& ctx);
TypeInfo visit_term(Node* node, SemanticContext& ctx);
TypeInfo visit_factor(Node* node, SemanticContext& ctx);
TypeInfo visit_variable(Node* node, SemanticContext& ctx);

TypeInfo visit_component_variable(Node* node, SemanticContext& ctx,
                                   TypeInfo baseType);

void visit_procedure_function_call(Node* node, SemanticContext& ctx);

void visit_compound_statement(Node* node, SemanticContext& ctx);
void visit_statement_list(Node* node, SemanticContext& ctx);
void visit_statement(Node* node, SemanticContext& ctx);
void visit_assignment_statement(Node* node, SemanticContext& ctx);
void visit_if_statement(Node* node, SemanticContext& ctx);
void visit_while_statement(Node* node, SemanticContext& ctx);
void visit_repeat_statement(Node* node, SemanticContext& ctx);
void visit_for_statement(Node* node, SemanticContext& ctx);
void visit_case_statement(Node* node, SemanticContext& ctx);
void visit_case_list(Node* node, SemanticContext& ctx,
                     const TypeInfo& exprType);
void visit_case_branch(Node* node, SemanticContext& ctx,
                       const TypeInfo& exprType);


#endif 