gestionCVS_MAIN: gestionCVS_MAIN.o gestionListeChaineeCVS.o gestionCVS.o
	gcc -o gestionCVS_MAIN gestionCVS_MAIN.o gestionListeChaineeCVS.o gestionCVS.o
gestionCVS_MAIN.o: gestionCVS_MAIN.c gestionListeChaineeCVS.h
	gcc -c gestionCVS_MAIN.c -Wall -I.
gestionListeChaineeCVS.o: gestionListeChaineeCVS.c gestionListeChaineeCVS.h
	gcc -c gestionListeChaineeCVS.c -Wall -I.
gestionCVS.o: gestionCVS.c gestionListeChaineeCVS.h
	gcc -c gestionCVS.c -Wall -I.
clean:
	rm *.o
   