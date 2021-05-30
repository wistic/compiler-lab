/* Lex program to count total number of each type of tokens */

%{
    #include <stdio.h>
%}
%option yylineno

/* Regex definitions */

constants (0|[1-9][0-9]*)|((0|[1-9][0-9]*)"."[0-9]+)|('[^\\]')
strings \"([^\\\"\n]|\\\\|\\\"|\\n)*\"
keywords "auto"|"double"|"int"|"struct"|"break"|"else"|"long"|"switch"|"case"|"enum"|"register"|"typedef"|"char"|"extern"|"return"|"union"|"const"|"float"|"short"|"unsigned"|"continue"|"for"|"signed"|"void"|"do"|"if"|"static"|"while"
operators "++"|"--"|"+="|"-="|"=="|"<="|">="|"*="|"/="|"!="|"+"|"-"|"*"|"/"|"<"|"!"|">"|"|"
identifiers [a-zA-Z_][a-zA-Z0-9_]*
symbols "."|"["|"]"|"("|")"|"{"|"}"|","|";"|"*"|"="|"#"

/* Rules for counting the tokens*/
%%
{strings} { fprintf(yyout, "string "); }

{constants} { fprintf(yyout, "constant "); }

{keywords} { fprintf(yyout, "keyword "); }

{operators} { fprintf(yyout, "operator "); }

{identifiers} { if (yyleng > 31){
                    fprintf(yyout, "unknown ");
                }
                else{
                    fprintf(yyout, "identifier ");
                }
            }

\n { fprintf(yyout, "\n"); }

{symbols} { fprintf(yyout, "special character "); }

[\t ]   ;

. { fprintf(yyout, "unknown "); }

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
    
    yylex();
    return 0;
}
