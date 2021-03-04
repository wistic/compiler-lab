/* Lex program to count total number of each type of tokens */

%{
    #include <stdio.h>
    int operator_count = 0; 
    int keyword_count = 0;
    int identifier_count = 0;
    int separator_count = 0;
    int constant_count = 0;
%}
%option yylineno

/* Regex definitions */

keywords "if"|"else"|"for"|"void"|"while"|"char"|"int"|"do"|"continue"|"default"|"const"|"float"
operators "++"|"--"|"+="|"-="|"=="|"<="|">="|"*="|"/="|"%="|"!="|"+"|"-"|"*"|"/"|"="|"<"|"!"|">"|"%"|"||"|"&&"|"|"|"&"
separators [(){}\[\],;:]
identifiers [a-zA-Z_][a-zA-Z0-9_]*
constants (0|[1-9][0-9]*)|((0|[1-9][0-9]*)"."[0-9]+)|('[^\\]')|(\"([^\\\"\n]|\\\\|\\\"|\\n)*\")

/* Rules for counting the tokens*/
%%
{constants} { constant_count++;
            printf("\tconstant(literal): %s\n", yytext); }

{keywords} { keyword_count++;
            printf("\tkeyword: %s\n", yytext); }

{operators} { operator_count++;
            printf("\toperator: %s\n", yytext); }

{identifiers} { identifier_count++;
            printf("\tidentifier: %s\n", yytext); }

\n { printf("Line %d\n", yylineno); }

{separators} { separator_count++;
                printf("\tseparator: %s\n", yytext); }

[\t ]   ;

. { printf("\tUnexpected token: %s\n", yytext); }

%%

int yywrap(){
    return 1;
}

int main(){
    printf("Line 1\n");

    yylex();

    printf("Global stats:\n");
    printf("\tCount of constants(literals): %d\n", constant_count);
    printf("\tCount of keywords: %d\n", keyword_count);
    printf("\tCount of identifiers: %d\n", identifier_count);
    printf("\tCount of separators: %d\n", separator_count);
    printf("\tCount of operators: %d\n", operator_count);
    printf("\tTotal count of recognized tokens: %d\n", constant_count+keyword_count+identifier_count+separator_count+operator_count);
    
    return 0;
}
