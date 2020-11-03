//#########################################################
//#
//# Titre : 	Utilitaires Liste Chainee et CVS LINUX Automne 20
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

extern struct noeudV* debutV;
extern struct noeudV* finV;
extern sem_t semDebutV, semFinV;
//#######################################
//#
//# Recherche un item dans la liste chaînée de VERSIONS
//#
//# RETOUR:	Un pointeur vers l'item recherché
//# 		
//# 		Retourne NULL dans le cas où l'item
//#			est introuvable
//#

struct noeudV* findItemV(const int no) {

	//La liste est vide 
	if ((debutV==NULL)&&(finV==NULL)) return NULL;
       
 struct noeudV * ptr = debutV;//curseur de navigation
		if(ptr->noVersion==no)
	    	return ptr;
//Tant qu'un item suivant existe
		while (ptr->suivant){
			ptr=ptr->suivant;
			if (ptr->noVersion==no){
				return ptr;
			}
		}
    }

	return NULL;
}

struct noeudL* findItemL(const int noV, const int no) {
  
  	struct noeudV* ptrV;
	
	ptrV = findItemV(noV);

	if (ptrV && (ptrV->debutL && ptrV->finL)) {
		struct noeudL* ptr = ptrV->debutL;

		if(ptr->ligne.noligne==no)
			return ptr;

		while (ptr->suivant) {
			ptr=ptr->suivant;
			if (ptr->ligne.noligne==no){
				return ptr;
			}
		}
	}

	return NULL;
}

struct noeudV* findPrevV(const int no){

	if (debutV && finV) {
		struct noeudV* ptr = debutV;

		while (ptr->suivant){
			if (ptr->suivant->noVersion==no){
				return ptr;
			}
			ptr=ptr->suivant;
		}
	}

	return NULL;
}

struct noeudL* findPrevL(const int noV, const int no){
  
  	struct noeudV* ptrV;
	
	ptrV = findItemV(noV);
	
	if (ptrV && (ptrV->debutL && ptrV->finL)) {

		struct noeudL* ptr = ptrV->debutL;

		while (ptr->suivant){
			if (ptr->suivant->ligne.noligne==no){
				return ptr;
			}
			ptr=ptr->suivant;
		}
	}

	return NULL;
}

void addItemL(struct paramAL* param){
	noVersion = param ->noVersion;
	nl = param->noligne;
	strcpy(tl,(const char*)param->tligne);
	free(param);
	struct noeudV* ptrV;
	
	ptrV = findItemV(noVersion);

	sem_wait(&ptrV->semV);
	if (ptrV) {
		struct noeudL* ni = (struct noeudL*)malloc(sizeof(struct noeudL));
		struct noeudL* ptrINS = findItemL(noVersion, nl);

		ni->ligne.noligne = nl;
		strcpy(ni->ligne.ptrligne, tl);

		//initilisation du semaphore
		sem_init(&ni->semL, 0, 1);

		sem_wait(&ptrV->semDebutL);
		sem_wait(&ptrV->semFinL);
		if(!ptrINS && nl == 1) {
			ni->suivant= NULL;
			ptrV->finL = ptrV-> debutL = ni;
		}
		else if (!ptrINS && nl > 1) {
			struct noeudL* tptr = ptrV->finL;
			ni->suivant= NULL;
			ptrV->finL = ni;
			tptr->suivant = ni;
		}
		else {
			struct noeudL* tptr = ptrINS;
			if(tptr == ptrV->debutL) // ajout a la tete de la liste
				ptrV->debutL = ni;
			else
			{
				struct noeudL* ptrPREV = findPrevL(noVersion, nl);
				ptrPREV->suivant = ni;
			}
			ni->suivant = tptr;
			
			while (tptr) {
				tptr->ligne.noligne++;
				tptr=tptr->suivant;
			}
		}
		sem_post(&ptrV->semDebutL);
		sem_post(&ptrV->semFinL);
	}
	sem_post(&ptrV->semV);
}

