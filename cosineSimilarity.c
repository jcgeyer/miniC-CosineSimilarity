#include <stdlib.h>
#include "ast.h"
#include "cosineSimilarity.h"
#include <cstdio>
#include <cstring>
#include <cmath>
#include <vector>
#include <algorithm>

#define prt(x) if(x) { printf("%s\n", x); }


int getDepth(astNode* node) {
    if (node == NULL) {
        return 0;
    }
    
    int maxChildDepth = 0;

    switch (node->type) {
        case ast_prog:
            maxChildDepth = getDepth(node->prog.ext1);
            maxChildDepth = (getDepth(node->prog.ext2) > maxChildDepth) ? getDepth(node->prog.ext2) : maxChildDepth;
            maxChildDepth = (getDepth(node->prog.func) > maxChildDepth) ? getDepth(node->prog.func) : maxChildDepth;
            break;
        case ast_func:
            maxChildDepth = getDepth(node->func.param);
            maxChildDepth = (getDepth(node->func.body) > maxChildDepth) ? getDepth(node->func.body) : maxChildDepth;
            break;
        case ast_stmt:
            switch (node->stmt.type) {
                case ast_call:
                    maxChildDepth = getDepth(node->stmt.call.param);
                    break;
                case ast_ret:
                    maxChildDepth = getDepth(node->stmt.ret.expr);
                    break;
                case ast_block:
                    for (std::vector<astNode*>::size_type i = 0; i < node->stmt.block.stmt_list->size(); i++) {
                        int childDepth = getDepth((*node->stmt.block.stmt_list)[i]);
                        maxChildDepth = (childDepth > maxChildDepth) ? childDepth : maxChildDepth;
                    }
                    break;
                case ast_while:
                    maxChildDepth = getDepth(node->stmt.whilen.cond);
                    maxChildDepth = (getDepth(node->stmt.whilen.body) > maxChildDepth) ? getDepth(node->stmt.whilen.body) : maxChildDepth;
                    break;
                case ast_if:
                    maxChildDepth = getDepth(node->stmt.ifn.cond);
                    maxChildDepth = (getDepth(node->stmt.ifn.if_body) > maxChildDepth) ? getDepth(node->stmt.ifn.if_body) : maxChildDepth;
                    if (node->stmt.ifn.else_body != NULL) {
                        maxChildDepth = (getDepth(node->stmt.ifn.else_body) > maxChildDepth) ? getDepth(node->stmt.ifn.else_body) : maxChildDepth;
                    }
                    break;
                case ast_asgn:
                    maxChildDepth = getDepth(node->stmt.asgn.lhs);
                    maxChildDepth = (getDepth(node->stmt.asgn.rhs) > maxChildDepth) ? getDepth(node->stmt.asgn.rhs) : maxChildDepth;
                    break;
                default:
                    break;
            }
            break;
        case ast_rexpr:
            maxChildDepth = getDepth(node->rexpr.lhs);
            maxChildDepth = (getDepth(node->rexpr.rhs) > maxChildDepth) ? getDepth(node->rexpr.rhs) : maxChildDepth;
            break;
        case ast_bexpr:
            maxChildDepth = getDepth(node->bexpr.lhs);
            maxChildDepth = (getDepth(node->bexpr.rhs) > maxChildDepth) ? getDepth(node->bexpr.rhs) : maxChildDepth;
            break;
        case ast_uexpr:
            maxChildDepth = getDepth(node->uexpr.expr);
            break;
        default:
            break;
    }
    
    return maxChildDepth + 1;
}

