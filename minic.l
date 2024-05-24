%{
#include "ast.h"
#ifndef AST_H
#error "AST header not included properly"
#endif
#include<stdio.h>
#include "y.tab.h"
%}

digit           [0-9]
letter          [a-zA-Z]
identifier      {letter}({letter}|{digit})*

%%

"int"           { return INT; }
"void"          { return VOID; }
"extern"        { return EXTERN; }
"return"        { return RETURN; }
"if"            { return IF; }
"else"          { return ELSE; }
"while"         { return WHILE; }
"print"         { return PRINT; }
"read"          { return READ; }

{identifier}    {   yylval.sval = strdup(yytext); 
                    // printf("Identifier: %s\n", yytext); 
                    return ID; }
-?{digit}+        {   yylval.ival = atoi(yytext); 
                    // printf("Number: %s\n", yytext); 
                    return NUM; }

"+"             { return '+'; }
"-"             { return '-'; }
"*"             { return '*'; }
"/"             { return '/'; }
"="             { return '='; }
"=="            { return EQ; }
">"             { return GT; }
"<"             { return LT; }
">="            { return GE; }
"<="            { return LE; }
"!="            { return NEQ; }

"("             { return '('; }
")"             { return ')'; }
"{"             { return '{'; }
"}"             { return '}'; }
","             { return ','; }
";"             { return ';'; }

[ \t\n]+ 
.                 { 
                    // printf("Unexpected character: %c\n", yytext[0]); 
                    return yytext[0]; }

%%

int yywrap() { return 1; }