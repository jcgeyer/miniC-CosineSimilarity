#ifndef SEMANTIC_ANALYSIS_H
#define SEMANTIC_ANALYSIS_H

#include "ast.h"
#include <string>
#include <unordered_set>
#include <unordered_map>
#include <vector>

struct SymbolTable; 

// Traverse the AST for semantic analysis
void traverseAST(astNode* node, SymbolTable& symbols);

// Perform semantic analysis on the AST
std::unordered_set<std::string>  performSemanticAnalysis(astNode* root);

#endif
