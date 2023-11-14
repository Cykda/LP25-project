#include <dirent.h>
#include "list_files.h"




void list_files(char* basepath)
{
    char path[1000];

    struct dirent *dp;


    DIR* dir = opendir(basepath);
    long size = 0;

    if(!dir)
    {
        return;
    }


    while((dp = readdir(dir)) != NULL)
    {
        if(strcmp(dp->d_name, ".") != 0 && strcmp(dp->d_name, "..") != 0)
        {
            strcpy(path, basepath);
            strcat(path, "/");
            strcat(path, dp->d_name);
            
            if((int)dp->d_type == 8)
            {
                printf("%s\n", path);
            }
            else
            {
                
                list_files(path);
            }
            
        }
    }

    closedir(dir);

}


