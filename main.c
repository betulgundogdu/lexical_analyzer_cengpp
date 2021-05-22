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
char *semicolon = ";";
char *colon = ":";
int endOfLine = 0;
int commentLine = 0;

//functions
int parseLine(char *str, int lineCount); // read line & remove case sensivity & find comments & detect structure
int findIdentifier(char *str, int lineCount); // find identifiers & make controls & create token 
int findConstant(char *str, int lineCount); // find integer and string constants & make controls & create token 
int findOperator(char *str, int lineCount); // find operators & make controls & create token 
int findKeyword(char *str, int lineCount); // find keywords & make controls & create token 
int findBrackets(char *str, int lineCount); // find keywords & make controls & create token 
int convertLex(char *str, int lineCount); // write a token on lex file created
char *strRemove(char *str, const char *sub);

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
        removeSpaces(str);
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
    char *left;
    char *right;
    char *identifier;
    char *rightHandSideInComment;
    char *leftHandSideInComment;
    char *rightHandSideInLine;
    char *leftHandSideInLine;    

    // comment line control
    if (strstr(str, "(*") != NULL) {
        if(strstr(str, "*)") == NULL) {
            leftHandSideInComment = strtok(str, "(*");
            commentLine = 1;
        } else {
            rightHandSideInComment = strtok(NULL, "*)");
        }      
    }

    if(leftHandSideInComment[0] != '\0'){
        str = leftHandSideInComment;
    }

    // end of line control
    // if(strstr(str, semicolon) != NULL) {
    //     endOfLine = 0; // ; varsa 
    //     leftHandSideInLine =  strtok(str, semicolon); // ard arda ; varsa ilk line
    //     // rightHandSideInLine = strtok(str, semicolon); // loop gerekiyor
    // }
    // else {
    //     endOfLine = 1;
    // }

    // if(leftHandSideInLine[0] != '\0')  {
    //     str = leftHandSideInLine;
    // }

    strRemove(str, "\n"); // remove last char for new line \n
    // keyword control
    findKeyword(str, lineCount);
    findIdentifier(str, lineCount);
    //findOperator(str, lineCount);
}

int findIdentifier(char *str, int lineCount){
    if(strstr(str, ":=") != NULL){
        char *identifier = strtok(str, colon);
        printf("Identifier(%s)\n", identifier);

        if(findOperator(str, lineCount) == 1){       
            char *value = strtok(NULL, colon);
            if(value[0] == '\0') {
                printf("Line %d, Error id: 0, identifier_syntax_lack_of_colon\n", lineCount);
                return 1; 
            } else {
                if( value[0] != 61) { // if equal sign does not come after colon
                    printf("Line %d, Error id: 1, identifier_syntax_lack_of_equal_sign\n", lineCount);
                    return 1;
                }
                else {                 
                    int init_size = strlen(identifier);
                    // controls to valid variable name or not
                    if (init_size > 20) { // size control of identifier
                        printf("Line %d, Error id: 2, identifier_syntax_too_long\n", lineCount);
                        return 1; 
                    }
                    // first char control
                    else if ( !isalpha(identifier[0]) ){
                        printf("Line %d, Error id: 3, identifier_syntax_first_char\n", lineCount);
                        return 1;
                    }
                    else {
                        // controls of every char of string is not number,letter or underscore
                        for(int i = 0; i < init_size; i++){
                            if ( !(isalpha(identifier[i]) || isdigit(identifier[i]) || identifier[i] == 95) ){
                                printf("Line %d, Error id: 4, identifier_syntax_unexpected_char : you can not use `%c` in a variable name.\n", lineCount, identifier[i]);
                                return 1;
                            }               
                        }
                        strRemove(value, ";");
                        memmove(value, value+1, strlen(value));
                        findConstant(value, lineCount);                  
                        return 0;  
                    }            
                }
            }
        }
    }
}

int findConstant(char *str, int lineCount){
    char *endToken;
    long int ret;
    int integerConstant;
    char *stringConstant;
    ret = strtol(str, &endToken, 10);
    if(ret == 0 && (endToken[0] == '\0')){ // integer constant = 0 statement control
        integerConstant = 0;
        printf("IntConst(%d)\n", integerConstant);
        return 0;
    } 
    else if (ret != 0 && (endToken[0] == '\0')) { // any integer constant
        integerConstant = ret;
        // Control for valid integer constant or not
        if (integerConstant < 0) {
            printf("Line %d, Error id: 6, integer_constant_syntax_negative_values\n", lineCount);
            return 1;
        }
        else if (integerConstant / 1000000000 > 0){
            printf("Line %d, Error id: 5, integer_constant_syntax_too_big\n", lineCount);
            return 1;
        }
        else {
            printf("IntConst(%d)\n", integerConstant);
            return 0;


        }
    }
    else if (ret == 0 && endToken[0] != '\0'){ // string constant statement
        stringConstant = strRemove(endToken, "\"");
        int countDoubleQuote = 0;
        // string constant control to include extra double quote or not
        for (int i = 0; i < strlen(stringConstant); i++){
            if (stringConstant[i] == 34) {
                countDoubleQuote++;
            } 
        }

        if(countDoubleQuote % 2 == 1) {
            printf("Line %d, Error id: 14, string_syntax_invalid_double_quote\n", lineCount);
            return 1;
        }
        printf("StringConst(%s)\n", stringConstant);
        return 0;
    }
    // else {
    //     stringConstant = endToken;
    //     // maybe have some problems
    //     return F;
    // }
}

int findOperator(char *str, int lineCount){
    for(int i = 0; i < OP_COUNT; i++){
        if(strstr(str, operators[i]) != NULL) {
            printf("Operator(%s)\n", operators[i]);
            return 0;
        } 
    }
    //printf("Line %d, Error id: 7, operators_syntax_invalid\n", lineCount);
    return 1;
}

int findKeyword(char *str, int lineCount){
    for(int i = 0; i < KEYWORD_COUNT; i++){
        if(strstr(str, keywords[i]) != NULL) {
            printf("Keyword(%s)\n", keywords[i]);
            return 0;
        }
    }
    // printf("Line %d, Error id: 11, keyword_syntax_invalid\n", lineCount);
    return 1;
}

int findBrackets(char *str, int lineCount)
{
    if(strstr(str,"(")  != NULL)
        printf("LeftPar\n");
    if(strstr(str,")")  != NULL)
        printf("RightPar\n");
    if(strstr(str,"[")  != NULL)
        printf("LeftSquareBracket\n");
    if(strstr(str,"]")  != NULL)
        printf("RightSquareBracket\n");
    if(strstr(str,"{")  != NULL)
        printf("LeftCurlyBracket\n");
    if(strstr(str,"}")  != NULL)
        printf("RightCurlyBracket\n");
}

char *strRemove(char *str, const char *sub)
{
    size_t len = strlen(sub);
    if (len > 0)
    {
        char *p = str;
        while ((p = strstr(p, sub)) != NULL)
        {
            memmove(p, p + len, strlen(p + len) + 1);
        }
    }
    return str;
}


int convertLex(char *str, int lineCount){}



/*
        // char to string to compare
        char char2str[2];// this is just temporary array to merge with      
        char2str[0] = str[i];
        char2str[1] = '\0';
*/