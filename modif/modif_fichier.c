#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <time.h>
#include <utime.h>
#include <fcntl.h>
#include <openssl/evp.h>

#define MAX_BUF_SIZE 1024

int calculate_md5(const char *filename, unsigned char *md5sum) {
    FILE *file = fopen(filename, "rb");
    if (!file) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

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

int afficher_information(const char *nom) {
    struct stat info;

    if (stat(nom, &info) == -1) {
        perror("Erreur lors de la récupération des informations sur le fichier");
        exit(EXIT_FAILURE);
    }

    if (S_ISREG(info.st_mode)) {
        printf("%s il s'agit d'un fichier.\n", nom);
        printf("Date de la dernière modification du fichier %s : %s", nom, ctime(&info.st_ctime));

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
        printf("\n");
        return 0;

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
        return 0;  
    } else {
        printf("Mais tu es con ou ?");
        return -1;
    }
}

// void changedatemodif(const char *filename, time_t new_mtime) {
//     struct utimbuf new_times;
//     new_times.actime = new_mtime;  // Aucun changement pour atime (temps d'accès)
//     new_times.modtime = new_mtime; // Définir le nouveau mtime

//     if (utime(filename, &new_times) == -1) {
//         perror("Error changing file modification time");
//         exit(EXIT_FAILURE);
//     }

//     printf("File modification time of %s changed to %ld\n", filename, new_mtime);
// }

void changepermissions(const char *filename, const char *permissions_str) {
    mode_t permissions = (mode_t)strtol(permissions_str, NULL, 8);

    if (chmod(filename, permissions) == -1) {
        perror("Error changing file permissions");
        exit(EXIT_FAILURE);
    }

    printf("File permissions of %s changed to %s\n", filename, permissions_str);
}

int main() {
    const char *nom = "test.c";
    afficher_information(nom);
    changepermissions(nom, "777");

    printf("\n\n");
    afficher_information(nom);
    
    struct utimbuf new_times;

    struct tm new_date = {0};
    new_date.tm_year = 2025 - 1900;
    new_date.tm_mon = 0;
    new_date.tm_mday = 1;
    new_date.tm_hour = 12;
    new_date.tm_min = 0;
    new_date.tm_sec = 0;


    struct tm new_date_2 = {0};
    new_date_2.tm_year = 2025 - 1900;
    new_date_2.tm_mon = 0;
    new_date_2.tm_mday = 1;
    new_date_2.tm_hour = 12;
    new_date_2.tm_min = 0;
    new_date_2.tm_sec = 0;
    
    new_times.actime = mktime(&new_date);
    new_times.modtime = mktime(&new_date);

    if (utime(nom, &new_times) == 0) {
        printf("La date du fichier a été modifiée avec succès.\n");
    } else {
        perror("Erreur lors de la modification de la date du fichier");
        return 1;
    }

    new_times.actime = time(NULL);
    new_times.modtime = time(NULL);

    printf("\n\n");
    afficher_information(nom);

    changepermissions(nom, "444");

    printf("\n\n");
    afficher_information(nom);

    return 0;
}