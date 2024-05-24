%{
#include <vector>
#include <set>
#include <string>
#include "ast.h"
#include "semantic_analysis.h"
#include <stdio.h>
#include <stdlib.h>
extern int yylex();
extern int yywrap(void);
extern int yylex_destroy();
void yyerror(const char *s);
extern FILE *yyin;

extern int parse_errors; 
extern int semantic_errors; 

astNode* root = NULL;

%}

%union {
    int ival;
    char *sval;
    astNode *ast;
    vector<astNode*> *ast_list;
}

%token <sval> ID
%token <ival> NUM
%token INT VOID EXTERN RETURN IF ELSE WHILE PRINT READ

%nonassoc LOWER_THAN_ELSE
%nonassoc ELSE

%token EQ GT LT GE LE NEQ

%type <ast> program function external_decl function_body block_stmt maybe_block_stmt opt_param factor stmt decl expr term cond extern_call
%type <ast_list> var_decls stmts

%destructor {
    if ($$) {
        for (auto node : *$$)
            freeNode(node);
        delete $$;
    }
} <ast_list>

/* commented this out, was causing lots of errors for bad semantic test cases */
/* %destructor {
    freeNode($$);
} <ast> */

%destructor {
    free($$);
} <sval>

%start program

%%

program
    : external_decl external_decl function
    { root = createProg($1, $2, $3); $$ = root; }
    ;

external_decl
    : EXTERN VOID PRINT '(' INT ')' ';'
    { $$ = createExtern("print"); }
    | EXTERN INT READ '(' ')' ';'
    { $$ = createExtern("read"); }
    ;

function
    : INT ID '(' opt_param ')' function_body
    { $$ = createFunc($2, $4, $6); free($2);}
    ;

opt_param
    :
    { $$ = NULL; }
    | INT ID
    { $$ = createVar($2); free($2); }
    ;

function_body
    : block_stmt
    { $$ = $1; }
    ;

var_decls
    :
    { $$ = new vector<astNode*>(); }
    | var_decls decl
    { $1->push_back($2); $$ = $1; }
    ;

decl
    : INT ID ';'
    {$$ = createDecl($2); free($2);}
    ;

block_stmt
    : '{' var_decls stmts '}'
    { vector<astNode*>* vec = new vector<astNode*>();
      if ($2) vec->insert(vec->end(), $2->begin(), $2->end());
      if ($3) vec->insert(vec->end(), $3->begin(), $3->end());
      $$ = createBlock(vec); 
      delete $3; delete $2; }
    ;

maybe_block_stmt
    : block_stmt
    { $$ = $1; }
    | stmt
    { $$ = createBlock(new vector<astNode*>{$1}); }

stmts
    :
    { $$ = new vector<astNode*>(); }
    | stmts stmt
    { $1->push_back($2); $$ = $1; }
    ;

stmt
    : ID '=' expr ';'
    { $$ = createAsgn(createVar($1), $3); free($1);}
    | PRINT '(' expr ')' ';'
    { $$ = createCall("print", $3); }
    | IF '(' cond ')' maybe_block_stmt %prec LOWER_THAN_ELSE
    { $$ = createIf($3, $5, NULL); }
    | IF '(' cond ')' maybe_block_stmt ELSE maybe_block_stmt
    { $$ = createIf($3, $5, $7); }
    | WHILE '(' cond ')' maybe_block_stmt
    { $$ = createWhile($3, $5); }
    | RETURN expr ';'
    { $$ = createRet($2); }
    ;

expr
    : expr '+' term
    { $$ = createBExpr($1, $3, add); }
    | expr '-' term
    { $$ = createBExpr($1, $3, sub); }
    | term
    { $$ = $1; }
    ;

term
    : term '*' factor
    { $$ = createBExpr($1, $3, mul); }
    | term '/' factor
    { $$ = createBExpr($1, $3, divide); }
    | factor
    { $$ = $1; }
    ;

factor
    : NUM                { $$ = createCnst($1); }
    | ID                 { $$ = createVar($1); free($1);}
    | extern_call
    | '(' expr ')'       { $$ = $2; }
    ;

extern_call
    : READ '(' ')'       { $$ = createCall("read", NULL); }
    | PRINT '(' expr ')' { $$ = createCall("print", $3); }
    ;

cond
    : expr EQ expr
    { $$ = createRExpr($1, $3, eq); }
    | expr GT expr
    { $$ = createRExpr($1, $3, gt); }
    | expr LT expr
    { $$ = createRExpr($1, $3, lt); }
    | expr GE expr
    { $$ = createRExpr($1, $3, ge); }
    | expr LE expr
    { $$ = createRExpr($1, $3, le); }
    | expr NEQ expr
    { $$ = createRExpr($1, $3, neq); }
    ;

%%
int parse_errors = 0; 
int semantic_errors = 0;

void yyerror(const char *s){
    fprintf(stderr, "Syntax error: %s\n", s);
    parse_errors++; 
}

