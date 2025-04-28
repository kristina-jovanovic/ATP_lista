#pragma once

#include "defs.h"

// tipovi
typedef char* STRING;

typedef enum { Info, Upozorenje, Greska } STATUS;

// kodovi poruka
typedef enum {
	//info kodovi
	INFO_KREIRAJ,
	INFO_UNISTI,
	INFO_UBACI,
	INFO_IZBACI,
	INFO_PODATAK_POSTOJI,
	INFO_PODATAK_NE_POSTOJI,
	INFO_LISTA_PRAZNA,
	INFO_LISTA_NIJE_PRAZNA,
	INFO_SORTIRAJ,
	//upozorenje kodovi
	UPOZORENJE_LISTA_PRAZNA, //ovo ce biti prijavljeno ukoliko zelimo da odradimo neku operaciju koja zahteva da lista ima elemenata, a lista je prazna -> operacija nece moci da se izvrsi
	UPOZORENJE_UBACI, // kada lista ima maskimalni dozvoljeni broj elemenata
	UPOZORENJE_IZBACI, //kada zelimo da izbacimo element odredjene vrednosti, a ta vrednost ne postoji u listi
	UPOZORENJE_LISTA_SORTIRAJ, //ukoliko imamo manje od 2 elementa 
	//greska kodovi
	GRESKA_UCITAVANJE_DATOTEKE,
	GRESKA_KREIRAJ,
	GRESKA_UNISTI,
	GRESKA_UBACI,
	GRESKA_LISTA_NE_POSTOJI, //ovo ce biti prijavljeno u bilo kojoj funkciji ukoliko je lista==NULL i slicno
	// da li ipak odvojiti da svaka funkcija ima svoj kod poruke za gresku iako je uzrok isti??
	// cuvamo i broj kodova
	BROJ_KODOVA // ovo dodajemo kao poslenju stavku u enum-u da znamo koliko ima kodova (kao brojac)
				// on ce dobiti vrednost broja kodova poruka jer se enum vrednosti dodaju redom od 0 ukoliko ne zadamo svoje vrednosti
} KOD_PORUKE;

typedef struct poruka {
	STATUS status; // Info, Upozorenje, Greska
	STRING tekst; // konkretan tekst poruke
} PORUKA;

// niz svih poruka
extern PORUKA poruke[];

// funkcije
void prijavi(KOD_PORUKE kod, STRING naziv_datoteke, int linija);
