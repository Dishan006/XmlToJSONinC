/* 
 * File:   main.c
 * Author: dishanm
 *
 * Created on October 29, 2015, 10:57 AM
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

enum TokenTypeEnum{ StartTag, EndTag, String, None };

typedef enum TokenTypeEnum TokenType;  

struct Token {
     char *text;
     struct Token *next;
};

struct TreeNode {
char* name;
char* content;
struct TreeNode *parent;  
struct TreeNode *children[100];
int childrenCount;
};

long getLength(FILE* file);
FILE* getFile();
struct Token *tokenize(FILE *file);
bool isUsableToken(char *text);
void printTokenList(struct Token *root);
struct Token* addTokenToList(char* text, struct Token *current, int n);
struct TreeNode *buildTree(struct Token* root);
TokenType getTokenType(struct Token* token);
void treverseTree(struct TreeNode *current,  bool isLastChild, bool isArrayElement);
char* getTextFromToken(char* tokenText);
struct TreeNode* AddTokenToTreeNode(struct TreeNode *treeNode, struct Token *token);
bool areChildrenInArray(struct TreeNode *treeNode);
char* appendToString(char* original, char* toAppend);
char* buildJsonFromTree(struct TreeNode *current, char* currentString, bool isLastChild, bool isArrayElement);


int main(int argc, char** argv) {

    FILE* file = getFile();
    
    long size = getLength(file);
    printf ("Size of file: %ld bytes.\n",size);
    
    struct Token * root = tokenize(file);
    // printTokenList(root);
    struct TreeNode *treeRoot = buildTree(root);
    // treverseTree(treeRoot, true, false);
    char* json = NULL;
    json = buildJsonFromTree(treeRoot,json,true,false);
    printf("%s\n", json);
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

struct TreeNode* buildTree(struct Token* root)
{
    struct Token *current = root;
    struct TreeNode *treeRoot = malloc( sizeof(struct TreeNode) );
    treeRoot->name = NULL;
    treeRoot->content = NULL;
    treeRoot->childrenCount = 0;
    struct TreeNode *currentTreeNode = treeRoot;

    while(current!= NULL)
    {
        if(current->text!=NULL)
        {
            TokenType type = getTokenType(current);
            char* text = getTextFromToken(current->text);

            if(type == StartTag)
            {
               //printf("Start Tag: %s\n",text); 

               if(currentTreeNode->name == NULL)
               {
                    currentTreeNode = AddTokenToTreeNode(currentTreeNode,current);
               }else
               {
                    //printf("Added Child: %d to: %s\n",currentTreeNode->childrenCount,currentTreeNode->token->text); 
                    struct TreeNode *createdNode = malloc( sizeof(struct TreeNode) ); 
                    createdNode->name == NULL;
                    createdNode->childrenCount =0;
                    createdNode = AddTokenToTreeNode(createdNode,current);

                    createdNode->content = NULL;
                    
                    currentTreeNode->children[currentTreeNode->childrenCount] = createdNode;
                    currentTreeNode->childrenCount++;
                    createdNode->parent = currentTreeNode;
                    
                    currentTreeNode = createdNode;
               }
               
            }
            else if(type == EndTag)
            {
                //printf("End Tag: %s\n",current->text); 
                currentTreeNode = currentTreeNode->parent;
            }else
            {
                currentTreeNode->content = text;
            }
            
        }

        current = current->next;
    }

    return treeRoot;
}

void treverseTree(struct TreeNode *current,  bool isLastChild, bool isArrayElement)
{
    if(current->childrenCount==0)
    {
        printf("\"%s\" : \"%s\"", current->name ,current->content); 

        if(!isLastChild)
        {
             printf(","); 
        }

        printf("\n"); 
        return;
    }

    printf("{"); 

    if(!isArrayElement)
    {
       printf("%s :", current->name);  
    }
    
    int i;
    bool childrenInArray = areChildrenInArray(current);

    if(childrenInArray)
    {
        printf("{%s :[ \n", current->children[0]->name); 
    }

    for(i=0;i<current->childrenCount;i++)
    {
        bool isThisLastChild = (i==(current->childrenCount-1));
        treverseTree(current->children[i], isThisLastChild, childrenInArray);
    }

    if(childrenInArray)
    {
        printf("]}\n"); 
    }

    printf("}"); 
    if(!isLastChild)
    {
         printf(","); 
    }
    printf("\n");    
}

char* buildJsonFromTree(struct TreeNode *current, char* currentString, bool isLastChild, bool isArrayElement)
{
    if(current->childrenCount==0)
    {
        currentString = appendToString(currentString,"\"\0");
        currentString = appendToString(currentString,current->name);
        currentString = appendToString(currentString,"\" : \"\0");
        currentString = appendToString(currentString,current->content); 
        currentString = appendToString(currentString,"\"\0");

        if(!isLastChild)
        { 
             currentString = appendToString(currentString,",\0");
        }

        currentString = appendToString(currentString,"\n\0"); 
        return;
    }

    currentString = appendToString(currentString,"{ \0");

    if(!isArrayElement)
    {
        currentString = appendToString(currentString,"\"\0");
        currentString = appendToString(currentString,current->name);
        currentString = appendToString(currentString,"\" :\0");
    }
    
    int i;
    bool childrenInArray = areChildrenInArray(current);

    if(childrenInArray)
    {
         currentString = appendToString(currentString,"{ \"\0");
         currentString = appendToString(currentString,current->children[0]->name);
         currentString = appendToString(currentString,"\" : [ \n\0");
    }

    for(i=0;i<current->childrenCount;i++)
    {
        bool isThisLastChild = (i==(current->childrenCount-1));
        currentString = buildJsonFromTree(current->children[i],currentString, isThisLastChild, childrenInArray);
    }

    if(childrenInArray)
    {
        currentString = appendToString(currentString,"]}\n\0");
    }

    currentString = appendToString(currentString,"}\0");

    if(!isLastChild)
    {
           currentString = appendToString(currentString,",\0");
    }
  
    return appendToString(currentString,"\n\0");
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

char* getTextFromToken(char* tokenText)
{
    char* newText = NULL;
    char current;
    int newTextLength=0;
    int count =0;
    do
    {
        current = (char)tokenText[count++];
        if(current!='<' && current!='>')
        {
            newText = realloc(newText, newTextLength+1);
            newText[newTextLength++] = current;
        }
        
    }while(current != '\0');

    return newText;
}

struct TreeNode *AddTokenToTreeNode(struct TreeNode *treeNode, struct Token *token)
{
    char* tokenText =  token->text;
    treeNode->name = NULL;
    
    char* newText = NULL;
    char current;
    char* key = NULL;
    char* value = NULL;
    int newTextLength=0;
    int count =0;
    bool isInLeteralString = false;
    do
    {
        current = (char)tokenText[count++];

        if(current=='"')
        {
            isInLeteralString =!isInLeteralString;
            continue;
        }

        if((current==' ' || current=='=' || current=='>') && !isInLeteralString)
        {
            newText = realloc(newText, newTextLength+1);
            newText[newTextLength++] = '\0';

            if(treeNode->name==NULL)
            {
                treeNode->name = newText;
            }else if(key==NULL)
            {

                key = newText;
            }else if(value==NULL)
            {
                value = newText;
            }
            
            newText = NULL;
            newTextLength=0;

            continue; // skip this char
        }

        if(current!='<' && current!='>')
        {
            newText = realloc(newText, newTextLength+1);
            newText[newTextLength++] = current;
        }

       if(key!=NULL && value!=NULL)
        {   
            // Add a new child node for the attribute.
            struct TreeNode *createdNode = malloc( sizeof(struct TreeNode) ); 
            treeNode->children[treeNode->childrenCount] = createdNode;
            treeNode->childrenCount++;
            createdNode->parent = treeNode; 
            createdNode->childrenCount =0;
            createdNode->name = key;
            createdNode->content = value;

            key = NULL;
            value = NULL;
        } 
        
    }while(current != '\0');

    // No attributes in this node.
    if(treeNode->name==NULL)
    {
     newText = realloc(newText, newTextLength+1);
     newText[newTextLength++] = '\0';
     treeNode->name = newText;
    }
     
    return treeNode;
}

bool areChildrenInArray(struct TreeNode *treeNode)
{
    if(treeNode==NULL)
    {
        return false;
    }

    if(treeNode->childrenCount==0)
    {
        return false;
    }

    int i=0;
    char* childName= NULL;

    for(i=0;i<treeNode->childrenCount;i++)
    {
       char* currentChildName = treeNode->children[i]->name;

       if(childName==NULL)
       {
        childName = currentChildName;
        continue;
       }

       if(strcmp(childName, currentChildName)!=0)
       {
            return false;
       }

    }

    return true;
}

char* appendToString(char* original, char* toAppend)
{
    int length =0;
    bool isOriginalNull =false;
    if(original==NULL)
    {
        length = strlen(toAppend)+1;
        isOriginalNull = true;
    }else
    {
        length = strlen(original)+strlen(toAppend)+1;
    }

    original = realloc(original,length);
    if(isOriginalNull)
    {
        original[0] = '\0';
    }
    strcat(original,toAppend);
    return original;
}
