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

/*!
 * @brief synchronize is the main function for synchronization
 * It will build the lists (source and destination), then make a third list with differences, and apply differences to the destination
 * It must adapt to the parallel or not operation of the program.
 * @param the_config is a pointer to the configuration
 * @param p_context is a pointer to the processes context
 */
void synchronize(configuration_t *the_config, process_context_t *p_context) {
    files_list_entry_t source;
    strcpy(source.path_and_name, the_config->source);
    printf("A\n");
    get_file_stats(&source);

    files_list_entry_t destination;
    strcpy(source.path_and_name, the_config->destination);
    printf("B\n");
    get_file_stats(&destination);
    printf("C\n");

    files_list_entry_t differences;
    strcpy(differences.path_and_name, "");

    if (mismatch(&source, &destination, true)) {
        if (mismatch(&source, &destination, false)) {
            strcpy(differences.path_and_name, source.path_and_name);
            memcpy(destination.md5sum, source.md5sum, sizeof(uint8_t) * 16);
        }
        else {
            memset(destination.md5sum, 0, sizeof(destination.md5sum));
        }
    } else {

    }

    if (source.size != destination.size) {
        differences.size = source.size;
    }
    else {
        differences.size = NULL;
    }

    if (source.entry_type != destination.entry_type) {
        differences.entry_type = source.entry_type;
    }
    else {
        differences.entry_type = source.entry_type; /*erreur*/
    }

    if (source.mtime.tv_sec != destination.mtime.tv_sec) {
        differences.mtime.tv_sec = source.mtime.tv_sec;
    }
    else {
        differences.mtime.tv_sec = NULL;
    }

    if (source.mtime.tv_nsec != destination.mtime.tv_nsec) {
        differences.mtime.tv_nsec = source.mtime.tv_nsec;
    }
    else {
        differences.mtime.tv_nsec = NULL;
    }

    if (source.mode != destination.mode) {
        differences.mode = source.mode;
    }
    else {
        differences.mode = NULL;
    }

    if (differences.entry_type != NULL) {
        copy_entry_to_destination(&source, the_config);
    }

    if (the_config->uses_md5) {
        while (is_md5sum_empty(differences.md5sum)) {
            copy_entry_to_destination(&source, the_config);
        }
    }

    if (strcmp(differences.path_and_name, "")) {
        const char *name1 = strrchr(source.path_and_name, '/');

        if (name1 == NULL) {
            name1 = source.path_and_name;
        } else {
            name1++;
        }
        rename(destination.path_and_name, name1);
    }

    if (differences.mtime.tv_sec != NULL || differences.mtime.tv_nsec != NULL) {
        struct utimbuf newTimes; /*error*/
        newTimes.modtime = destination.mtime.tv_sec;
        utime(destination.path_and_name, &newTimes);
    }


    if (differences.size != NULL) {
        truncate(destination.path_and_name, destination.size);
    }

    if (differences.mode != NULL) {
        chmod(destination.path_and_name, differences.mode);
    }
}

bool is_md5sum_empty(const uint8_t *md5sum) { /*error*/
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
    const char *name1 = strrchr(lhd->path_and_name, '/');
    const char *name2 = strrchr(rhd->path_and_name, '/');

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
    DIR *d;
    struct dirent *dir;
    d = opendir(target_path);
    char path[PATH_SIZE];
    struct stat path_stat;
    
    if (d)
    {
        while ((dir = readdir(d)) != NULL)
        {
            if(strcmp(dir->d_name, ".") && strcmp(dir->d_name, "..")) // exclude . and .. or we would be stuck in an infinite loop
            {


                strcpy(path, target_path);
                strcat(path, "/");
                strcat(path, dir->d_name);

              
                add_file_entry(list, path);

                stat(path, &path_stat);
                
                if(S_ISREG(path_stat.st_mode) == 0)
                {
                    make_files_list(list, path);
                }
            }
        }
        closedir(d);
    }
    for(files_list_entry_t *fle = list->head; fle != NULL; fle = fle->next)
    {
        get_file_stats(fle);
        fprintf(stdout, "%s\n", fle->path_and_name);
    }


    

    
    
    
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

    int read_fd;
    int write_fd;
    struct stat stat_buf;
    off_t offset = 0;

    read_fd = open (the_config->source, O_RDONLY);
    fstat (read_fd, &stat_buf);

    write_fd = open (the_config->destination, O_WRONLY | O_CREAT, stat_buf.st_mode);

    sendfile (write_fd, read_fd, &offset, stat_buf.st_size);

    close (read_fd);
    close (write_fd);



}

/*!
 * @brief make_list lists files in a location (it recurses in directories)
 * It doesn't get files properties, only a list of paths
 * This function is used by make_files_list and make_files_list_parallel
 * @param list is a pointer to the list that will be built
 * @param target is the target dir whose content must be listed
 */
void make_list(files_list_t *list, char *target) {


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
}
