#include <file-properties.h>

#include <sys/stat.h>
#include <dirent.h>
#include <openssl/evp.h>
#include <unistd.h>
#include <assert.h>
#include <string.h>
#include <defines.h>
#include <fcntl.h>
#include <stdio.h>
#include <utility.h>

/*!
 * @brief get_file_stats gets all of the required information for a file (inc. directories)
 * @param the files list entry
 * You must get:
 * - for files:
 *   - mode (permissions)
 *   - mtime (in nanoseconds)
 *   - size
 *   - entry type (FICHIER)
 *   - MD5 sum
 * - for directories:
 *   - mode
 *   - entry type (DOSSIER)
 * @return -1 in case of error, 0 else
 */
int get_file_stats(files_list_entry_t *entry) {
    const char *nom = entry->path_and_name;
    struct stat info;

    if (stat(entry->path_and_name, &info) == -1) {
        perror("Erreur lors de la récupération des informations sur le fichier");
        return -1;
    }

    if (S_ISREG(info.st_mode)) {
        // struct timespec mtime;
        entry->mtime.tv_sec = info.st_mtime;
        entry->mtime.tv_nsec = 0;

        // uint64_t size;
        entry->size = info.st_size;

        // uint8_t md5sum[16];
        compute_file_md5(entry);

        // file_type_t entry_type;
        entry->entry_type = FICHIER;

        // mode_t mode;
        mode_t file_permissions = info.st_mode;
        S_ISDIR(file_permissions);
        entry->mode= file_permissions;
        
        return 0;

    } else if (S_ISDIR(info.st_mode)) {
        // struct timespec mtime;
        entry->mtime.tv_sec = NULL;
        entry->mtime.tv_nsec = NULL;

        // uint64_t size;
        entry->size = NULL;

        // Suint8_t md5sum[16];
        strcpy(entry->md5sum, "");
        
        // file_type_t entry_type;
        entry->entry_type = DOSSIER;

        // mode_t mode;
        mode_t directory_permissions = info.st_mode;
        S_ISDIR(directory_permissions);  
        entry->mode = directory_permissions;

        return 0;
    }
    return -1;

}

/*!
 * @brief compute_file_md5 computes a file's MD5 sum
 * @param the pointer to the files list entry
 * @return -1 in case of error, 0 else
 * Use libcrypto functions from openssl/evp.h
 */
int compute_file_md5(files_list_entry_t *entry) {
    FILE *file = fopen(entry->path_and_name, "rb");
    if (!file) {
        perror("Error opening file");
    }
    
    OpenSSL_add_all_algorithms();

    EVP_MD_CTX *mdctx = EVP_MD_CTX_new();
    EVP_DigestInit_ex(mdctx, EVP_md5(), NULL);

    unsigned char buffer[4096];
    size_t bytesRead;

    while ((bytesRead = fread(buffer, 1, sizeof(buffer), file)) > 0) {
        EVP_DigestUpdate(mdctx, buffer, bytesRead);
    }

    fclose(file);

    unsigned int md_len;
    EVP_DigestFinal_ex(mdctx, entry->md5sum, &md_len);
    EVP_MD_CTX_free(mdctx);
    /*
    printf("Le MD5(%s) = ", entry->path_and_name);
    for (unsigned int i = 0; i < md_len; i++) {
        printf("%02x", entry->md5sum[i]);
    }
    printf("\n");
    */

    EVP_cleanup();
    
    return 0;
}

/*!
 * @brief directory_exists tests the existence of a directory
 * @path_to_dir a string with the path to the directory
 * @return true if directory exists, false else
 */
bool directory_exists(char *path_to_dir) {
    struct stat dir_stat;
    if (stat(path_to_dir, &dir_stat) == 0 && S_ISDIR(dir_stat.st_mode)) {
        return true;
    } else {
        return false;
    }
}


/*!
 * @brief is_directory_writable tests if a directory is writable
 * @param path_to_dir the path to the directory to test
 * @return true if dir is writable, false else
 * Hint: try to open a file in write mode in the target directory.
 */
bool is_directory_writable(char *path_to_dir) {
    if (access(path_to_dir, W_OK) == -1){
        return false;
    } else {
        return true;
    }
}