double* getEmbeddingForNode(astNode* node) {
    int embeddingSize = 10;
    double* embedding = (double*)malloc(embeddingSize * sizeof(double));
    memset(embedding, 0, embeddingSize * sizeof(double));
    int i = 0;
    
    switch (node->type) {
        case ast_prog:
            embedding[i++] = 1.0;
            break;
        case ast_func:
            embedding[i++] = 0.9;
            if (node->func.name != NULL) {
                embedding[i++] = 0.8;
            }
            break;
        case ast_extern:
            embedding[i++] = 0.7;
            if (node->ext.name != NULL) {
                embedding[i++] = 0.6;
            }
            break;
        case ast_stmt:
            switch (node->stmt.type) {
                case ast_call:
                    embedding[i++] = 0.8;
                    if (node->stmt.call.name != NULL) {
                        embedding[i++] = 0.7;
                    }
                    break;
                case ast_asgn:
                    embedding[i++] = 0.6;
                    break;
                case ast_if:
                case ast_while:
                    embedding[i++] = 0.8;
                    break;
                case ast_ret:
                    embedding[i++] = 0.9;
                    break;
                case ast_block:
                    embedding[i++] = 0.5;
                    break;
                default:
                    break;
            }
            break;
        case ast_var:
            embedding[i++] = 0.6;
            if (node->var.name != NULL) {
                embedding[i++] = 0.5;
            }
            break;
        case ast_cnst:
            embedding[i++] = 0.4;
            embedding[i++] = node->cnst.value * 0.1;
            break;
        case ast_rexpr:
            embedding[i++] = 0.6;
            embedding[i++] = node->rexpr.op * 0.1;
            break;
        case ast_bexpr:
            embedding[i++] = 0.6;
            embedding[i++] = node->bexpr.op * 0.1;
            break;
        case ast_uexpr:
            embedding[i++] = 0.4;
            embedding[i++] = node->uexpr.op * 0.1;
            break;
        default:
            break;
    }
    
    int depth = getDepth(node);
    embedding[i++] = 1.0 - depth * 0.1;
    
    return embedding;
}

std::vector<double> getEmbeddingsForAST(astNode* node) {
    std::vector<double> embeddings;

    if (node == NULL) {
        return embeddings;
    }

    // Get the embedding for the current node
    double* nodeEmbedding = getEmbeddingForNode(node);
    int embeddingSize = 4; // Adjust the size based on the number of embedding features

    // Append the current node's embedding to the embeddings vector
    embeddings.insert(embeddings.end(), nodeEmbedding, nodeEmbedding + embeddingSize);

    // Free the dynamically allocated memory for the node embedding
    free(nodeEmbedding);

    // Recursively get the embeddings for the child nodes based on the node type
    switch (node->type) {
        case ast_prog:
            {
                std::vector<double> ext1Embeddings = getEmbeddingsForAST(node->prog.ext1);
                std::vector<double> ext2Embeddings = getEmbeddingsForAST(node->prog.ext2);
                std::vector<double> funcEmbeddings = getEmbeddingsForAST(node->prog.func);
                embeddings.insert(embeddings.end(), ext1Embeddings.begin(), ext1Embeddings.end());
                embeddings.insert(embeddings.end(), ext2Embeddings.begin(), ext2Embeddings.end());
                embeddings.insert(embeddings.end(), funcEmbeddings.begin(), funcEmbeddings.end());
            }
            break;
        case ast_func:
            {
                std::vector<double> paramEmbeddings = getEmbeddingsForAST(node->func.param);
                std::vector<double> bodyEmbeddings = getEmbeddingsForAST(node->func.body);
                embeddings.insert(embeddings.end(), paramEmbeddings.begin(), paramEmbeddings.end());
                embeddings.insert(embeddings.end(), bodyEmbeddings.begin(), bodyEmbeddings.end());
            }
            break;
        case ast_stmt:
            switch (node->stmt.type) {
                case ast_call:
                    {
                        std::vector<double> paramEmbeddings = getEmbeddingsForAST(node->stmt.call.param);
                        embeddings.insert(embeddings.end(), paramEmbeddings.begin(), paramEmbeddings.end());
                    }
                    break;
                case ast_ret:
                    {
                        std::vector<double> exprEmbeddings = getEmbeddingsForAST(node->stmt.ret.expr);
                        embeddings.insert(embeddings.end(), exprEmbeddings.begin(), exprEmbeddings.end());
                    }
                    break;
                case ast_block:
                    {
                        for (std::vector<astNode*>::size_type i = 0; i < node->stmt.block.stmt_list->size(); i++) {
                            std::vector<double> stmtEmbeddings = getEmbeddingsForAST((*node->stmt.block.stmt_list)[i]);
                            embeddings.insert(embeddings.end(), stmtEmbeddings.begin(), stmtEmbeddings.end());
                        }
                    }
                    break;
                case ast_while:
                    {
                        std::vector<double> condEmbeddings = getEmbeddingsForAST(node->stmt.whilen.cond);
                        std::vector<double> bodyEmbeddings = getEmbeddingsForAST(node->stmt.whilen.body);
                        embeddings.insert(embeddings.end(), condEmbeddings.begin(), condEmbeddings.end());
                        embeddings.insert(embeddings.end(), bodyEmbeddings.begin(), bodyEmbeddings.end());
                    }
                    break;
                case ast_if:
                    {
                        std::vector<double> condEmbeddings = getEmbeddingsForAST(node->stmt.ifn.cond);
                        std::vector<double> ifBodyEmbeddings = getEmbeddingsForAST(node->stmt.ifn.if_body);
                        embeddings.insert(embeddings.end(), condEmbeddings.begin(), condEmbeddings.end());
                        embeddings.insert(embeddings.end(), ifBodyEmbeddings.begin(), ifBodyEmbeddings.end());
                        if (node->stmt.ifn.else_body != NULL) {
                            std::vector<double> elseBodyEmbeddings = getEmbeddingsForAST(node->stmt.ifn.else_body);
                            embeddings.insert(embeddings.end(), elseBodyEmbeddings.begin(), elseBodyEmbeddings.end());
                        }
                    }
                    break;
                case ast_asgn:
                    {
                        std::vector<double> lhsEmbeddings = getEmbeddingsForAST(node->stmt.asgn.lhs);
                        std::vector<double> rhsEmbeddings = getEmbeddingsForAST(node->stmt.asgn.rhs);
                        embeddings.insert(embeddings.end(), lhsEmbeddings.begin(), lhsEmbeddings.end());
                        embeddings.insert(embeddings.end(), rhsEmbeddings.begin(), rhsEmbeddings.end());
                    }
                    break;
                default:
                    break;
            }
            break;
        case ast_rexpr:
            {
                std::vector<double> lhsEmbeddings = getEmbeddingsForAST(node->rexpr.lhs);
                std::vector<double> rhsEmbeddings = getEmbeddingsForAST(node->rexpr.rhs);
                embeddings.insert(embeddings.end(), lhsEmbeddings.begin(), lhsEmbeddings.end());
                embeddings.insert(embeddings.end(), rhsEmbeddings.begin(), rhsEmbeddings.end());
            }
            break;
        case ast_bexpr:
            {
                std::vector<double> lhsEmbeddings = getEmbeddingsForAST(node->bexpr.lhs);
                std::vector<double> rhsEmbeddings = getEmbeddingsForAST(node->bexpr.rhs);
                embeddings.insert(embeddings.end(), lhsEmbeddings.begin(), lhsEmbeddings.end());
                embeddings.insert(embeddings.end(), rhsEmbeddings.begin(), rhsEmbeddings.end());
            }
            break;
        case ast_uexpr:
            {
                std::vector<double> exprEmbeddings = getEmbeddingsForAST(node->uexpr.expr);
                embeddings.insert(embeddings.end(), exprEmbeddings.begin(), exprEmbeddings.end());
            }
            break;
        default:
            break;
    }

    return embeddings;
}

