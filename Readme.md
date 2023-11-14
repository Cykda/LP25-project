# LP25 Project



## organisation du travail

Date 1er rendu: 10 Décembre
Date rendu final: 7 janvier 


### Liste des tâches

- Ensemble 1
    - Analyse des options du programme (voir la section option des instructions)
    - Gestion des listes chaînées de fichiers
- Ensemble 2
    - Opérations de tests sur les répertoires (existence, écrivable, etc.)
    - Recherche des informations de fichiers/dossiers (stat)
    - Calcul de la somme de contrôle des fichiers (MD5). Peut se tester manuellement sur des fichiers en comparant le résultat de l'implémentation avec l'appel à la commande md5sum
- Ensemble 3
    - Analyse séquentielle (pas de processus)
    - Copie des fichiers à synchroniser (avec conservation des droits et de la mtime)
- Ensemble 4
    - Implémentation multiprocessus
        - Création des processus
        - Gestion de la MQ
        - Implémentation du protocole entre processus
- Ensemble 5
    - Option verbose
    - Option dry-run
    - Exécution de la bonne implémentation (parallèle ou non) en fonction de l'option de parallélisme



0 - organiser la structure du projet


1 - Analyse des arguments
2 - parcours des fichiers
3 - récupérations des infos
4 - traitement des infos
5 - copie des fichiers
6 - modification des infos des fichiers copiés
7 - comparaison des fichiers ( MD5 )


8 - *parallélisation des tâches* (**GROSSE TÂCHE**)


9 - greffe sur le squelette
