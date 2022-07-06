# Brep-Tools 
## Un emsemble d'outils pour comprendre et manipuler des fichiers STEP

## Effectuer un taper sur un fichier STEP

#### En cours d'implementation, voir les fonctions de types taper* dans Utils.cpp pour faire des essais, 
#### Utiliser CAD_ASSISTANT de OpenCascade pour visualiser les fichier step
## Récuperer des informations sur un ensemble de fichiers STEP :

- ### modifier main.cpp pour appeler StepFolder_Stats() seulement (disponible dans exemples...)

- Mettre dans les fichiers STEP dans le dossier step_files

- Creer un dossier build, s'y rendre à l'aide d'un terminal

- Configurer et compiler avec cmake.. && make

  (si il y a des erreurs cmake voir CmakeList ou avec Cmake-gui)

- Exécuter occ_test dans build

### Visualiser les informations:

- #### Executer le script python stats_analyser.py (python 3)
