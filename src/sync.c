#include <sync.h>
#include <dirent.h>
#include <string.h>
#include <processes.h>
#include <utility.h>
#include <messages.h>
#include <file-properties.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/sendfile.h>
#include <unistd.h>
#include <sys/msg.h>
#include <utime.h>
#include "errors.h"
#include <stdio.h>
#include <errno.h>
#include "colors.h"
#include "errors.h"
// #0  0x000055823508a0e3 in synchronize ()

/*!
 * @brief synchronize is the main function for synchronization
 * It will build the lists (source and destination), then make a third list with differences, and apply differences to the destination
 * It must adapt to the parallel or not operation of the program.
 * @param the_config is a pointer to the configuration
 * @param p_context is a pointer to the processes context
 */
void synchronize(configuration_t *the_config, process_context_t *p_context) {
    /*L'arboréscence du dossier*/
    files_list_t *list_source = malloc(sizeof(file_type_t));
    list_source->head = NULL;
    list_source->tail = NULL;

    make_files_list(list_source, the_config->source);

    files_list_t *list_destination = malloc(sizeof(file_type_t));
    list_destination->head = NULL;
    list_destination->tail = NULL;

    make_files_list(list_destination, the_config->destination );

    files_list_entry_t *source = list_source->head;
    files_list_entry_t *destination = list_destination->head;
    /*Pour chaque élément de la liste chainé*/
    /*Vérifier la synchronization*/
    files_list_entry_t* differences = malloc(sizeof(differences));
    
    while (source != NULL) {
        /*Copier*/
        copy_entry_to_destination(source, the_config);
        
        /*Vérifier la synchronization*/
        
        

        //strcpy(differences->path_and_name, "");
        if (mismatch(source, destination, true)) {
            
            if (mismatch(source, destination, false)) {
                strcpy(differences->path_and_name, source->path_and_name);
                memcpy(destination->md5sum, source->md5sum, sizeof(uint8_t) * 16);
            }
            else {
                
                memset(destination->md5sum, 0, sizeof(destination->md5sum));
                strcpy(differences->path_and_name, "");
            }
        } else {
            
            
            memset(destination->md5sum, 0, sizeof(destination->md5sum));
            strcpy(differences->path_and_name, "");
        }
        if (source->size != destination->size) {
            differences->size = source->size;
        }
        else {

            differences->size = NULL;
        }

        if (source->entry_type != destination->entry_type) {
            differences->entry_type = source->entry_type;
        }
        else {
            differences->entry_type = source->entry_type; 
        }

        if (source->mtime.tv_sec != destination->mtime.tv_sec) {
            differences->mtime.tv_sec = source->mtime.tv_sec;
        }
        else {
            differences->mtime.tv_sec = NULL;
        }

        if (source->mtime.tv_nsec != destination->mtime.tv_nsec) {
            differences->mtime.tv_nsec = source->mtime.tv_nsec;
        }
        else {
            differences->mtime.tv_nsec = NULL;
        }

        if (source->mode != destination->mode) {
            differences->mode = source->mode;
        }
        else {
            differences->mode = NULL;
        }
        
        if (differences->entry_type != NULL) {
            copy_entry_to_destination(&source, the_config);
        }

        if (the_config->uses_md5) {
            while (is_md5sum_empty(differences->md5sum)) {
                copy_entry_to_destination(&source, the_config);
            }
        }
        
        if (strcmp(differences->path_and_name, "")) {
            const char *name1 = strrchr(source->path_and_name, '/');

            if (name1 == NULL) {
                name1 = source->path_and_name;
            } else {
                name1++;
            }
            rename(destination->path_and_name, name1);
        }

        if (differences->mtime.tv_sec != NULL || differences->mtime.tv_nsec != NULL) {
            struct utimbuf newTimes;
            newTimes.modtime = destination->mtime.tv_sec;
            utime(destination->path_and_name, &newTimes);
        }


        if (differences->size != NULL) {
            truncate(destination->path_and_name, destination->size);
        }

        if (differences->mode != NULL) {
            chmod(destination->path_and_name, differences->mode);
        }
        
    }

    free(differences);
    clear_files_list(list_source);
    free(list_source);
    clear_files_list(list_destination);
    free(list_destination);
}


