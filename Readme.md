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


1 - Analyse des arguments <br>
- voir si tout est en ordre
- si les bons arguments sont présent
- si il y a des erreurs de syntaxe et/ou de logique

2 - parcours des fichiers <br>
- lister récursivement les fichiers sans cas d'erreur

3 - récupérations des infos <br>
- récup les infos de fichier (ex: date de création, taille, etc...)

4 - traitement des infos <br>
- centraliser les infos des fichiers et les liker avec le fichier en question

5 - copie des fichiers <br>
- copie de fichiers bit à bit

6 - modification des infos des fichiers copiés <br>
- modifier les infos des fichiers fraichement copiés pour se conformer aux anciens fichiers

7 - comparaison des fichiers ( MD5 ) <br>
- comparer la somme MD5 des fichiers pour se vérifier la bonne copie des fichiers

8 - *parallélisation des tâches* (**GROSSE TÂCHE**) <br>


9 - greffe sur le squelette <br>






## Etat de Greffe

Branches greffes:
- [ ] analyse
- [ ] comparaison
- [ ] copie
- [ ] modif
- [ ] paral
- [x] parcours
- [ ] recup
