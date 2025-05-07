#pragma once

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
void	kreiraj(LISTA*);
void	unisti(LISTA);
void	ubaci(LISTA, PODATAK, NACIN);
void	izbaci(LISTA, PODATAK*, NACIN);
void	prikazi(LISTA);
void	sortiraj(LISTA, SMER_SORTIRANJA, ALGORITAM_SORTIRANJA);
bool	prazna(LISTA);
bool	sadrzi(LISTA, PODATAK, VRSTA_PRETRAGE);

/////////////////////////////////////
// status -> obrada gresaka ; deklaracija struktura
extern const struct status_kodovi {
	struct {
		int Kreiraj, Unisti, Ubaci, Lista_ne_postoji, Ucitavanje_datoteke;
	} Greska;
	struct {
		int Sortiraj, Ubaci, Izbaci, Lista_prazna;
	} Upozorenje;
	struct {
		int Kreiraj, Unisti, Ubaci, Izbaci, Podatak_postoji,
			Podatak_ne_postoji, Lista_prazna, Lista_nije_prazna, Sortiraj;
	} Info;
} Kod;

extern const struct status_poruke {
	struct {
		UTF8_STRING Kreiraj, Unisti, Ubaci, Lista_ne_postoji, Ucitavanje_datoteke;
	} Greska;
	struct {
		UTF8_STRING Sortiraj, Ubaci, Izbaci, Lista_prazna;
	} Upozorenje;
	struct {
		UTF8_STRING Kreiraj, Unisti, Ubaci, Izbaci, Podatak_postoji,
			Podatak_ne_postoji, Lista_prazna, Lista_nije_prazna, Sortiraj;
	} Info;
} Poruka;

// funkcija koja automatski vraca poruku na osnovu koda
const UTF8_STRING nadji_poruku(KOD kod);
// funkcija koja ispisuje poruku o statusu
void prijavi(KOD kod, const STRING datoteka, int linija, ...);

// definicije status kodova i poruka
const struct status_kodovi Kod;
const struct status_poruke Poruka;