bool is_md5sum_empty(const uint8_t *md5sum) {
    for (size_t i = 0; i <= 16; i++) {
        if (md5sum[i] != 0) {
            return false;  // Non-zero element found
        }
    }
    return true;  // All elements are zero
}

/*!
 * @brief mismatch tests if two files with the same name (one in source, one in destination) are equal
 * @param lhd a files list entry from the source
 * @param rhd a files list entry from the destination
 * @has_md5 a value to enable or disable MD5 sum check
 * @return true if both files are not equal, false else
 */
bool mismatch(files_list_entry_t *lhd, files_list_entry_t *rhd, bool has_md5) {

    char *name1 = strrchr(lhd->path_and_name, '/');
    char *name2 = strrchr(rhd->path_and_name, '/');


    if (name1 == NULL) {
        name1 = lhd->path_and_name;
    } else {
        name1++;
    }

    if (name2 == NULL) {
        name2 = rhd->path_and_name;
    } else {
        name2++;
    }

    if (has_md5) {
        
        if (memcmp(lhd->md5sum, rhd->md5sum, sizeof(uint8_t) * 16) == 0) {
            return strcmp(name1, name2) != 0;
        }
        
        else {
            return true;
        }

    }
    
    return strcmp(name1, name2) != 0;
}


/*!
 * @brief make_files_list buils a files list in no parallel mode
 * @param list is a pointer to the list that will be built
 * @param target_path is the path whose files to list
 */
void make_files_list(files_list_t *list, char *target_path) {
    DIR *dir;
    struct dirent *entry;
    if(list == NULL || target_path == NULL)
    {
        printf("PROBLEME\n");
        return; 
    }

    // Open the directory
    dir = opendir(target_path);
    if (dir == NULL) {
        perror("Unable to open directory");
        return;
    }

    // Traverse the directory entries
    while ((entry = readdir(dir)) != NULL) {
        // Skip "." and ".." entries
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        // Create a new entry for the file or directory
        files_list_entry_t *new_entry = (files_list_entry_t *)malloc(sizeof(files_list_entry_t));
        if (new_entry == NULL) {
            perror("Unable to allocate memory");
            return;
        }

        // Set the path and name of the file or directory
        snprintf(new_entry->path_and_name, sizeof(new_entry->path_and_name), "%s/%s", target_path, entry->d_name);

        // Check if it is a directory
        struct stat entry_stat;
        if (stat(new_entry->path_and_name, &entry_stat) == 0 && S_ISDIR(entry_stat.st_mode)) {
            new_entry->entry_type = DOSSIER;
            // Recursively call make_files_list for the subdirectory
            make_files_list(list, new_entry->path_and_name);
        } else {
            new_entry->entry_type = FICHIER;
        }

        // Add the new entry to the list
        new_entry->next = list->head;
        list->head = new_entry;
    }

    // Close the directory
    closedir(dir);
}

/*!
 * @brief make_files_lists_parallel makes both (src and dest) files list with parallel processing
 * @param src_list is a pointer to the source list to build
 * @param dst_list is a pointer to the destination list to build
 * @param the_config is a pointer to the program configuration
 * @param msg_queue is the id of the MQ used for communication
 */
void make_files_lists_parallel(files_list_t *src_list, files_list_t *dst_list, configuration_t *the_config, int msg_queue) {
}

/*!
 * @brief copy_entry_to_destination copies a file from the source to the destination
 * It keeps access modes and mtime (@see utimensat)
 * Pay attention to the path so that the prefixes are not repeated from the source to the destination
 * Use sendfile to copy the file, mkdir to create the directory
 */
