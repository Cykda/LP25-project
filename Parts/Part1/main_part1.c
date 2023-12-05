#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <string.h>
#include <string.h>
#include <ctype.h>
#include <getopt.h>
#include <stdbool.h>

void print_usage() {
    printf("Usage: lp25-backup <dossier_source> <dossier_sortie> [options]\n");
    printf("Options:\n");
    printf("  --date-size-only    Utiliser la date et la taille uniquement\n");
    printf("  -n <nombre>          Spécifier le nombre (ex: -n 5)\n");
    printf("  --no-parallel       Désactiver le traitement parallèle\n");
    printf("  -v                  Afficher les informations détaillées\n");
    printf("  --dry-run           Effectuer une simulation sans effectuer la copie\n");
}

//************************FONCTION A AJOUTER AU FICHIER "file_properties.c"****************************
bool directory_exists(const char *path_to_dir) {
    struct stat dir_stat;
    if (stat(path_to_dir, &dir_stat) == 0 && S_ISDIR(dir_stat.st_mode)) {
        return true;
    }
    return false;
}



int main(int argc, char *argv[]) {
    // Variables pour les options
    int date_size_only = 0;
    int num = -1;
    int no_parallel = 0;
    int verbose = 0;
    int dry_run = 0;

    // Vérifier le nombre d'arguments
    if (argc < 3) {
        printf("Erreur: Nombre d'arguments insuffisant.\n");
    }

    int opt;
    struct option long_options[] = {
        {"date-size-only", no_argument, 0, 'd'},
        {"no-parallel", no_argument, 0, 'p'},
        {"dry-run", no_argument, 0, 'r'},
        {0, 0, 0, 0}
    };

    //Obtention des informations sur les paramètres demandés
    while ((opt = getopt_long(argc, argv, "n:v", long_options, NULL)) != -1) {
        switch (opt) {
            case 'n':
                num = atoi(optarg);
                break;
            case 'v':
                verbose = 1;
                break;
            case 'd':
                date_size_only = 1;
                break;
            case 'p':
                no_parallel = 1;
                break;
            case 'r':
                dry_run = 1;
                break;
            case '?':
                if (optopt == 'n') {
                    fprintf(stderr, "Erreur: L'option -%c nécessite un argument.\n", optopt);
                } else if (isprint(optopt)) {
                    fprintf(stderr, "Erreur: Option inconnue -%c.\n", optopt);
                } else {
                    fprintf(stderr, "Erreur: Caractère d'option inconnu '\\x%x'.\n", optopt);
                }
                print_usage();
                exit(EXIT_FAILURE);
        }
    }

    // Dossiers source et destination
    char *source = argv[argc-2];
    char *destination = argv[argc-1];
    // Vérifier si le dossier source existe
    if (!directory_exists(source)) {
        printf("Erreur: Le dossier source n'existe pas.\n");
        exit(EXIT_FAILURE);
    }

    // Vérifier si le dossier destination existe
    //A RAJOUTER : IL N'EST PAS DANS L'ARBORESCENCE DE FICHIERS DESTINATION
    if (!directory_exists(destination)) {
        printf("Erreur: Le dossier destination n'existe pas.\n");
        exit(EXIT_FAILURE);
    } else if (access(destination, W_OK) == -1) {
        printf("Erreur: Le dossier destination existe mais n'est pas accessible en écriture.\n");
        exit(EXIT_FAILURE);
    }

    //afficher les fichiers source et destination s'ils sont conforme aux normes demandées
    printf("Dossier source: %s et dossier destination : %s\n",source,destination);

    // Afficher les options détectées
    if (date_size_only) printf("Option détectée: --date-size-only\n");
    if (num != -1) printf("Option détectée: -n %d\n", num);
    if (no_parallel) printf("Option détectée: --no-parallel\n");
    if (verbose) printf("Option détectée: -v\n");
    if (dry_run) printf("Option détectée: --dry-run\n");
    
    return 0;
}
