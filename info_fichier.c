#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <openssl/evp.h>

int main() {
    const char *nom_fichier = "test.txt"; // Remplacez cela par le chemin de votre fichier

    struct stat info_fichier;

    // Obtenez les informations sur le fichier
    if (stat(nom_fichier, &info_fichier) == -1) {
        perror("Erreur lors de la récupération des informations sur le fichier");
        exit(EXIT_FAILURE);


    // Affichez la date de création du fichier
    printf("Date de création du fichier %s : %s", nom_fichier, ctime(&info_fichier.st_ctime));
    printf("MD5 Adresse du fichier %s : %s", nom_fichier);

    unsigned char digest[16];
    compute_md5(nom_fichier, digest);
    for (int i=0; i < 16; i++) {
        printf("%02x ", digest[i]);
    }
    putchar('\n');
    return 0;
}