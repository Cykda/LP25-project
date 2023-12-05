#include "configuration.h"
#include <stddef.h>
#include <stdlib.h>
#include <getopt.h>
#include <stdio.h>
#include <string.h>

typedef enum {DATE_SIZE_ONLY, NO_PARALLEL} long_opt_values;

/*!
 * @brief function display_help displays a brief manual for the program usage
 * @param my_name is the name of the binary file
 * This function is provided with its code, you don't have to implement nor modify it.
 */
void display_help(char *my_name) {
    printf("%s [options] source_dir destination_dir\n", my_name);
    printf("Options: \t-n <processes count>\tnumber of processes for file calculations\n");
    printf("         \t-h display help (this text)\n");
    printf("         \t--date_size_only disables MD5 calculation for files\n");
    printf("         \t--no-parallel disables parallel computing (cancels values of option -n)\n");
}

/*!
 * @brief init_configuration initializes the configuration with default values
 * @param the_config is a pointer to the configuration to be initialized
 */
void init_configuration(configuration_t *the_config) {
    // Initialisation avec des valeurs par défaut
    strcpy(the_config->source, "");
    strcpy(the_config->destination, "");
    the_config->processes_count = 1;
    the_config->is_parallel = false;
    the_config->uses_md5 = false;
}
/*!
 * @brief set_configuration updates a configuration based on options and parameters passed to the program CLI
 * @param the_config is a pointer to the configuration to update
 * @param argc is the number of arguments to be processed
 * @param argv is an array of strings with the program parameters
 * @return -1 if configuration cannot succeed, 0 when ok
 */
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
