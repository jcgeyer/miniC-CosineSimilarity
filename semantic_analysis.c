#include "ast.h"
#include "semantic_analysis.h"
#include <vector>
#include <string>
#include <iostream>
#include <cstring>
#include <unordered_set>
#include <unordered_map>

struct SymbolTable {
    std::vector<std::unordered_map<std::string, std::string>> stack;
    std::unordered_set<std::string> varNames; 

    int error_count = 0;

    void enterScope() {
        stack.emplace_back();
    }

    void leaveScope() {
        if (!stack.empty()) {
            stack.pop_back();
        }
    }

    bool declareVariable(const std::string& name) {
        if (!stack.empty() && stack.back().find(name) != stack.back().end()) {
            std::cout << "Semantic Error: Variable '" << name << "' redeclared in the same scope." << std::endl;
            error_count++;
            return false;
        }
        std::string newName = generateUniqueName(name);
        stack.back()[name] = newName;
        varNames.insert(newName);
        return true;
    }

    std::string useVariable(const std::string& name) {
        for (auto it = stack.rbegin(); it != stack.rend(); ++it) {
            auto varIt = it->find(name);
            if (varIt != it->end()) {
                return varIt->second;
            }
        }
        std::cout << "Semantic Error: Variable '" << name << "' used but not declared in accessible scopes." << std::endl;
        error_count++;
        return "";
    }

    std::string generateUniqueName(const std::string& name) {
        static int uniqueId = 0;
        return name + "_" + std::to_string(uniqueId++);
    }
};

void renameVariables(astNode* node, SymbolTable& symbols) {
    if (!node) return;

    switch (node->type) {
        case ast_prog:
            renameVariables(node->prog.ext1, symbols);
            renameVariables(node->prog.ext2, symbols);
            renameVariables(node->prog.func, symbols);
            break;
        case ast_func:
            symbols.enterScope();
            if (node->func.param) {
                if (symbols.declareVariable(node->func.param->var.name)) {
                    std::string newName = symbols.useVariable(node->func.param->var.name);
                    free(node->func.param->var.name);
                    node->func.param->var.name = strdup(newName.c_str());
                }
            }
            for (auto& stmt : *node->func.body->stmt.block.stmt_list) {
                renameVariables(stmt, symbols);
            }
            symbols.leaveScope();
            break;
        case ast_bexpr:
            renameVariables(node->bexpr.lhs, symbols);
            renameVariables(node->bexpr.rhs, symbols);
            break;
        case ast_rexpr:
            renameVariables(node->rexpr.lhs, symbols);
            renameVariables(node->rexpr.rhs, symbols);
            break;
        case ast_stmt: {
            astStmt* stmt = &node->stmt;
            switch(stmt->type) {
                case ast_block:
                    symbols.enterScope();
                    for (auto child_stmt : *stmt->block.stmt_list) {
                        renameVariables(child_stmt, symbols);
                    }
                    symbols.leaveScope();
                    break;
                case ast_decl:
                    if (symbols.declareVariable(stmt->decl.name)) {
                        std::string newName = symbols.useVariable(stmt->decl.name);
                        free(stmt->decl.name);
                        stmt->decl.name = strdup(newName.c_str());
                    }
                    break;
                case ast_call:
                    if (stmt->call.param) {
                        renameVariables(stmt->call.param, symbols);
                    }
                    break;
                case ast_asgn:
                    renameVariables(stmt->asgn.lhs, symbols);
                    renameVariables(stmt->asgn.rhs, symbols);
                    break;
                case ast_if:
                    symbols.enterScope();
                    renameVariables(stmt->ifn.cond, symbols);
                    renameVariables(stmt->ifn.if_body, symbols);
                    if (stmt->ifn.else_body) renameVariables(stmt->ifn.else_body, symbols);
                    symbols.leaveScope();
                    break;
                case ast_while:
                    symbols.enterScope();
                    renameVariables(stmt->whilen.cond, symbols);
                    renameVariables(stmt->whilen.body, symbols);
                    symbols.leaveScope();
                    break;
                case ast_ret:
                    renameVariables(stmt->ret.expr, symbols);
                    break;
                default:
                    break;
            }
            break;
        }
        case ast_var:
            {
                std::string newName = symbols.useVariable(node->var.name);
                if (!newName.empty()) {
                    free(node->var.name);
                    node->var.name = strdup(newName.c_str());
                } else {
                    newName = symbols.generateUniqueName(node->var.name);
                    free(node->var.name);
                    node->var.name = strdup(newName.c_str());
                }
            }
            break;
        default:
            break;
    }
}

std::unordered_set<std::string> performSemanticAnalysis(astNode* root) {
    SymbolTable symbols;
    std::cout << "Starting semantic analysis..." << std::endl;
    renameVariables(root, symbols);
    if (symbols.error_count > 0) {
        std::cout << "Semantic analysis completed with " << symbols.error_count << " errors." << std::endl;
    } else {
        std::cout << "Semantic analysis completed without errors." << std::endl;
    }

    return symbols.varNames;
}