void addItemV(const bool isNew, const int nv, const char* vl){

	struct noeudV* ni = (struct noeudV*)malloc(sizeof(struct noeudV));

	ni->noVersion = nv;
	strcpy(ni->ptrNoVersion, vl);
	ni->oldVersion = !isNew;
	ni->commited = false;
	ni->debutL = ni->finL = NULL;

	sem_init(&ni->semDebutL, 0, 1);
	sem_init(&ni->semFinL, 0, 1);
	sem_init(&ni->semV, 0, 1);

	sem_wait(&semFinV);
	if(!finV) {
		ni->suivant= NULL;
		sem_wait(&semDebutV);
		finV = debutV = ni;
		sem_post(&semDebutV);
	}
	else {
	    struct noeudV* tptr = finV;
	    ni->suivant= NULL;
	    finV = ni;
	    tptr->suivant = ni;	
	}
	sem_post(&semFinV);
}

void copyItemV(const bool isNew, const int nv){
  
	struct noeudV* ptrV = findItemV(nv);
	
	sem_wait(&ptrV->semV);
	if (ptrV) {
		struct noeudV* ni = (struct noeudV*)malloc(sizeof(struct noeudV));

		sem_wait(&semFinV);
		ni->noVersion = finV->noVersion+1;

		char nomVersion[200];
		sprintf(nomVersion,"V%d",ni->noVersion);
		strcpy(ni->ptrNoVersion, nomVersion);
		ni->debutL = ni->finL = NULL;
		ni->commited = false;
		ni->oldVersion = !isNew;

		sem_init(&ni->semDebutL, 0, 1);
		sem_init(&ni->semFinL, 0, 1);
		sem_init(&ni->semV, 0, 1);

		struct noeudV* tptr = finV;
		ni->suivant= NULL;
		finV = ni;
		tptr->suivant = ni;
		
		sem_wait(&ptrV->semDebutL);
		struct noeudL * ptrL = ptrV->debutL;
		while (ptrL){
			addItemL(finV->noVersion, ptrL->ligne.noligne, ptrL->ligne.ptrligne);
			ptrL = ptrL->suivant;
		}
		sem_post(&ptrV->semDebutL);
		sem_post(&semFinV);
	}
	sem_post(&ptrV->semV);
}

void removeItemV(const int noV){

	struct noeudV* ptrV;
	struct noeudV* tptrV = NULL;
	struct noeudV* optrV;
	struct noeudL* ptrL;
	struct noeudL* ptrLSuivant;
	
	char texteVersion[100];
	
	if ((noV>0) && (debutV && finV)) {

		if(noV == 1){
			ptrV = debutV;
		}
		else{
			ptrV = findPrevV(noV);
		}

		if (ptrV) {
			if(debutV == ptrV)
			{
				if(debutV == finV)
				{
					ptrL = ptrV->debutL;
					while (ptrL) { 
						ptrLSuivant = ptrL->suivant;
						free(ptrL);
						ptrL = ptrLSuivant;
					}
					free(ptrV);
					finV = debutV = NULL;
				} 
				else {
					tptrV = ptrV->suivant;
					debutV = tptrV;
					ptrL = ptrV->debutL;
					while (ptrL) { 
						ptrLSuivant = ptrL->suivant;
						free(ptrL);
						ptrL = ptrLSuivant;
					}
					free(ptrV);
				}
			}
			else if (finV == ptrV->suivant) {
				finV = ptrV;
				ptrL = ptrV->suivant->debutL;
				while (ptrL) { 
					ptrLSuivant = ptrL->suivant;
					free(ptrL);
					ptrL = ptrLSuivant;
				}
				free(ptrV->suivant);
				ptrV->suivant = NULL;
			}
			else {
				optrV = ptrV->suivant;
				ptrL = ptrV->suivant->debutL; // supprimer la liste de code de la version a la fin
				while (ptrL) { 
					ptrLSuivant = ptrL->suivant;
					free(ptrL); // suppression du noeud de la liste de code 
					ptrL = ptrLSuivant;
				}
				ptrV->suivant = ptrV->suivant->suivant;
				tptrV = ptrV->suivant;
				free(optrV);
			}
			
			while (tptrV) {
				tptrV->noVersion--;

				sprintf(texteVersion, "V%d",tptrV->noVersion);
				strcpy(tptrV->ptrNoVersion, texteVersion);

				tptrV=tptrV->suivant;
			}
		}
	}
}

