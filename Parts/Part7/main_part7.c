#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <string.h>
#include <string.h>
#include <ctype.h>
#include <getopt.h>
#include <stdbool.h>
#include <openssl/md5.h>
#include <openssl/evp.h>

#define MD5_DIGEST_LENGTH 16

//************************Je ne sais pas si cette fonction a un emplacement déja défini sur le squelette****************************
int compare_md5(const char *file1, const char *file2) {
    FILE *fp1, *fp2;
    unsigned char md5_1[MD5_DIGEST_LENGTH], md5_2[MD5_DIGEST_LENGTH];

    // Ouvrir les fichiers en mode binaire
    fp1 = fopen(file1, "rb");
    fp2 = fopen(file2, "rb");

    if (fp1 == NULL || fp2 == NULL) {
        perror("Erreur lors de l'ouverture des fichiers");
        exit(EXIT_FAILURE);
    }

    // Initialiser le contexte MD5
    EVP_MD_CTX *mdctx1, *mdctx2;
    mdctx1 = EVP_MD_CTX_new();
    mdctx2 = EVP_MD_CTX_new();

    EVP_DigestInit_ex(mdctx1, EVP_md5(), NULL);
    EVP_DigestInit_ex(mdctx2, EVP_md5(), NULL);

    // Calculer la somme MD5 du premier fichier
    unsigned char buffer1[1024];
    size_t bytesRead1;
    while ((bytesRead1 = fread(buffer1, 1, sizeof(buffer1), fp1)) != 0) {
        EVP_DigestUpdate(mdctx1, buffer1, bytesRead1);
    }
    EVP_DigestFinal_ex(mdctx1, md5_1, NULL);

    // Réinitialiser le contexte MD5 pour le second fichier
    EVP_DigestInit_ex(mdctx2, EVP_md5(), NULL);

    // Calculer la somme MD5 du deuxième fichier
    unsigned char buffer2[1024];
    size_t bytesRead2;
    while ((bytesRead2 = fread(buffer2, 1, sizeof(buffer2), fp2)) != 0) {
        EVP_DigestUpdate(mdctx2, buffer2, bytesRead2);
    }
    EVP_DigestFinal_ex(mdctx2, md5_2, NULL);

    // Fermer les fichiers
    fclose(fp1);
    fclose(fp2);

    // Comparer les sommes MD5
    if (memcmp(md5_1, md5_2, MD5_DIGEST_LENGTH) == 0) {
        printf("Les fichiers sont identiques.\n");
        return 1; // Les fichiers sont identiques
    } else {
        printf("Les fichiers sont différents.\n");
        return 0; // Les fichiers sont différents
    }
}


int main() {
    const char *file1 = "File1.txt";
    const char *file2 = "DT/File1.txt";

    if (compare_md5(file1, file2)) {
        printf("Les fichiers ont la même somme MD5.\n");
    } else {
        printf("Les fichiers n'ont pas la même somme MD5.\n");
    }

    return 0;
}