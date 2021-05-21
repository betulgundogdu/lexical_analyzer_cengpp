#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#define MAXCHAR 1000 //maxchar on a line
#define KEYWORD_COUNT 18 //count of keywords
#define OP_COUNT 7


// Declaration of enum
typedef enum { F, T } boolean;

// global variables
boolean endOfLine = F;
boolean hasIdentifier = F;
boolean hasConstant = F;
boolean hasOperator = F;
boolean hasKeyword = F;
boolean hasLeftPar = F;
boolean hasLeftCurly = F;
boolean hasLeftSquare = F;
boolean hasCommentStart = F;

//functions
int parseLine(char *str, int lineCount); // read line & remove case sensivity & find comments & detect structure
boolean isIdentifier(char *str, int lineCount); // find identifiers & make controls & create token 
boolean isConstant(char *str, int lineCount); // find integer and string constants & make controls & create token 
boolean isOperator(char *str, int lineCount); // find operators & make controls & create token 
boolean isKeyword(char *str, int lineCount); // find keywords & make controls & create token 
int convertLex(char *str, int lineCount); // write a token on lex file created

void removeSpaces(char* s) {
    const char* d = s;
    do {
        while (*d == ' ') {
            ++d;
        }
    } while (*s++ = *d++);
}

char* toLower(char* s) {
  for(char *p=s; *p; p++) *p=tolower(*p);
  return s;
}

char *keywords[KEYWORD_COUNT] = {
    "break","case","char","const","continue","do","else","enum","float","for","goto","if","int","long","record","return","static","while"
};

char *operators[OP_COUNT] = {
    "+","-","*","/","++","--",":="
};

// struct for errors
struct Error
{
    int errorId;
    char *errorName;
    char *errorDetail;
};

struct Error typeOfError[21] = {
                            // identifier errors
                            {0, "identifier_syntax_lack_of_colon", "Error: Expected `:` before `=` token."},
                            {1, "identifier_syntax_lack_of_equal_sign", "Error: Expected `=` after `:` token."},
                            {2, "identifier_syntax_too_long", "Error: Variable name is too long. It should be maximum 20 characters."},
                            {3, "identifier_syntax_first_char", "Error: Variable name should be start with a letter."},
                            {4, "identifier_syntax_unexpected_char", "Error: Variable name should be included only letter, number or `_`."},
                            // integer constant errors
                            {5, "integer_constant_syntax_too_big", "Error: Maximum integer size is 10 digits."},
                            {6, "integer_constant_syntax_negative_values", "Error: Negative values are not supported."},
                            // operator errors
                            {7, "operators_syntax_invalid", "Error: Invalid operators. Valid operators of the language are `+,-,*,/,++,--,:=`."},
                            // bracket errors
                            {8, "brackets_syntax_lack_of_LeftPar", "Error: There should be `(` in this line."},
                            {9, "brackets_syntax_lack_of_RightPar", "Error: There should be `)` in this line."},
                            {10, "brackets_syntax_lack_of_LeftCurly", "Error: There should be `{` in this line."},
                            {11, "brackets_syntax_lack_of_RightCurly", "Error: There should be `}` in this line."},
                            {12, "brackets_syntax_lack_of_LeftSquare", "Error: There should be `[` in this line."},
                            {13, "brackets_syntax_lack_of_RightSquare", "Error: There should be `]` in this line."},
                            // string errors
                            {14, "string_syntax_invalid_double_quote", "Error: String should be started with double quote. "},
                            // keyword error
                            {15, "keyword_syntax_invalid", "Error: Invalid keyword. "},
                            // comment errors
                            {16, "comment_syntax_lack_of_right_star", "Error: Comment should be ended with `*` before `)` token. "},
                            {17, "comment_syntax_lack_of_left_star", "Error: Comment should be started with `*` after `(` token. "},
                            {18, "comment_syntax_lack_of_right_par", "Error: Comment should be ended with `)` after `*` token. "},
                            {19, "comment_syntax_lack_of_left_par", "Error: Comment should be started with `(` before `*` token. "},
                            // end of line error
                            {20, "syntax_lack_of_semicolon", "Error: Expected `;` end of line."}
                        };


int main() {
    FILE *fp;
    char str[MAXCHAR];
    char* filename = "test.txt";
    int lineCount = 1;
    fp = fopen(filename, "r"); // start to read file
    if (fp == NULL){
        printf("Could not open file %s", filename);
        return 1;
    }
    while (fgets(str, MAXCHAR, fp) != NULL){ // read file line by line
        if(strcmp(str, "\n") == 0 || strcmp(str,"\r\n") == 0) {
            printf("\n--Line %d: %s\n", lineCount, str);
            lineCount++;     
        }
        else {
            printf("\n--Line %d: %s\n", lineCount, str);
            parseLine(str, lineCount);
            lineCount++;    
        }     
    }
    fclose(fp);
    return 0;
}

