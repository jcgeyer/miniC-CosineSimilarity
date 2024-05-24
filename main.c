#include "ast.h"
#include "semantic_analysis.h"
#include "cosineSimilarity.h"
#include <stdio.h>
#include <stdlib.h>
#include <iostream>


#define prt(x) if(x) { printf("%s\n", x); }


extern int yyparse();
extern FILE *yyin;
extern FILE *yyin1;
extern FILE *yyin2;
extern void yylex_destroy();
extern int parse_errors;
extern int semantic_errors;
extern astNode* root;

int main(int argc, char** argv) {

    astNode* root1;
    astNode* root2;

    prt("Args");
    prt(argv[1]);
    prt(argv[2]);

    // PASS 1 (FILE 1) 

    // take 3 args 
    if (argc == 3) {
        yyin = fopen(argv[1], "r");
        // yyin1 = fopen(argv[1], "r");
        // yyin2 = fopen(argv[2], "r");
        if (!yyin) {
            fprintf(stderr, "Error opening file: %s\n", argv[1]);
            return 1;
        }
    }

    // Parsing file 1
    int result = yyparse();
    fclose(yyin);
    printf("Number of parse errors: %i\n", parse_errors);

    std::unordered_set<std::string> varNames;
    if (result == 0 && parse_errors == 0 && root != NULL) {
        printf("Parsing completed successfully.\n");
        printNode(root, 0);
        printf("Performing semantic analysis (and renaming vars)...\n");

        // Semantic analysis
        std::unordered_set<std::string> varNames = performSemanticAnalysis(root);
        std::cout << "Unique Variable Names:" << std::endl;
        for (const std::string& varName : varNames) {
            std::cout << varName << std::endl;
        }

        // save root to root1
        root1 = root;
        prt("SUCCESS");


        // if (root != NULL) {
        //     freeNode(root);
        // }
    } else {
        fprintf(stderr, "Parsing File 1 failed.\n");
    }


    // PASS 2 (FILE 2) 

    // take 3 args 
    yyin = fopen(argv[2], "r");
    if (!yyin) {
        fprintf(stderr, "Error opening file: %s\n", argv[2]);
        return 1;
    }

    // Parsing file 2
    result = yyparse();
    fclose(yyin);
    printf("Number of parse errors: %i\n", parse_errors);

    if (result == 0 && parse_errors == 0 && root != NULL) {
        printf("Parsing completed successfully.\n");
        printNode(root, 0);
        printf("Performing semantic analysis (and renaming vars)...\n");

        // Semantic analysis
        std::unordered_set<std::string> varNames = performSemanticAnalysis(root);
        std::cout << "Unique Variable Names:" << std::endl;
        for (const std::string& varName : varNames) {
            std::cout << varName << std::endl;
        }

        // save root to root2
        root2 = root;
        prt("Success (again)");

        // if (root != NULL) {
        //     freeNode(root);
        // }
    } else {
        fprintf(stderr, "Parsing File 1 failed.\n");
    }

    printNode(root1, 0);
    printNode(root2, 0);



    // COMPARE
    prt("embedding 1");
    std::vector<double> embeddings = getEmbeddingsForAST(root1);
    printEmbeddings(embeddings);
    prt("embedding 2");
    std::vector<double> embeddings2 = getEmbeddingsForAST(root2);
    printEmbeddings(embeddings2);

    double c_sim = cosineSimilarity(embeddings, embeddings2);
    printf("\nCosine Similarity: %f\n", c_sim);

    // int c_sim = cosineSimilarity(root1, root2);
    // prt(c_sim);




    yylex_destroy();
    return parse_errors ? 1 : 0;
}





// int main(int argc, char** argv) {
//     if (argc == 2) {
//         yyin = fopen(argv[1], "r");
//         if (!yyin) {
//             fprintf(stderr, "Error opening file: %s\n", argv[1]);
//             return 1;
//         }
//     }

//     // Parsing
//     int result = yyparse();
//     fclose(yyin);
//     printf("Number of parse errors: %i\n", parse_errors);

//     std::unordered_set<std::string> varNames;
//     if (result == 0 && parse_errors == 0 && root != NULL) {
//         printf("Parsing completed successfully.\n");
//         printNode(root, 0);
//         printf("Performing semantic analysis (and renaming vars)...\n");

//         // Semantic analysis
//         std::unordered_set<std::string> varNames = performSemanticAnalysis(root);
//         std::cout << "Unique Variable Names:" << std::endl;
//         for (const std::string& varName : varNames) {
//             std::cout << varName << std::endl;
//         }

//         // IR builder
//         if (root != NULL) {
//             std::string filename;
//             if (argc == 2) {
//                 filename = argv[1];
//                 size_t dotPos = filename.find_last_of('.');
//                 if (dotPos != std::string::npos) {
//                     filename = filename.substr(0, dotPos);
//                 }
//             } else {
//                 filename = "output";
//             }
//             generateIR(root, varNames, filename);
//             freeNode(root);
//         }
//     } else {
//         fprintf(stderr, "Parsing failed.\n");
//     }

//     yylex_destroy();
//     return parse_errors ? 1 : 0;
// }