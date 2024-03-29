//#########################################################
//#
//# Titre : 	Utilitaires CVS LINUX Automne 20
//#		SIF-1015 - Système d'exploitation
//#		Université du Québec à Trois-Rivières
//#
//# Auteur : 	Francois Meunier
//# Date :	Septembre 2020
//#
//# Langage : 	ANSI C on LINUX 
//#
//#######################################
#include <stdlib.h>
 #include <semaphore.h> //Pour les semaphores
#include "gestionListeChaineeCVS.h"

//Pointeur de tête de la liste de versions 
extern struct noeudV* debutV;
//Pointeur de queue de liste de versions
extern struct noeudV* finV;

//#######################################
//#
//# Affiche une série de retour de ligne pour "nettoyer" la console
//#
void cls(void){
	printf("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n");
}

//#######################################
//#
//# Affiche un message et quitte le programme
//#
void error(const int exitcode, const char * message){
	printf("\n-------------------------\n%s\n",message);
	exit(exitcode);
}

//#######################################
//#
//# Chargement d'une version Vi ou i est le numero de version
//#
void loadVersions(){
	//Initialisation des pointeurs sur la liste de versions
	debutV = NULL;
	finV   = NULL;

	char version[100];
	int numVersion=1;

	//Ouverture du fichier contenant la liste des version en mode "rt" : [r]ead [t]ext
	FILE *f;
	f = fopen("ListeVersions.txt", "rt");
	
	// Au debut le fichier de version n'existe pas ,..
	if (f==NULL) {
	  	addItemV(true, 1, "V1"); // Ajouter une premiere version vide	  
	}
	else {	
		// Fichier existe donc des versions sont disponibles
  		// Ajout des éléments extraits du fichier de versions
	  	while(!feof(f)){
	    	fgets(version, 100, f);
	    	addItemV(false, numVersion++, version);
	    	loadVersion(numVersion-1); // chargement du code de la version
	  	}

	  	fclose(f);
	}
 }
 
//#######################################
//#
//# Chargement d'une version Vi ou i est le numero de version
//#
void loadVersion(const int noV){

	char tligne[100];
	//int noligne = 0;
	
	struct noeudV * ptr = findItemV(noV);
	
	if(ptr == NULL) // Verifier si la version existe
	  return;
	
	ptr->debutL = NULL;
	ptr->finL = NULL;
	ptr->oldVersion = true;
	ptr->commited = false;

	//Ouverture du fichier source en mode "rt" : [r]ead [t]ext
	FILE *f;
	f = fopen(ptr->ptrNoVersion, "rt"); // ouverture du fichier de code c de la version
	if (f==NULL)
	  return;

	//Ajout des éléments extraits du fichier source
	
	while(!feof(f)){
		fgets(tligne, 100, f);

		struct paramAL *ptr = (struct paramAL*) malloc(sizeof(struct paramAL));
		ptr->noVersion = noV;
		ptr->noLigne += 1;
		strcpy(ptr->tLigne, (const char *)tligne);

		addItemL(ptr);
	}
	
	fclose(f);
 }

 
//#######################################
//#
//# Enregistre le contenu de la liste chaînée de versions  dans le  fichier texte ListeVersion.txt
//#
void saveItemsV(){

	struct noeudV * ptr = debutV;	// premier element de la liste des versions
	FILE *f;

	//Ouverture du fichier en mode "wt" : [w]rite [t]ext

 	f= fopen("./output/ListeVersions.txt", "wt");
	
	if (f==NULL)
		error(2, "saveItemsV: Erreur lors de l'ouverture du fichier des versions pour écriture en mode texte.");

	while (ptr!=NULL){
		if((ptr->oldVersion == true) || (ptr->commited == true)){ 
		  //Écriture des données
		  fprintf(f,"%s\n",ptr->ptrNoVersion);
		}
		//Déplacement du pointeur
		ptr = ptr->suivant;
	}
	
	fclose(f);
}
	
//#######################################
//#
//# Enregistre le contenu de la liste chaînée de lignes de codes dans un fichier texte
//#
void saveItemsL(const int noVersion){
	struct noeudV * ptrV;
	
	ptrV = findItemV(noVersion);
	
	// Verifier si la version existe
	if (ptrV==NULL)
	  return;  

	ptrV->commited = true;
	struct noeudL * ptr = ptrV->debutL;	// premier element de la liste de code
	FILE *f;
	char nomC[200];
	char nomTXT[200];
	
	strcpy(nomTXT,ptrV->ptrNoVersion); // copier le nom de version
	sprintf(nomC,"./output/%s.c",nomTXT);


	//Ouverture du fichier en mode "wt" : [w]rite [t]ext
 	f= fopen(nomC, "wt");

	
	if (f==NULL)
		error(2, "saveItemsL: Erreur lors de l'ouverture du fichier pour écriture en mode texte.");

	while (ptr!=NULL){

		//Écriture des données
		fprintf(f,"%s\n",ptr->ligne.ptrligne);
		
		//Déplacement du pointeur
		ptr = ptr->suivant;
	}
	
	fclose(f);
}