int parseLine(char *str, int lineCount){
    isIdentifier(str, lineCount);
    return 0;
}

boolean isIdentifier(char *str, int lineCount){
    char *colon = ":";
    removeSpaces(str);
    char *identifier = strtok(str, colon);
    char *value = strtok(NULL, colon);
    printf("//////   %s-%s", identifier, value);
    if(value[0] == '\0') {
        // Error id: 0, "identifier_syntax_lack_of_colon"
        return F; 
    } else {
        if( value[0] != 61) { // if equal sign does not come after colon
            // Error id: 1, "identifier_syntax_lack_of_equal_sign"
            return F;
        }
        else {
            int init_size = strlen(identifier);
            // controls to valid variable name or not
            if (init_size > 20) { // size control of identifier
                // Error id= 2, "identifier_syntax_too_long"
                return F; 
            }
            // first char control
            else if ( !(identifier[0] > 64 && identifier[0] < 91) ||  !(identifier[0] > 96 && identifier[0] < 122)){
                // Error id= 3, "identifier_syntax_first_char"
                return F;
            }
            else {
                // controls of every char of string is not number,letter or underscore
                for(int i = 0; i < init_size; i++){
                    if (!(identifier[i] > 64 && identifier[i] < 91) ||  !(identifier[i] > 96 && identifier[i] < 122) || !(identifier[i] > 47 && identifier[i] < 58) || identifier[i] != 95){
                        // Error id= 4, "identifier_syntax_unexpected_char"
                        return F;
                    }               
                }
                // Example Token = Identifier(identifier)
                printf("\nIdentifier(%s)\n", identifier);
                isConstant(value, lineCount);
                return T;  
            }            
        }
    }
}

boolean isConstant(char *str, int lineCount){
    char *endToken;
    long int ret;
    int integerConstant;
    char *stringConstant;
    ret = strtol(str, &endToken, 10);
    if(ret == 0 && (endToken[0] == '\0')){ // integer constant = 0 statement control
        integerConstant = 0;
        return T;
        printf("\nIntConst(%d)\n", integerConstant);
        // Example Token: IntConst(integerConstant) 
    } 
    else if (ret != 0 && (endToken[0] != '\0')) { // any integer constant
        integerConstant = ret;
        // Control for valid integer constant or not
        if (integerConstant < 0) {
            // Error id= 6, "integer_constant_syntax_negative_values",
            return F;
        }
        else if (integerConstant / 1000000000 > 0){
            // Error id= 5, "integer_constant_syntax_too_big"
            return F;
        }
        else {
            return T;
            // Example Token: IntConst(integerConstant) 
            printf("\nIntConst(%d)\n", integerConstant);

        }
    }
    else if (ret == 0 && !(endToken[0] != '\0')){ // string constant statement
        stringConstant = endToken;
        int countDoubleQuote = 0;
        // string constant control to include extra double quote or not
        for (int i = 0; i < strlen(stringConstant); i++){
            if (stringConstant[i] == 34) {
                countDoubleQuote++;
            } 
        }

        if(countDoubleQuote % 2 == 1) {
            // Error id= 14, "string_syntax_invalid_double_quote"
            return F;
        }
        return T;
        printf("\nStringConst(%s)\n", stringConstant);
    }
    // else {
    //     stringConstant = endToken;
    //     // maybe have some problems
    //     return F;
    // }
}

boolean isOperator(char *str, int lineCount){
    for(int i = 0; i < OP_COUNT; i++){
        if(strcmp(str, operators[i]) == 0) {
            // Example Token: Operator(str)
            return T;
        } 
    }
    // Error id= 7, "operators_syntax_invalid"
    return F;
}

boolean isKeyword(char *str, int lineCount){
    for(int i = 0; i < KEYWORD_COUNT; i++){
        if(strcmp(str, keywords[i]) == 0) {
            // Example Token: Keyword(str)
            return T;
        }
    }
    // Error id= 11, "keyword_syntax_invalid"
    return F;
}

int convertLex(char *str, int lineCount){}



/*
        // char to string to compare
        char char2str[2];// this is just temporary array to merge with      
        char2str[0] = str[i];
        char2str[1] = '\0';
*/