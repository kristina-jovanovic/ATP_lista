#pragma once

#include "defs.h"
#include "status.h"

// ErrorList
#ifndef ErrorList
#define ErrorList  ((void *)(-1))
#endif

// domen
#ifndef PODATAK
#define PODATAK int
#endif

// tipovi
typedef void* POKAZIVAC;
typedef struct element {
	POKAZIVAC prethodni; //ovo ce biti NULL kod JUL
	PODATAK   podatak;
	POKAZIVAC sledeci;
} ELEMENT;
struct lista {
	void* skladiste;
	size_t kapacitet;
	size_t broj_elemenata;
};
typedef struct lista* LISTA;

typedef enum nacin { Pocetak, Kraj, Vrednost } NACIN; // na koji nacin treba da se ubaci/izbaci element
typedef enum smer_sortiranja { Rastuce, Opadajuce } SMER_SORTIRANJA;
typedef enum vrsta_pretrage { Iterativno, Binarno } VRSTA_PRETRAGE;
typedef enum algoritam_sortiranja { Bubble, Insertion, Selection } ALGORITAM_SORTIRANJA;


// funkcije
PORUKA	kreiraj(LISTA*);
PORUKA	unisti(LISTA*);
PORUKA	ubaci(LISTA*, PODATAK, NACIN);
PORUKA	izbaci(LISTA*, PODATAK*, NACIN);
void	prikazi(LISTA);
PORUKA	sortiraj(LISTA*, SMER_SORTIRANJA, ALGORITAM_SORTIRANJA);
PORUKA	prazna(LISTA);
PORUKA	sadrzi(LISTA*, PODATAK, VRSTA_PRETRAGE);


