#ifndef COSINESIMILARITY_H
#define COSINESIMILARITY_H

#include <stdlib.h>
#include "ast.h"

int getDepth(astNode* node);
double* getEmbeddingForNode(astNode* node);
void printEmbedding(double* embedding, int size);
std::vector<double> getEmbeddingsForAST(astNode* node);
void printEmbeddings(const std::vector<double>& embeddings);
double cosineSimilarity(std::vector<double>& embeddings1, std::vector<double>& embeddings2);

#endif