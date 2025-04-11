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

// funkcije
SIGNAL	kreiraj(LISTA*);
SIGNAL	unisti(LISTA*);
SIGNAL	ubaci_na_pocetak(LISTA*, PODATAK);
SIGNAL	izbaci_sa_pocetka(LISTA*, PODATAK*);
void	prikazi(LISTA);
SIGNAL	sortiraj(LISTA*);
SIGNAL	prazna(LISTA);
SIGNAL	sadrzi(LISTA, PODATAK);