void copy_entry_to_destination(files_list_entry_t *source_entry, configuration_t *the_config) {
    char destination_path[PATH_SIZE];
    snprintf(destination_path, sizeof(destination_path), "%s/%s", the_config->destination, source_entry->path_and_name);
    //printf("1\n");
    int read_fd;
    int write_fd;
    struct stat stat_buf;
    off_t offset = 0;
    if(source_entry == NULL || the_config == NULL)
    {
        printf("PROBLEME\n");
        return; 
    }
    
    


    if (source_entry->entry_type == DOSSIER) {
        printf("Trying to copy folder %s to %s\n", source_entry->path_and_name, destination_path);
        
        // Create the destination directory
        if (mkdir(destination_path, 0777) == -1 && errno != EEXIST) {
            throwError(WARNING, "Unable to create destination directory\n\n");
        } else {
            CFprintf(FG_COLOR_DGREEN, "Directory sucessfully created !\n\n");
        }

        // Recursively copy the next entry
        if (source_entry->next != NULL) {
            copy_entry_to_destination(source_entry->next, the_config);
        }
    } else {
        printf("Tying to copy file from %s to %s\n", source_entry->path_and_name, destination_path);

        // Create the path to the destination file
        char* last_slash = strrchr(destination_path, '/');
        if (last_slash != NULL) {
            *last_slash = '\0'; // Remove the file name from the path
            if (mkdir(destination_path, 0777) == -1 && errno != EEXIST) {
                //perror("Unable to create destination directory");
                throwError(WARNING, "Unable to create destination directory");
                *last_slash = '/'; // Restore the file name in the path
                return;
            }
            *last_slash = '/'; // Restore the file name in the path
        }

        read_fd = open(source_entry->path_and_name, O_RDONLY);
        if (read_fd == -1) {
            //perror("Unable to open source file");
            throwError(ERROR, "Unable to open source file");
            return;
        }

        fstat(read_fd, &stat_buf);

        write_fd = open(destination_path, O_WRONLY | O_CREAT | O_TRUNC, stat_buf.st_mode);
        if (write_fd == -1) {
            //perror("Unable to open destination file");
            throwError(ERROR, "Unable to open destination file");
            close(read_fd);
            return;
        }

        sendfile(write_fd, read_fd, &offset, stat_buf.st_size);
        CFprintf(FG_COLOR_DGREEN, "File copied successfully !\n\n");
        close(read_fd);
        close(write_fd);

        // Recursively copy the next entry
        if (source_entry->next != NULL) {
            copy_entry_to_destination(source_entry->next, the_config);
        }
    }
}

/*!
 * @brief make_list lists files in a location (it recurses in directories)
 * It doesn't get files properties, only a list of paths
 * This function is used by make_files_list and make_files_list_parallel
 * @param list is a pointer to the list that will be built
 * @param target is the target dir whose content must be listed
 */
void make_list(files_list_t *list, char *target) {
    DIR *dir;
    struct stat path_stat;
    if (!(dir = open_dir(target))){return;}
    
    struct dirent *entry = get_next_entry(dir);
    while (entry != NULL){
        char path[PATH_SIZE];
        char truncated_path[PATH_SIZE];
        
        concat_path(path, target, entry->d_name);
        stat(path, &path_stat);
        if(S_ISREG(path_stat.st_mode) == 0)
        {
            make_list(list, path);
        }
        add_file_entry(list, path);
        entry = get_next_entry(dir);
    } 
    closedir(dir);
    return;

}

/*!
 * @brief open_dir opens a dir
 * @param path is the path to the dir
 * @return a pointer to a dir, NULL if it cannot be opened
 */
DIR *open_dir(char *path) {
    DIR *d;
    d = opendir(path);
    return d;
}

/*!
 * @brief get_next_entry returns the next entry in an already opened dir
 * @param dir is a pointer to the dir (as a result of opendir, @see open_dir)
 * @return a struct dirent pointer to the next relevant entry, NULL if none found (use it to stop iterating)
 * Relevant entries are all regular files and dir, except . and ..
 */
struct dirent *get_next_entry(DIR *dir) {
    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
            if (entry->d_type == 4 || entry->d_type == 8) {
                return entry;
            }
            //printf("No relevent entry found");
        }
    }
    return NULL; // No relevant entry found
}