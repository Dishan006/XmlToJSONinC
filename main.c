/* 
 * File:   main.c
 * Author: dishanm
 *
 * Created on October 29, 2015, 10:57 AM
 */

#include <stdio.h>
#include <stdlib.h>
#include "xmlToJson.h"

FILE* getFile();
long getLength(FILE* file);

int main(int argc, char** argv) {

    FILE* file = getFile();
    
    long size = getLength(file);
    printf ("Size of file: %ld bytes.\n",size);
    
    char* json = convertToJson(file);
    printf("%s\n", json);
    return (EXIT_SUCCESS);
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
