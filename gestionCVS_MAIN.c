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

//Pointeur de tête de liste des versions
struct noeudV* debutV;
//Pointeur de queue de liste des versions pour ajout rapide
struct noeudV* finV;


int main(int argc, char* argv[]){

	//Initialisation des pointeurs
	debutV = NULL;
	finV = NULL;

	//"Nettoyage" de la fenêtre console
	//cls();
	
	// Chargement de la liste des versions 
	loadVersions();

	readTrans(argv[1]); // Passage du fichier de transactions

	//Fin du programme
	exit( 0);
}