//#######################################
//#
//# Execute le fichier source .c 
//#
void executeFile(char* sourcefname){

	char command[100];
	char *nomFichier, *sp;
	
	nomFichier = strtok_r(sourcefname, ".", &sp); // extraction du nom de fichier sans ext

	sprintf(command, "gcc -o ./output/fichCVSEXE%s ./output/%s.c\n", nomFichier, nomFichier);
	system(command);

	// execution du fichier fichCVSEXE
	sprintf(command, "./output/fichCVSEXE%s", nomFichier);
	system(command);
}

//#######################################
//#
//# Lister  les fichiers sources .c du repertoire courant
//#
void listFileC(){
	char command[100];

	// listage des fichiers .c du dossier courant 
	sprintf(command, "ls -l ./output/V*.c");
	system(command);
}


//#######################################
//#
//# fonction utilisée par les threads de transactions
//#
void* readTrans(char* nomFichier){
	FILE *f;
	char buffer[100];
	char *tok, *sp;

	pthread_t tid[1000];
	int nbThreads = 0;

	//Ouverture du fichier en mode "r" (equiv. "rt") : [r]ead [t]ext
	f = fopen(nomFichier, "rt");
	if (f==NULL)
		error(2, "readTrans: Erreur lors de l'ouverture du fichier.");

	//Lecture (tentative) d'une ligne de texte
	fgets(buffer, 100, f);
	
	//Pour chacune des lignes lues
	while(!feof(f)){

		//Extraction du type de transaction 2 cas: commande avec arguments et sans argument
		tok = strtok_r(buffer, " .", &sp);

		//Branchement selon le type de transac	
		if(strcmp(tok, "AL") == 0){
			//Extraction des paramètres
				int noVersion = atoi(strtok_r(NULL, " ", &sp));
				int noligne = atoi(strtok_r(NULL, " ", &sp));
				char *tligne = strtok_r(NULL, "\n", &sp);
			// Allocation de mémoire pour paramAl la fonction  addItemL().
			struct paramAL *ptr = (struct paramAL*) malloc(sizeof(struct paramAL));
			ptr->noVersion = atoi(strtok_r(NULL, " ", &sp));
			ptr->noLigne = atoi(strtok_r(NULL, " ", &sp));
			strcpy(ptr->tLigne, (const char *)strtok_r(NULL, "\n", &sp));

			// thread pour faire addItemL() en  concurrence.
			
			pthread_create(&tid[nbThreads++], NULL, addItemL, (void *)ptr); // cast de ptr en pointeur void.
			
		}
		else if(strcmp(tok, "CV") == 0){
			//Extraction des paramètres
			int noVersion = atoi(strtok_r(NULL, "\n ", &sp));
			//Appel de la fonction associée
			copyItemV(true, noVersion);
				
		}
		else if(strcmp(tok, "EL") == 0){
			//Extraction du paramètre
			int noversion = atoi(strtok_r(NULL, " ", &sp));
			int noligne = atoi(strtok_r(NULL, "\n", &sp));
			//Appel de la fonction associée
			removeItemL(noversion, noligne);
				
		}
		else if(strcmp(tok, "EV") == 0){
			//Extraction du paramètre
			int noversion = atoi(strtok_r(NULL, "\n", &sp));
			//Appel de la fonction associée
			removeItemV(noversion);
				
		}
		else if(strcmp(tok, "ML") == 0){
			//Extraction des paramètres
			int noversion = atoi(strtok_r(NULL, " ", &sp));
			int noligne = atoi(strtok_r(NULL, " ", &sp));
			char *tligne = strtok_r(NULL, "\n", &sp);
			//Appel de la fonction associée
			modifyItemL(noversion, noligne,tligne);			
				
		}
		else if(strcmp(tok, "LL") == 0){
			//Extraction des paramètres
			int noversion = atoi(strtok_r(NULL, " ", &sp));
			int nstart = atoi(strtok_r(NULL, "-", &sp));
			int nend = atoi(strtok_r(NULL, "\n", &sp));
			//Appel de la fonction associée
			listItemsL(noversion, nstart, nend);
				
		}
		else if(strcmp(tok, "LV") == 0){
			//Extraction des paramètres
			int nstart = atoi(strtok_r(NULL, "-", &sp));
			int nend = atoi(strtok_r(NULL, "\n", &sp));
			//Appel de la fonction associée
			listItemsV(nstart, nend);
				
		}
		else if(strcmp(tok, "LC") == 0){ // lister les fichier c 
			//Appel de la fonction associée
			listFileC();
				
		}
		else if(strcmp(tok, "SV") == 0){ // sauvegarde de la liste des versions
			//Appel de la fonction associée
			saveItemsV();
				
		}
		else if(strcmp(tok, "SL") == 0){
			//Appel de la fonction associée
			int noversion = atoi(strtok_r(NULL, "\n", &sp));
			saveItemsL(noversion);
				
		}
		else if(strcmp(tok, "X") == 0){
			//Appel de la fonction associée
			char *nomfich = strtok_r(NULL, "\n", &sp); // nom du fichier source .c
			executeFile(nomfich);
				
		}

		//Lecture (tentative) de la prochaine ligne de texte
		fgets(buffer, 100, f);
	}

	// faire un pthread_join() sur la liste des tid apres la boucle de gestion des transactions
	int i = 0;
	for (i = 0; i < nbThreads; i++) {
		pthread_join(tid[i], NULL);
	}

	fclose(f);
	return NULL;
}
