#include <stdio.h>
#define MAXCHAR 1000 //maxchar on a line

int parse_line(char *str, int line_count); // read line & remove case sensivity & find comments & detect structure
int find_identifiers(char *str, int line_count); // find identifiers & make controls & create token 
int find_constants(char *str, int line_count); // find integer and string constants & make controls & create token 
int find_operators(char *str, int line_count); // find operators & make controls & create token 
int find_brackets(char *str, int line_count); // find brackets & make controls & create token 
int find_keywords(char *str, int line_count); // find keywords & make controls & create token 
int write_token_on_lex(char *str, int line_count); // write a token on lex file created
int show_error(char* type, int line_count); // show error with type and line count

int main() {
    FILE *fp;
    char str[MAXCHAR];
    char* filename = "test.txt";
    int line_count = 1;
    fp = fopen(filename, "r"); // start to read file
    if (fp == NULL){
        printf("Could not open file %s", filename);
        return 1;
    }
    while (fgets(str, MAXCHAR, fp) != NULL){ // read file line by line
        parse_line(str, line_count);
        line_count++;
    }
    fclose(fp);
    return 0;
}

int parse_line(char *str, int line_count){
    /*
        End of line: ; 
        Example Token: EndOfLine 
        Anything between (* and *) is a comment.
        If a comment cannot terminate before the file end, there should be a lexical error issued.
        Comments are just like blank space and they provide no tokens.
    */
    printf("Line %d: %s", line_count, str);
    // remove case sensitivity
    // escape whitespaces more than one
    // switch case for splitting
    return 0;
}

int find_identifiers(char *str, int line_count){
    /*
        maximum identifier size is 20 characters. If you use an identifier larger than that, the lexical analyzer issues an error message.
        ceng++ language is not case sensitive
        identifiers start with an alphabetic character (a letter) and are composed of one or more letters,digits or_ (underscore)
        Example Token: Identifier(my_var_1) 
    */
}

int find_constants(char *str, int line_count){
    /* 
        max integer size is 10 digits
        negative values are not supported 
        Example Token: IntConst(352) 
        string constants of Ceng++ are delimited by double quotes (ASCII code 34) as in “this is a string”
        string constants have unlimited size
        string constants cannot contain the double quote character. when you reach one, the string terminates.
        if a string constant cannot terminate before the file end, there should be a lexical error issued.
     */
}

int find_operators(char *str, int line_count){
    /*  valid operators of the language are +,-,*,/,++,--,:=
        Example Token: Operator(++) 
    */
}
int find_brackets(char *str, int line_count){
    /*
        LeftPar: ( RightPar: )
        LeftSquareBracket: [ RightSquareBracket: ]
        LeftCurlyBracket: { RightCurlyBracket: }
        Example Token: LeftCurlyBracket 
    */
}

int find_keywords(char *str, int line_count){
    /*
        Keywords are:
        break,case,char,const,continue,do,else,enum,float,for,goto,if,int,long,record,return,static,while
        Ceng++ language is not case sensitive and all the keywords are standardized as lower case. 
        You can write the same word as “while” OR “While” OR “WHILE” and they all generate the
        Example Token: Keyword(while)
    */
}

int write_token_on_lex(char *str, int line_count){}

int show_error(char* type, int line_count){}