void printEmbedding(double* embedding, int size) {
    printf("Embedding: [");
    for (int i = 0; i < size; i++) {
        printf("%.2f", embedding[i]);
        if (i < size - 1) {
            printf(", ");
        }
    }
    printf("]\n");
}

void printEmbeddings(const std::vector<double>& embeddings) {
    printf("Embeddings: [");
    for (size_t i = 0; i < embeddings.size(); i++) {
        printf("%.2f", embeddings[i]);
        if (i < embeddings.size() - 1) {
            printf(", ");
        }
    }
    printf("]\n");
}

double cosineSimilarity(std::vector<double>& embeddings1, std::vector<double>& embeddings2) {
    
    printf("\nEmbedding 1 Size: %ld", embeddings1.size());
    printf("\nEmbedding 2 Size: %ld", embeddings2.size());
    
    size_t maxSize = std::max(embeddings1.size(), embeddings2.size());

    // Pad the smaller embedding vector with zeros
    if (embeddings1.size() < maxSize) {
        embeddings1.resize(maxSize, 0.0);
    } else if (embeddings2.size() < maxSize) {
        embeddings2.resize(maxSize, 0.0);
    }

    double dotProduct = 0.0;
    double magnitude1 = 0.0;
    double magnitude2 = 0.0;

    for (size_t i = 0; i < embeddings1.size(); i++) {
        dotProduct += embeddings1[i] * embeddings2[i];
        magnitude1 += embeddings1[i] * embeddings1[i];
        magnitude2 += embeddings2[i] * embeddings2[i];
    }

    magnitude1 = sqrt(magnitude1);
    magnitude2 = sqrt(magnitude2);

    if (magnitude1 == 0.0 || magnitude2 == 0.0) {
        // Avoid division by zero
        return 0.0;
    }

    return dotProduct / (magnitude1 * magnitude2);
}
