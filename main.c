#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#define MAXCHAR 1000 //maxchar on a line
#define KEYWORD_COUNT 18 //count of keywords
#define OP_COUNT 7
#define BRACKET_COUNT 6


// Declaration of enum
typedef enum { F, T } boolean;

// global variables
char *semicolon = ";";
char *colon = ":";
boolean endOfLine = F;
boolean commentLine = F;
boolean needCurlyLeftBrackets = F;
boolean needCurlyRightBrackets = F;
boolean needSquareLeftBrackets = F;
boolean needSquareRightBrackets = F;
boolean needLeftPar = F;
boolean needRightPar = F;

//functions
int parseLine(char *str, int lineCount); // read line & remove case sensivity & find comments & detect structure
int findIdentifier(char *str, int lineCount); // find identifiers & make controls & create token 
int findConstant(char *str, int lineCount); // find integer and string constants & make controls & create token 
int findOperator(char *str, int lineCount); // find operators & make controls & create token 
int findKeyword(char *str, int lineCount); // find keywords & make controls & create token 
int *findBrackets(char *str, int lineCount); // find keywords & make controls & create token 
int writeErrorOnLex(char *str, int lineCount); // write a error on lex file created
int writeErrorOnLexWithArgument (char *str, int lineCount, char token); // write error on lex file created
int writeTokenOnLex(char *str, char *token); // write a token on lex file created
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

