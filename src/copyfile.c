#include "copyfile.h"



int copyfile(char* src, char* dest)
{
    FILE *fptr1, *fptr2; 
    char  c; 

    // Open one file for reading 
    fptr1 = fopen(src, "r"); 
    if (fptr1 == NULL) 
    { 
        printf("Cannot open file\n"); 
        return -1; 
    } 
  

    // Open another file for writing 
    fptr2 = fopen(dest, "w"); 
    if (fptr2 == NULL) 
    { 
        printf("Cannot open file\n"); 
        return -1;
    } 
  
    // Read contents from file 
    c = fgetc(fptr1); 
    while (c != EOF) 
    { 
        fputc(c, fptr2); 
        c = fgetc(fptr1); 
    } 
  

  
    fclose(fptr1); 
    fclose(fptr2); 
    return 0;


}