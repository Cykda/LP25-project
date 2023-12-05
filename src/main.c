#include <stdio.h>
#include <stdlib.h>
#include "flist.h"



int main(int argc, char** argv)
{
    
    printf("START\n");
    files_list_t *flt = malloc(sizeof(*flt));
    flt->head = malloc(sizeof(files_list_entry_t));
    flt->tail = NULL;
    
    
    
    
    if(!flt) {return EXIT_FAILURE;}
    
    printf("flt variable allocated successfully\n");
    
    
    add_file_entry(flt, "C:/");
    add_file_entry(flt, "C:/Windows");
    add_file_entry(flt, "C:/Windows/Users");

    
    
    display_files_list(flt);
    clear_files_list(flt);
    return EXIT_SUCCESS;
}