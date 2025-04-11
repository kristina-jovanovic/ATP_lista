#pragma once

#include "defs.h"

// tipovi
typedef char* STRING;

typedef enum { Info, Upozorenje, Greska } STATUS;

typedef struct poruka {
	struct {
		STRING kreiraj, unisti, ubaci, izbaci, sortiraj;
	} GRESKA;
	struct {
		STRING ubaci, izbaci, sortiraj, ucitavanje;
	} UPOZORENJE;
	struct {
		STRING kreiraj, unisti, ubaci, izbaci, podatak_postoji, podatak_ne_postoji, lista_prazna, lista_nije_prazna, sortiraj;
	} INFO;
} PORUKA;

typedef struct signal {
	STATUS status;
	STRING poruka;
} SIGNAL;

// funkcije
void obrada_statusa(STATUS, STRING, STRING, int); ///////////