char *brackets[BRACKET_COUNT] = {
    "(", ")", "{", "}", "[", "]"
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
    char* filename = "code.Ceng";
    int lineCount = 1;
    fp = fopen(filename, "r"); // start to read file
    if (fp == NULL){
        printf("Could not open file %s", filename);
        return 1;
    }

    while (fgets(str, MAXCHAR, fp) != NULL){ // read file line by line
        removeSpaces(str);
        if(strcmp(str, "\n") == 0 || strcmp(str,"\r\n") == 0) {
            lineCount++;     
        }
        else {
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
            commentLine = T;
        } else {
            rightHandSideInComment = strtok(NULL, "*)");
            commentLine = F;
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
    findKeyword(str, lineCount); 
    findBrackets(str, lineCount);  
    findIdentifier(str, lineCount);
    findOperator(str, lineCount);
}

int findIdentifier(char *str, int lineCount){
    if(strstr(str, ":=") != NULL){
        char *identifier = strtok(str, colon);
        if(findOperator(str, lineCount) == 1){       
            char *value = strtok(NULL, colon);
            if(value[0] == '\0') {
                writeErrorOnLex("Line %d, Error id: 0, identifier_syntax_lack_of_colon\n", lineCount);
                return 1; 
            } else {
                if( value[0] != 61) { // if equal sign does not come after colon
                    writeErrorOnLex("Line %d, Error id: 1, identifier_syntax_lack_of_equal_sign\n", lineCount);
                    return 1;
                }
                else {                 
                    int init_size = strlen(identifier);
                    // controls to valid variable name or not
                    if (init_size > 20) { // size control of identifier
                        writeErrorOnLex("Line %d, Error id: 2, identifier_syntax_too_long\n", lineCount);
                        return 1; 
                    }
                    // first char control
                    else if ( !isalpha(identifier[0]) ){
                        writeErrorOnLex("Line %d, Error id: 3, identifier_syntax_first_char\n", lineCount);
                        return 1;
                    }
                    else {
                        // controls of every char of string is not number,letter or underscore
                        for(int i = 0; i < init_size; i++){
                            if ( !(isalpha(identifier[i]) || isdigit(identifier[i]) || identifier[i] == 95) ){
                                writeErrorOnLexWithArgument("Line %d, Error id: 4, identifier_syntax_unexpected_char : you can not use `%c` in a variable name.\n", lineCount, identifier[i]);
                                return 1;
                            }               
                        }
                        writeTokenOnLex("Identifier(%s)\n", identifier);
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
        char number[20];
	    sprintf(number, "%d", integerConstant);   
        writeTokenOnLex("IntConst(%d)\n", number);
        return 0;
    } 
    else if (ret != 0 && (endToken[0] == '\0')) { // any integer constant
        integerConstant = ret;
        // Control for valid integer constant or not
        if (integerConstant < 0) {
            writeErrorOnLex("Line %d, Error id: 6, integer_constant_syntax_negative_values\n", lineCount);
            return 1;
        }
        else if (integerConstant / 1000000000 > 0){
            writeErrorOnLex("Line %d, Error id: 5, integer_constant_syntax_too_big\n", lineCount);
            return 1;
        }
        else {
            char number[20];
	        sprintf(number, "%d", integerConstant);
            writeTokenOnLex("IntConst(%d)\n", number);
            return 0;


        }
    }
    else if (ret == 0 && endToken[0] != '\0'){ // string constant statement
        int countDoubleQuote = 0;
        stringConstant = endToken;
        // string constant control to include extra double quote or not
        for (int i = 0; i < strlen(stringConstant); i++){
            if (stringConstant[i] == '\"') {
                countDoubleQuote++;
            } 
        }
        if(countDoubleQuote % 2 == 1) {
            writeErrorOnLex("Line %d, Error id: 14, string_syntax_invalid_double_quote\n", lineCount);
            return 1;
        }
        else {
            stringConstant = strRemove(endToken, "\"");
            writeTokenOnLex("StringConst(%s)\n", stringConstant);
            return 0;
        }

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
            writeTokenOnLex("Operator(%s)\n", operators[i]);
            return 0;
        } 
    }
    //printf("Line %d, Error id: 7, operators_syntax_invalid\n", lineCount);
    return 1;
}

int findKeyword(char *str, int lineCount){
    char *keyword;
    int indexOfKeyword;
    for(int i = 0; i < KEYWORD_COUNT; i++){
        if(strstr(str, keywords[i]) != NULL) {
            indexOfKeyword = (int)(strstr(str, keywords[i]) - str);
            keyword = keywords[i];
            switch (i)
            {
                case 0: //break keyword control
                case 4: //continue
                case 15: // return
                    if(endOfLine == T)
                        endOfLine = F;    
                    break;
                case 1: ;//case keyword control
                    int indexOfColon = (int)(strchr(str, ':') - str);
                    if(indexOfColon != indexOfKeyword + 4)
                        writeErrorOnLex("Error: Line %d, colon is not use end of case.", lineCount);
                    break;       
                case 2: //char keyword control
                case 3: //const keyword control
                case 7: //enum
                case 8: // float
                case 10: // goto
                case 12: // int
                case 13: // long
                case 16: ;// static
                    int lenOfKeyword = strlen(keyword);
                    if(strstr(str, keyword) != NULL){
                        int index = (int)(strstr(str, keyword) - str);
                        int shift = index + lenOfKeyword;
                        memmove(str, str+shift, strlen(str));
                        findIdentifier(str, lineCount);
                    }
                    break;  
                case 5: // do keyword control
                case 6: // else keyword control
                    if(findBrackets(str, lineCount) == NULL){
                        needCurlyLeftBrackets = T;
                        needCurlyLeftBrackets = T;
                    }     
                    break;
                case 9: // for
                case 11: // if
                case 14: // record
                case 17:  // while
                    break;
                default:
                    break;
            }
            writeTokenOnLex("Keyword(%s)\n", keyword);
        }
    }
    // printf("Line %d, Error id: 11, keyword_syntax_invalid\n", lineCount);
    return 0;
}

int *findBrackets(char *str, int lineCount)
{  
    for (int i = 0; i < BRACKET_COUNT; i++) {
        if(strstr(str, brackets[i]) != NULL){
            int indexOfBracket = (int)(strstr(str, brackets[i]) - str);
            switch (i)
            {
                case 0:
                    writeTokenOnLex("%s\n", "LeftPar");
                    break;
                case 1:
                    writeTokenOnLex("%s\n", "RightPar");
                    break;
                case 2:    
                    writeTokenOnLex("%s\n", "LeftSquareBracket");
                    break;
                case 3:
                    writeTokenOnLex("%s\n", "RightSquareBracket");
                    break;
                case 4:
                    writeTokenOnLex("%s\n", "LeftCurlyBracket");
                    break;
                case 5:
                    writeTokenOnLex("%s\n", "RightCurlyBracket");
                    break;
                default:
                    break;
            }
        }
        return 0;
    }
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


int writeErrorOnLex(char *str, int lineCount){
    /* File pointer to hold reference to our file */
    FILE * fPtr;
    /* 
     * Open file in w (write) mode. 
     */
    fPtr = fopen("code.lex", "a");

    /* fopen() return NULL if last operation was unsuccessful */
    if(fPtr == NULL)
    {
        /* File not created hence exit */
        printf("Unable to create file.\n");
        exit(EXIT_FAILURE);
    }

    fprintf(fPtr, str, lineCount);
    fclose(fPtr);
}

int writeTokenOnLex(char *str, char *token){
    /* File pointer to hold reference to our file */
    FILE * fPtr;
    /* 
     * Open file in w (write) mode. 
     */
    fPtr = fopen("code.lex", "a");

    /* fopen() return NULL if last operation was unsuccessful */
    if(fPtr == NULL)
    {
        /* File not created hence exit */
        printf("Unable to create file.\n");
        exit(EXIT_FAILURE);
    }

    fprintf(fPtr, str, token);
    fclose(fPtr);
}


int writeErrorOnLexWithArgument (char *str, int lineCount, char token){
    /* File pointer to hold reference to our file */
    FILE * fPtr;
    /* 
     * Open file in w (write) mode. 
     */
    fPtr = fopen("code.lex", "a");

    /* fopen() return NULL if last operation was unsuccessful */
    if(fPtr == NULL)
    {
        /* File not created hence exit */
        printf("Unable to create file.\n");
        exit(EXIT_FAILURE);
    }

    fprintf(fPtr, str, token);
    fclose(fPtr);
}

/*
        // char to string to compare
        char char2str[2];// this is just temporary array to merge with      
        char2str[0] = str[i];
        char2str[1] = '\0';
*/