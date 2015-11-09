/* 
 * File:   main.c
 * Author: dishanm
 *
 * Created on October 29, 2015, 10:57 AM
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

enum TokenTypeEnum{ StartTag, EndTag, String, None };

typedef enum TokenTypeEnum TokenType;  

struct Token {
     char *text;
     struct Token *next;
};

struct TreeNode {
struct Token * token;
struct TreeNode *parent;  
struct TreeNode *children[100];
int childrenCount;
};

long getLength(FILE* file);
FILE* getFile();
struct Token *tokenize(FILE *file);
bool isUsableToken(char *text);
void printTokenList(struct Token *root);
struct Token * addTokenToList(char* text, struct Token *current, int n);
void buildTree(struct Token* root);
TokenType getTokenType(struct Token* token);


int main(int argc, char** argv) {

    FILE* file = getFile();
    
    long size = getLength(file);
    printf ("Size of file: %ld bytes.\n",size);
    
    struct Token * root = tokenize(file);
    // printTokenList(root);
    buildTree(root);
    return (EXIT_SUCCESS);
}

struct Token * tokenize(FILE* file)
{
    char *tokenChars = NULL;
    size_t n = 0;
    int c;
    
    /* This won't change, or we would lose the list in memory */
     struct Token *root;       
    /* This will point to each node as it traverses the list */
     struct Token *current;  

    root = malloc( sizeof(struct Token) );  
    current = root; 

    while ((c = fgetc(file)) != EOF)
    {
        if(c =='<')
        {
            if(isUsableToken(tokenChars))
            {
              current = addTokenToList(tokenChars,current, n);
            }
            
            if(n !=0)
            {
                n=0;
                tokenChars = NULL;
            }
        }
        
        if(c>31)
        {
            tokenChars = realloc(tokenChars, n+1);
            tokenChars[n++] = (char) c;
        }
        
        if( c =='>')
        {
            if(isUsableToken(tokenChars))
            {
               current=  addTokenToList(tokenChars,current, n);
            }
            
            // Reset
            if(n !=0)
            {
                n=0;
                tokenChars = NULL;
            }
        }
        
    }
    return root;
}

void buildTree(struct Token* root)
{
    struct Token *current = root;
    struct TreeNode *treeRoot = malloc( sizeof(struct TreeNode) );
    treeRoot->token = NULL;
    treeRoot->childrenCount = 0;
    struct TreeNode *currentTreeNode = treeRoot;

    while(current!= NULL)
    {
        if(current->text!=NULL)
        {
            TokenType type = getTokenType(current);

            if(type == StartTag)
            {
               printf("Start Tag: %s\n",current->text); 

               if(currentTreeNode->token == NULL)
               {
                    currentTreeNode->token = current;
               }else
               {
                    printf("Added Child: %d to: %s\n",currentTreeNode->childrenCount,currentTreeNode->token->text); 
                    struct TreeNode *createdNode = malloc( sizeof(struct TreeNode) ); 
                    currentTreeNode->children[currentTreeNode->childrenCount] = createdNode;
                    currentTreeNode->childrenCount++;
                    createdNode->parent = currentTreeNode;
                    createdNode->token = current;
                    createdNode->childrenCount =0;
                    currentTreeNode = createdNode;
               }
               
            }
            else if(type == EndTag)
            {
                printf("End Tag: %s\n",current->text); 
                currentTreeNode = currentTreeNode->parent;
            }
            
        }

        current = current->next;
    }

}

TokenType getTokenType(struct Token* token)
{
    if(token==NULL)
    {
        return None;
    }

   //Todo: token does not always have to be length>=2
    if(token->text[0]=='<')
    {
        if(token->text[1]=='/')
        {
            return EndTag;
        }
        return StartTag;
    }

    return String;
}

struct Token * addTokenToList(char* text, struct Token *current, int n)
{
    text = realloc(text, n+1);
    text[n] = '\0'; 
            
    current->text = text;
    /* Creates a node at the end of the list */
    current->next = malloc( sizeof(struct Token) );  
    current = current->next; 
    current->next = NULL;
    current->text = NULL;
    
    return current;
}

void printTokenList(struct Token* root)
{
    struct Token *current = root;
    
    while(current!= NULL)
    {
        if(current->text != NULL)
        {
            printf("Token: %s\n",current->text);
        }

         current = current->next;
    }
}

bool isUsableToken(char* text)
{
    if(text==NULL)
    {
        return false;
    }

    if(text[1]=='?') // ignore XML declaration.
    {
        return false;
    }

    char* c;
    bool valid = false;
    for(c=text; *c!='\0';c++)
    {
        if(*c>32)
        {
            valid = true;
            break;
        }
    }

    return valid;  
}

FILE * getFile()
{
    FILE* file;
    file = fopen("C:\\Temp\\note.xml","r");

    if (file == NULL) {
    fprintf(stderr, "Can't open input FILE!\n");
    exit(1);
    }
    
    return file;
}

long getLength(FILE* file)
{
    fseek(file, 0L, SEEK_END);
    long size = ftell(file);
    fseek(file, 0L, SEEK_SET);
    return size;
}
