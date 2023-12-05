#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <string.h>
#include <string.h>
#include <ctype.h>
#include <getopt.h>
#include <stdbool.h>
#include <stdint.h>


typedef struct {
    char source[1024];
    char destination[1024];
    uint8_t processes_count;
    bool is_parallel;
    bool uses_md5;
} configuration_t;

//************************FONCTION A AJOUTER AU FICHIER "configuration.c"****************************
//************************Je ne sais pas comment l'intégrer par contre...****************************
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

//************************FONCTION A AJOUTER AU FICHIER "file_properties.c"****************************
bool is_directory_writable(char *path_to_dir) {
    if (access(path_to_dir, W_OK) == -1){
        return 0;
    } else {
        return 1;
    }
}

//************************FONCTION A AJOUTER AU FICHIER "configuration.c"****************************
void init_configuration(configuration_t *the_config) {
    // Initialisation avec des valeurs par défaut
    strcpy(the_config->source, "");
    strcpy(the_config->destination, "");
    the_config->processes_count = 1;
    the_config->is_parallel = false;
    the_config->uses_md5 = false;
}

//************************FONCTION A AJOUTER AU FICHIER "configuration.c"****************************

int set_configuration(configuration_t *the_config, int argc, char *argv[]) {
    // Si le nombre d'arguments est insuffisant
    if (argc < 3) {
        printf("Erreur: Nombre d'arguments insuffisant.\n");
        return -1;
    }

    // Copie des valeurs des arguments dans la configuration
    strcpy(the_config->source, argv[1]);
    strcpy(the_config->destination, argv[2]);

    // Liste des arguments activés
    // Parcourir les autres options si nécessaire
    printf(" Fichier zourze : %s // Fichier Dezdinazion : %s",&the_config->source, the_config->destination);
    if (argc > 3){
        printf("arguments active :\n");
        for (int i = 3; i < argc; ++i) {
            if (strcmp(argv[i], "--date-size-only") == 0) {
                // Gérer l'option --date-size-only
                the_config->uses_md5 = true;
                printf("arguments active : --date-size-only\n");
            } else if (strcmp(argv[i], "-n") == 0) {
                // Gérer l'option -n
                if (i + 1 < argc) {
                    the_config->processes_count = atoi(argv[i + 1]);
                    i++;  // Passer à l'argument suivant
                }
                printf("arguments active : -n %d\n", the_config->processes_count);
            } else if (strcmp(argv[i], "--no-parallel") == 0) {
                // Gérer l'option --no-parallel
                the_config->is_parallel = false;
                printf("arguments active : --no-parallel\n");
            } else if (strcmp(argv[i], "-v") == 0) {
                // Gérer l'option -v
                printf("arguments active : -v\n");
            } else if (strcmp(argv[i], "--dry-run") == 0) {
                // Gérer l'option --dry-run
                printf("arguments active : --dry-run\n");
            } else {
                // Gérer les options inconnues
                printf("Erreur: Option inconnue %s.\n", argv[i]);
                return -1;
            }
        }
    }

    printf("\n");

    // Tout s'est bien passé
    return 0;
}



int main(int argc, char *argv[]) {
    configuration_t my_config;
    init_configuration(&my_config);
    if (set_configuration(&my_config, argc, argv) == -1) {
        printf("ERREUR MA COUILLE");
        return -1;
    }
    // // Variables pour les options
    // int date_size_only = 0;
    // int num = -1;
    // int no_parallel = 0;
    // int verbose = 0;
    // int dry_run = 0;

    // // Vérifier le nombre d'arguments
    // if (argc < 3) {
    //     printf("Erreur: Nombre d'arguments insuffisant.\n");
    // }

    // int opt;
    // struct option long_options[] = {
    //     {"date-size-only", no_argument, 0, 'd'},
    //     {"no-parallel", no_argument, 0, 'p'},
    //     {"dry-run", no_argument, 0, 'r'},
    //     {0, 0, 0, 0}
    // };

    // //Obtention des informations sur les paramètres demandés
    // while ((opt = getopt_long(argc, argv, "n:v", long_options, NULL)) != -1) {
    //     switch (opt) {
    //         case 'n':
    //             num = atoi(optarg);
    //             break;
    //         case 'v':
    //             verbose = 1;
    //             break;
    //         case 'd':
    //             date_size_only = 1;
    //             break;
    //         case 'p':
    //             no_parallel = 1;
    //             break;
    //         case 'r':
    //             dry_run = 1;
    //             break;
    //         case '?':
    //             if (optopt == 'n') {
    //                 fprintf(stderr, "Erreur: L'option -%c nécessite un argument.\n", optopt);
    //             } else if (isprint(optopt)) {
    //                 fprintf(stderr, "Erreur: Option inconnue -%c.\n", optopt);
    //             } else {
    //                 fprintf(stderr, "Erreur: Caractère d'option inconnu '\\x%x'.\n", optopt);
    //             }
    //             print_usage();
    //             exit(EXIT_FAILURE);
    //     }
    // }

    // // Dossiers source et destination
    // char *source = argv[argc-2];
    // char *destination = argv[argc-1];
    // // Vérifier si le dossier source existe
    // if (!directory_exists(source)) {
    //     printf("Erreur: Le dossier source n'existe pas.\n");
    //     exit(EXIT_FAILURE);
    // }

    // // Vérifier si le dossier destination existe
    // //A RAJOUTER : IL N'EST PAS DANS L'ARBORESCENCE DE FICHIERS DESTINATION
    // if (!directory_exists(destination)) {
    //     printf("Erreur: Le dossier destination n'existe pas.\n");
    //     exit(EXIT_FAILURE);
    // } else if (!is_directory_writable(destination)) {
    //     printf("Erreur: Le dossier destination existe mais n'est pas accessible en écriture.\n");
    //     exit(EXIT_FAILURE);
    // }

    // //afficher les fichiers source et destination s'ils sont conforme aux normes demandées
    // printf("Dossier source: %s et dossier destination : %s\n",source,destination);

    // printf("Je sapelle groot %s et %s\n",&my_config.source, &my_config.destination);

    if (!directory_exists(my_config.source) || !directory_exists(my_config.destination)) {
        printf("Either source or destination directory do not exist\nAborting\n");
        return -1;
    }
    // Is destination writable?
    if (!is_directory_writable(my_config.destination)) {
        printf("Destination directory %s is not writable\n", my_config.destination);
        return -1;
    }

    // // Afficher les options détectées
    // if (date_size_only) printf("Option détectée: --date-size-only\n");
    // if (num != -1) printf("Option détectée: -n %d\n", num);
    // if (no_parallel) printf("Option détectée: --no-parallel\n");
    // if (verbose) printf("Option détectée: -v\n");
    // if (dry_run) printf("Option détectée: --dry-run\n");
    
    return 0;
}
