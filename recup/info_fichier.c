#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <openssl/evp.h>

#define MAX_BUF_SIZE 1024

int calculate_md5(const char *filename, unsigned char *md5sum) {
    FILE *file = fopen(filename, "rb");
    if (!file) exit(EXIT_FAILURE);;

    OpenSSL_add_all_algorithms();

    EVP_MD_CTX *mdctx = EVP_MD_CTX_new();
    EVP_DigestInit_ex(mdctx, EVP_md5(), NULL);

    unsigned char buffer[MAX_BUF_SIZE];
    size_t bytesRead;

    while ((bytesRead = fread(buffer, 1, sizeof(buffer), file)) > 0) {
        EVP_DigestUpdate(mdctx, buffer, bytesRead);
    }

    fclose(file);

    unsigned int md_len;

    EVP_DigestFinal_ex(mdctx, md5sum, &md_len);
    EVP_MD_CTX_free(mdctx);

    printf("Le MD5(%s) = ", filename);
    for (unsigned int i = 0; i < md_len; i++) {
        printf("%02x", md5sum[i]);
    }
    printf("\n");

    EVP_cleanup();

    return 0;
}

int main() {
    const char *nom = "Readme.md"; 

    struct stat info;

    if (stat(nom, &info) == -1) {
        perror("Erreur lors de la récupération des informations sur le fichier");
        exit(EXIT_FAILURE);
    }

    if (S_ISREG(info.st_mode)) {
        printf("%s il s'agit d'un fichier.\n", nom);
        printf("Date du dernier accès du fichier %s : %s", nom, ctime(&info.st_atime));
        printf("Date de la dernière modification du fichier %s : %s", nom, ctime(&info.st_mtime));
        printf("Date du dernier changement d'état du fichier %s : %s", nom, ctime(&info.st_ctime));
        #ifdef st_birthtime
            printf("Date de création du fichier %s : %s", nom, ctime(&info.st_birthtime));
        #else
            printf("Date de création : Non disponible\n");
        #endif

        printf("Taille du fichier %s: %ld bytes\n", nom, info.st_size);

        unsigned char digest[EVP_MAX_MD_SIZE];

        calculate_md5(nom, digest);

        mode_t file_permissions = info.st_mode;
        
        printf("Les permitions de %s sont : ", nom);

        S_ISDIR(file_permissions);
        printf((file_permissions & S_IRUSR) ? "r" : "-");
        printf((file_permissions & S_IWUSR) ? "w" : "-");
        printf((file_permissions & S_IXUSR) ? "x" : "-");
        printf((file_permissions & S_IRGRP) ? "r" : "-");
        printf((file_permissions & S_IWGRP) ? "w" : "-");
        printf((file_permissions & S_IXGRP) ? "x" : "-");
        printf((file_permissions & S_IROTH) ? "r" : "-");
        printf((file_permissions & S_IWOTH) ? "w" : "-");
        printf((file_permissions & S_IXOTH) ? "x" : "-");

    } else if (S_ISDIR(info.st_mode)) {
        printf("%s il ne s'agit d'un dossier.\n", nom);

        mode_t directory_permissions = info.st_mode;
        
        printf("Les permitions de %s sont : ", nom);

        S_ISDIR(directory_permissions);
        printf((directory_permissions & S_IRUSR) ? "r" : "-");
        printf((directory_permissions & S_IWUSR) ? "w" : "-");
        printf((directory_permissions & S_IXUSR) ? "x" : "-");
        printf((directory_permissions & S_IRGRP) ? "r" : "-");
        printf((directory_permissions & S_IWGRP) ? "w" : "-");
        printf((directory_permissions & S_IXGRP) ? "x" : "-");
        printf((directory_permissions & S_IROTH) ? "r" : "-");
        printf((directory_permissions & S_IWOTH) ? "w" : "-");
        printf((directory_permissions & S_IXOTH) ? "x" : "-");        
    } else {
        printf("Mais tu es con ou ?");
    }
    printf("\n");

    return 0;
}