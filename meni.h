#pragma once

#include "defs.h"
#define MAX_ITEMS 10
#define MAX_DESCRIPTION 100
#define MAX_NAME 30

// tipovi

// definicija tipa funkcije koja predstavlja akciju opcije (void funkcija bez parametara)
typedef void (*FUNKCIJA)();

typedef struct stavka_menija {
	wchar_t opis[MAX_DESCRIPTION];		// tekstualni opis stavke menija
	FUNKCIJA funkcija;  //pokazivac na f-ju koja se poziva prilikom izbora ove stavke menija
} STAVKA_MENIJA;

typedef struct meni {
	wchar_t naziv[MAX_NAME];
	int broj_stavki;
	STAVKA_MENIJA stavke[MAX_ITEMS];
} MENI;

// funkcije
void kreiraj_meni(MENI* meni, STRING naziv_datoteke); // ucitavanje podataka o meniju iz datoteke
void prikazi_meni(MENI meni);
void obradi_opciju(MENI meni, int opcija);

