#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#define MAXCHAR 1000 //maxchar on a line

int parse_line(char *str, int line_count);         // read line & remove case sensivity & find comments & detect structure
int find_identifiers(char *str, int line_count);   // find identifiers & make controls & create token
int find_constants(char *str, int line_count);     // find integer and string constants & make controls & create token
int find_operators(char *str, int line_count);     // find operators & make controls & create token
int find_brackets(char *str, int line_count);      // find brackets & make controls & create token
int find_keywords(char *str, int line_count);      // find keywords & make controls & create token
int write_token_on_lex(char *str, int line_count); // write a token on lex file created
int show_error(char *type, int line_count);        // show error with type and line count
char *strremove(char *str, const char *sub);
char *identifiers[1000];
int identifier_count = 0;

//////////////////////daha düzenlenecek bu kod.
int main()
{
    FILE *fp;
    char str[MAXCHAR];
    char *filename = "code.ceng";
    int line_count = 1;

    fp = fopen(filename, "r"); // start to read file
    if (fp == NULL)
    {
        printf("Could not open file %s", filename);
        return 1;
    }
    while (fgets(str, MAXCHAR, fp) != NULL)
    { // read file line by line
        parse_line(str, line_count);
        find_identifiers(str, line_count);
        line_count++;
    }
    printf("\n");
    fclose(fp);
    return 0;
}

int parse_line(char *str, int line_count)
{

    /*
        End of line: ; 
        Example Token: EndOfLine 
        Anything between (* and *) is a comment.
        If a comment cannot terminate before the file end, there should be a lexical error issued.
        Comments are just like blank space and they provide no tokens.
    */

    //printf("Line %d: %s", line_count, str);
    // remove case sensitivity
    // escape whitespaces more than one
    // switch case for splitting
    //printf("deneme\n");

    return 0;
}

int find_identifiers(char *str, int line_count)
{
    /*
        maximum identifier size is 20 characters. If you use an identifier larger than that, the lexical analyzer issues an error message.
        ceng++ language is not case sensitive
        identifiers start with an alphabetic character (a letter) and are composed of one or more letters,digits or_ (underscore)
        Example Token: Identifier(my_var_1) 
    */
    char *identifier;
    char *righthandside;
    const char *operators[7] = {"+", "-", "*", "/", "++", "--", ":="};
    const char *keywords[20] = {"break", "case", "char", "const", "continue", "do", "else", "enum", "float", "for", "goto", "if", "int", "long", "record", "return", "static", "while"};
    char *left;
    char *right;
    int endofline = 0;

    str = strremove(str, "(*");
    str = strremove(str, "*)");
    for (int i = 0; i < 18; i++)
    {
        if (strstr(str, keywords[i]) != NULL)
        {
            printf("Keyword(%s) \n", keywords[i]);
            str = strremove(str, keywords[i]);
        }
    }
    find_brackets(str,line_count);
    if (strstr(str, ";") != NULL)
        endofline = 1;
   
    
        if (strstr(str, ":=") != NULL)
        {
            identifier = strtok(str, ":=");
            while (righthandside != NULL)
            {

                righthandside = strtok(NULL, " :=");
            }

            if (strlen(identifier) > 20)
            {
                printf("Idetifier size is exceeded. Maximum identifier size must be 20. \n");
                exit(0);
            }

            else
            {
                if (isalpha(identifier[0]))
                {
                    printf("Identifier(%s) \n", identifier);
                    printf("Operator(:=)\n");
                }
                else
                {
                    printf("Idetifiers must begin with an alphabet. \n");
                    exit(0);
                }
            }
        }
    

    for (int i = 0; i < 7; i++)
    {
        if (strstr(str, operators[i]) != NULL)
        {
            printf("Operator(%s) \n", operators[i]);
        }
    }

    if (endofline == 1)
    {
        printf("EndOfLine\n");
        endofline = 0;
    }

    return 0;
}
int find_constants(char *str, int line_count)
{
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

int find_operators(char *str, int line_count)
{
    /*  valid operators of the language are +,-,*,/,++,--,:=
        Example Token: Operator(++) 
    */
}
int find_brackets(char *str, int line_count)
{
    /*
        LeftPar: ( RightPar: )
        LeftSquareBracket: [ RightSquareBracket: ]
        LeftCurlyBracket: { RightCurlyBracket: }
        Example Token: LeftCurlyBracket 
    */
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

int find_keywords(char *str, int line_count)
{
    /*
        Keywords are:
        break,case,char,const,continue,do,else,enum,float,for,goto,if,int,long,record,return,static,while
        Ceng++ language is not case sensitive and all the keywords are standardized as lower case. 
        You can write the same word as “while” OR “While” OR “WHILE” and they all generate the
        Example Token: Keyword(while)
    */
}

int write_token_on_lex(char *str, int line_count) {}

int show_error(char *type, int line_count) {}

char *strremove(char *str, const char *sub)
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
