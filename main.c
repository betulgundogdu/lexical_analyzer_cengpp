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
boolean endOfLine = T;
boolean commentLine = F;
boolean needBrackets = F;

//functions
int parseLine(char *str, int lineCount); // read line & remove case sensivity & find comments & detect structure
int findIdentifier(char *str, int lineCount); // find identifiers & make controls & create token 
int findConstant(char *str, int lineCount); // find integer and string constants & make controls & create token 
int findOperator(char *str, int lineCount); // find operators & make controls & create token 
int findKeyword(char *str, int lineCount); // find keywords & make controls & create token 
int findBrackets(char *str, int lineCount); // find keywords & make controls & create token 
int writeErrorOnLex(char *str, int lineCount); // write a error on lex file created
int writeErrorOnLexWithCharArgument (char *str, int lineCount, char token); // write error with char argument on lex file created
int writeErrorOnLexWithIntegerArgument (char *str, int lineCount, int integer); // write error with integer argument on lex file created
int writeStringTokenOnLex(char *str, char *token); // write a string token on lex file created
int writeIntegerTokenOnLex(char *str, int token); // write a integer token on lex file created
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
    "++","--",":=", "+","-","*","/"
};

char *brackets[BRACKET_COUNT] = {
    "(", ")", "{", "}", "[", "]"
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

    //missing brackets control
    if(needBrackets == T) {
        if(findBrackets(str, lineCount) == 1){
            writeErrorOnLex("Line %d, missing brackets \n", lineCount);
        }
        else {
            needBrackets = F;
        }
    }

    // comment line control 
    if (strstr(str, "(*") != NULL){
        commentLine = T;
        if(strstr(str, "*)") != NULL){
            commentLine = F;
            int indexOfCommentStart = (int)(strstr(str, "(*") - str);
            char * dest;
            strncpy(dest, str, strlen(str) - indexOfCommentStart); // detect start of comment and get strings after it.   
            strcpy(str, dest);
        }
        
    }
    if(commentLine == T){
        if(strstr(str, "*)") != NULL){
            commentLine = F;
            int indexOfCommentEnd = (int)(strstr(str, "*)") - str);
            memmove(str, str+indexOfCommentEnd, strlen(str)); // detect end of comment and get strings after it.
        }
    }    
    else{ // if strings are not in the comment line
        if(strstr(str, "*)") != NULL){
            writeErrorOnLex("Line %d, missing comment start `(*` \n", lineCount);
        }
        else {
            strRemove(str, "\n"); // remove last char for new line \n
            findKeyword(str, lineCount); 
            findBrackets(str, lineCount);  
            findIdentifier(str, lineCount);

            // end of line control
            if(strstr(str, semicolon) != NULL || endOfLine == T) {           
                writeStringTokenOnLex("%s\n", "EndOfLine");    
            }
            else {
                writeErrorOnLex("Line %d, missing semicolon end of line\n", lineCount);
            }
        }
    }
}

int findIdentifier(char *str, int lineCount){
    if(strstr(str, ":=") != NULL){
        char *identifier = strtok(str, colon);
        if(findOperator(str, lineCount) == 1){       
            char *value = strtok(NULL, colon);
            if( value[0] != '=') { // if equal sign does not come after colon
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
                            writeErrorOnLexWithCharArgument("Line %d, Error id: 4, identifier_syntax_unexpected_char : you can not use `%c` in a variable name.\n", lineCount, identifier[i]);
                            return 1;
                        }               
                    }
                    writeStringTokenOnLex("Identifier(%s)\n", identifier);
                    // end of line control
                    if(strstr(value, ";") != NULL) {
                        endOfLine = T;
                        strRemove(value, ";");
                    } else {
                        endOfLine = F;
                    }
                    memmove(value, value+1, strlen(value));
                    findConstant(value, lineCount);                  
                    return 0;  
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
        writeIntegerTokenOnLex("IntConst(%d)\n", integerConstant);
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
            writeIntegerTokenOnLex("IntConst(%d)\n", integerConstant);
            return 0;
        }
    }
    else if (ret == 0 && endToken[0] != '\0'){ // string constant statement
        int countDoubleQuote = 0;
        stringConstant = endToken;
        // find that it is statement or constant
        if(findOperator(str, lineCount) != 0) {
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
                writeStringTokenOnLex("StringConst(%s)\n", stringConstant);
                return 0;
            }
        }

    }
    else {
       return 0;
    }
}

int findOperator(char *str, int lineCount){
    for(int i = 0; i < OP_COUNT; i++){
        if(strstr(str, operators[i]) != NULL) {
            writeStringTokenOnLex("Operator(%s)\n", operators[i]);
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
                case 16: ; // static
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
                    if(findBrackets(str, lineCount) == 1){
                        writeErrorOnLex("Line %d, Error id: 10, brackets_syntax_lack_of_LeftCurly, Error: There should be `{` in this line.\n", lineCount);
                        writeErrorOnLex("Line %d, Error id:11, brackets_syntax_lack_of_RightCurly, Error: There should be `}` in this line.\n", lineCount);
                    } else {

                    }    
                    break;
                case 9: // for
                case 11: // if
                case 14: // record
                case 17:  // while
                    if(findBrackets(str, lineCount) == 1){
                        writeErrorOnLex("Line %d, Error id:8, brackets_syntax_lack_of_LeftPar, Error: There should be `(` in this line.\n", lineCount);
                        writeErrorOnLex("Line %d, Error id:9, brackets_syntax_lack_of_RightPar, Error: There should be `)` in this line.\n", lineCount);
                    } 
                    else {
                        needBrackets = T;
                        int startIndex = (int)(strstr(str, "(") - str);
                        int endIndex = (int)(strstr(str, ")") - str);
                        memmove(str, str+startIndex, strlen(str) - endIndex);
                        findIdentifier(str, lineCount);
                    } 
                    break;
                default:
                    break;
            }
            writeStringTokenOnLex("Keyword(%s)\n", keyword);
        }
    }
    // printf("Line %d, Error id: 11, keyword_syntax_invalid\n", lineCount);
    return 0;
}

int findBrackets(char *str, int lineCount)
{  
    for (int i = 0; i < BRACKET_COUNT; i++) {
        if(strstr(str, brackets[i]) != NULL){
            int indexOfBracket = (int)(strstr(str, brackets[i]) - str);
            switch (i)
            {
                case 0:
                    writeStringTokenOnLex("%s\n", "LeftPar");
                    break;
                case 1:
                    writeStringTokenOnLex("%s\n", "RightPar");
                    break;
                case 2:    
                    writeStringTokenOnLex("%s\n", "LeftSquareBracket");
                    break;
                case 3:
                    writeStringTokenOnLex("%s\n", "RightSquareBracket");
                    break;
                case 4:
                    writeStringTokenOnLex("%s\n", "LeftCurlyBracket");
                    break;
                case 5:
                    writeStringTokenOnLex("%s\n", "RightCurlyBracket");
                    break;
                default:
                    break;
            }
            return 0;
        }
        return 1;
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

int writeStringTokenOnLex(char *str, char *token){
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

int writeIntegerTokenOnLex(char *str, int token){
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

int writeErrorOnLexWithCharArgument (char *str, int lineCount, char token){
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

    fprintf(fPtr, str, lineCount, token);
    fclose(fPtr);
}

int writeErrorOnLexWithIntegerArgument (char *str, int lineCount, int integer){
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

    fprintf(fPtr, str, lineCount, integer);
    fclose(fPtr);
}