void removeItemL(const int noVersion, const int noline){

	struct noeudL* ptrL;
	struct noeudL* tptrL = NULL;
	struct noeudL* optrL;
	struct noeudV* ptrV;
	
	ptrV = findItemV(noVersion);
	
	if (ptrV && noline>0 && (ptrV->debutL && ptrV->finL)) {

		if(noline==1){
			ptrL = ptrV->debutL;
		}
		else{
			ptrL = findPrevL(noVersion, noline);
		}

		if (ptrL){
			if(ptrV->debutL  == ptrL){
				if(ptrV->debutL == ptrV->finL) {
					free(ptrL);
					ptrV->debutL = ptrV->finL = NULL;
				}
				else {
					tptrL = ptrL->suivant;
					ptrV->debutL = tptrL;
					free(ptrL);
				}
			}
			else if (ptrV->finL == ptrL->suivant) {
				ptrV->finL = ptrL;
				free(ptrL->suivant);
				ptrL->suivant=NULL;
			}
			else {
				optrL = ptrL->suivant;	
				ptrL->suivant = ptrL->suivant->suivant;
				tptrL = ptrL->suivant;
				free(optrL);
			}

			while (tptrL) {
				tptrL->ligne.noligne--;
				tptrL=tptrL->suivant;
			}
		}
	}
}

void modifyItemL(const int noVersion, const int noline, const char* tline){
	struct noeudV* ptrV;

	ptrV = findItemV(noVersion);
	
	if (ptrV && noline>0 && (ptrV->debutL && ptrV->finL)) {

		struct noeudL* ptrL = findItemL(noVersion, noline);

		if (ptrL){
			strcpy(ptrL->ligne.ptrligne, tline);
		}
	}
}

void listItemsV(const int start, const int end){

	printf("noVersion  Nom Version                                  \n");
	printf("======= ================================================\n");


	sem_wait(&semDebutV);

	struct noeudV* ptr = debutV;

	sem_post(&semDebutV);

	while (ptr){
		if (ptr->noVersion >= start && ptr->noVersion <= end) {
			printf("%d \t %s\n",
				ptr->noVersion,
				ptr->ptrNoVersion);
			}
		if (ptr->noVersion > end){
			ptr = NULL;
		}
		else{
			ptr = ptr->suivant;
		}

	}

	printf("======= ===============================================\n\n");
}

void listItemsL(const int noVersion, const int start, const int end){
  
	struct noeudV* ptrV;
	
	ptrV = findItemV(noVersion);

	sem_wait(&ptrV->semV);
	if (ptrV) {
		printf("noVersion noligne	texte                                          \n");
		printf("=======   ===========   ==================================\n");

		sem_wait(&ptrV->semDebutL);

		struct noeudL* ptr = ptrV->debutL;

		sem_post(&ptrV->semDebutL);

		while (ptr){
			if (ptr->ligne.noligne >= start && ptr->ligne.noligne <= end) {
				printf("%d \t %d \t %s\n",
					ptrV->noVersion,
					ptr->ligne.noligne,
					ptr->ligne.ptrligne);
				}
			if (ptr->ligne.noligne > end) {
				ptr = NULL;
			}
			else {
				ptr = ptr->suivant;
			}
		}

		printf("=========================================================\n\n");
	}

	sem_post(&ptrV->semV);
}
