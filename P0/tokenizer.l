/* Lex program to count total number of each type of tokens */

%{
    #include <stdio.h>
    int operator_count = 0; 
    int keyword_count = 0;
    int identifier_count = 0;
    int separator_count = 0;
    int constant_count = 0;
    int comment_count = 0;
%}
%option yylineno

/* Regex definitions */

double_line_comment "/*"(.*[\n]*.*)*"*/"
single_line_comment ("//"(.*[ \t]*.*)*[\n])
constants (0|[1-9][0-9]*)|((0|[1-9][0-9]*)"."[0-9]+)|('[^\\]')|(\"([^\\\"\n]|\\\\|\\\"|\\n)*\")
keywords "if"|"else"|"void"|"while"|"char"|"int"|"return"
operators "++"|"--"|"+="|"-="|"=="|"<="|">="|"*="|"/="|"!="|"+"|"-"|"*"|"/"|"="|"<"|"!"|">"|"|"
identifiers [a-zA-Z_][a-zA-Z0-9_]*
separators [(){}\[\],;:]

/* Rules for counting the tokens*/
%%
{double_line_comment} { comment_count++;
                        fprintf(yyout, "\tcomment: %s\n", yytext); }

{single_line_comment} { comment_count++;
                        fprintf(yyout, "\tcomment: %sLine %d\n", yytext, yylineno); }

{constants} { constant_count++;
            fprintf(yyout, "\tconstant(literal): %s\n", yytext); }

{keywords} { keyword_count++;
            fprintf(yyout, "\tkeyword: %s\n", yytext); }

{operators} { operator_count++;
            fprintf(yyout, "\toperator: %s\n", yytext); }

{identifiers} { identifier_count++;
            fprintf(yyout, "\tidentifier: %s\n", yytext); }

\n { fprintf(yyout, "Line %d\n", yylineno); }

{separators} { separator_count++;
                fprintf(yyout, "\tseparator: %s\n", yytext); }

[\t ]   ;

. { fprintf(yyout, "\tUnexpected token: %s\n", yytext); }

%%

int yywrap(){
    return 1;
}

int main(int argc, char *argv[]){

    extern FILE *yyin, *yyout;
    if (argc == 1) {
        printf("Input file path missing.\nUsage: ./a.out <path-to-input-file> <output-file-path>\nNote: Output path can be ommited.\n");
        return 0;
    } else if (argc == 2) {
        yyin = fopen(argv[1], "r");
        yyout = stdout;
    }
    else if (argc == 3) {
        yyin = fopen(argv[1], "r"); 
        yyout = fopen(argv[2], "w"); 
    }
    if (yyin == NULL) {
        printf("Input file path wrong.\nUsage: ./a.out <path-to-input-file> <output-file-path>\nNote: Output path can be ommited.\n");
        return 0;
    }
    
    fprintf(yyout, "Line 1\n");

    yylex();

    fprintf(yyout, "Global stats:\n");
    fprintf(yyout, "\tCount of constants(literals): %d\n", constant_count);
    fprintf(yyout, "\tCount of keywords: %d\n", keyword_count);
    fprintf(yyout, "\tCount of identifiers: %d\n", identifier_count);
    fprintf(yyout, "\tCount of separators: %d\n", separator_count);
    fprintf(yyout, "\tCount of operators: %d\n", operator_count);
    fprintf(yyout, "\tCount of comments: %d\n", comment_count);
    fprintf(yyout, "\tTotal count of recognized tokens: %d\n", constant_count+keyword_count+identifier_count+separator_count+operator_count+comment_count);
    
    return 0;
}
