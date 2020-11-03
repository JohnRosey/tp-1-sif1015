//#########################################################
//#
//# Titre : 	UTILITAIRES (MAIN) TP1 LINUX Automne 20
//#				SIF-1015 - Système d'exploitation
//#				Université du Québec à Trois-Rivières
//#
//# Auteur : 	Francois Meunier
//#	Date :		Septembre 2020
//#
//# Langage : 	ANSI C on LINUX 
//#
//#######################################

#include "gestionListeChaineeCVS.h"

struct noeudV* debutV;
struct noeudV* finV;
// semaphores pour l'exclusion mututelle
sem_t semDebutV, semFinV; 

int main() {
	sem_init(&semDebutV, 0, 1);
	sem_init(&semFinV, 0, 1);

	cls();

	loadVersions();

	printf("Rentrez un nom de fichier de transaction: \n");
	char word[20];
	scanf("%s", word);

	readTrans(word);

	sem_destroy(&semDebutV);
	sem_destroy(&semFinV);

	exit(0);
}
