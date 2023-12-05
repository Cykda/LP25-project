#include <stdio.h>
#include <stdlib.h>
#include "flist.h"


void clear_flt(files_list_t *flt) // files_list_t *list
{
    printf("CLEAR START\n");
    
    if(flt->head == NULL)
    {
        printf("NULL HEAD\n");
        return;
        
    }
    while (flt->head) {
        files_list_entry_t *tmp = flt->head;
        flt->head = tmp->next;
        free(tmp);
    }
    
}
int main(int argc, char** argv)
{
    
    printf("START\n");
    files_list_t *flt = malloc(sizeof(file_type_t));
    flt->head = malloc(sizeof(files_list_entry_t));
    flt->tail = NULL;
    
    
    
    
    if(flt == NULL) {return EXIT_FAILURE;}
    
    printf("flt variable allocated successfully\n");
    
    
    add_file_entry(flt, "C:/");
    add_file_entry(flt, "C:/Windows");
    add_file_entry(flt, "C:/Windows/Users");

    
    
    display_files_list(flt);
    clear_files_list(flt);
    return 0;
}