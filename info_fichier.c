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
    const char *nom_fichier = "test.txt"; // Remplacez cela par le chemin de votre fichier

    struct stat info_fichier;

    // Obtenez les informations sur le fichier
    if (stat(nom_fichier, &info_fichier) == -1) {
        perror("Erreur lors de la récupération des informations sur le fichier");
        exit(EXIT_FAILURE);
    }

    // Affichez la date de création du fichier
    printf("Date de création du fichier %s : %s", nom_fichier, ctime(&info_fichier.st_ctime));

    const char *filename = "test.txt"; // Remplacez cela par le chemin de votre fichier
    unsigned char digest[EVP_MAX_MD_SIZE];

    calculate_md5(nom_fichier, digest);

    return 0;
}