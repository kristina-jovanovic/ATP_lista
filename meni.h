#pragma once

#include "defs.h"
#define MAX_ITEMS 10

// tipovi

// definicija tipa funkcije koja predstavlja akciju opcije (void funkcija bez parametara)
typedef void (*FUNKCIJA)();

typedef struct stavka_menija {
	STRING opis;		// tekstualni opis stavke menija
	FUNKCIJA funkcija;  //pokazivac na f-ju koja se poziva prilikom izbora ove stavke menija
} STAVKA_MENIJA;

typedef struct meni {
	STRING naziv;
	int broj_stavki;
	STAVKA_MENIJA stavke[MAX_ITEMS];
} MENI;

// funkcije
void kreiraj_meni(MENI* meni, STRING naziv_datoteke); // ucitavanje podataka o meniju iz datoteke
void prikazi_meni(MENI meni);
void obradi_opciju(MENI meni, int opcija);

