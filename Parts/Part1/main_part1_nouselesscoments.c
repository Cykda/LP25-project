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
                    if (atoi(argv[i + 1]) != 0){
                        the_config->processes_count = atoi(argv[i + 1]);
                        i++;  // Passer à l'argument suivant                        
                    } else {
                        printf("ERREUR, l'argument de -n doit etre un entier, et doit etre different de 0");
                        return -1;
                    }
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
        printf("ERREUR");
        return -1;
    }

    if (!directory_exists(my_config.source) || !directory_exists(my_config.destination)) {
        printf("Either source or destination directory do not exist\nAborting\n");
        return -1;
    }
    // Is destination writable?
    if (!is_directory_writable(my_config.destination)) {
        printf("Destination directory %s is not writable\n", my_config.destination);
        return -1;
    }
    return 0;
}
