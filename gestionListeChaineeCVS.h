#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <semaphore.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>
#include <stdbool.h>
#ifndef bool
    #define bool int
    #define false ((bool)0)
    #define true  ((bool)1)
#endif
struct infoligne{						
	int	noligne;
	char ptrligne[100];
};

struct noeudL{
	struct infoligne ligne;
	struct noeudL *suivant;
	sem_t semL;// Déclaration sémaphores pour les accès en exclusion mutuelle.

};
	
struct noeudV{
	int noVersion;
	char ptrNoVersion[100];
    bool oldVersion;
	bool commited;
	struct noeudL *debutL;
	struct noeudL *finL;
	struct noeudV *suivant;
     //Déclaration sémaphores pour les accès en exclusion mutuelle.

	sem_t semV, semDebutL, semFinL;//noeud de liste versions + (pointeurDebut+PointeurFin) de liste ligne de Verison 
};
struct paramAL {
	int noVersion;
	int noLigne;
	char tLigne[100];
};
struct paramAL param;


void cls(void);
void error(const int exitcode, const char * message);

struct noeudL* findItemL(const int noV, const int no);
struct noeudL* findPrevL(const int noV, const int no);
struct noeudV* findItemV(const int no);
struct noeudV* findPrevV(const int no);

void addItemV(const bool isNew, const int nv, const char* vl);
 //ancienne fonction //void addItemL(const int noVersion, const int nl, const char* tl);
void addItemL(struct paramAL* param);
void copyItemV(const bool isNew, const int nv);
void removeItemV(const int noV);
void removeItemL(const int noVersion, const int noline);
void modifyItemL(const int noVersion, const int noline, const char* tline);
void listItemsV(const int start, const int end);
void listItemsL(const int noVersion, const int start, const int end);

void saveItemsV();
void saveItemsL(const int noVersion);
void executeFile(char* sourcefname);
void listFileC();
void loadVersions();
void loadVersion(const int noV);
void* readTrans(char* nomFichier);
