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
		STRING Kreiraj, Unisti, Ubaci, Lista_ne_postoji, Ucitavanje_datoteke;
	} Greska;
	struct {
		STRING Sortiraj, Ubaci, Izbaci, Lista_prazna;
	} Upozorenje;
	struct {
		STRING Kreiraj, Unisti, Ubaci, Izbaci, Podatak_postoji,
			Podatak_ne_postoji, Lista_prazna, Lista_nije_prazna, Sortiraj;
	} Info;
} Poruka;

// funkcija koja automatski vraca poruku na osnovu koda
const UTF8_STRING nadji_poruku(KOD kod) {
	switch (kod) {
		// Greska
	case 0: return Poruka.Greska.Kreiraj;
	case 1: return Poruka.Greska.Unisti;
	case 2: return Poruka.Greska.Ubaci;
	case 3: return Poruka.Greska.Lista_ne_postoji;
	case 4: return Poruka.Greska.Ucitavanje_datoteke;

		// Upozorenje
	case 10: return Poruka.Upozorenje.Sortiraj;
	case 11: return Poruka.Upozorenje.Ubaci;
	case 12: return Poruka.Upozorenje.Izbaci;
	case 13: return Poruka.Upozorenje.Lista_prazna;

		// Info
	case 20: return Poruka.Info.Kreiraj;
	case 21: return Poruka.Info.Unisti;
	case 22: return Poruka.Info.Ubaci;
	case 23: return Poruka.Info.Izbaci;
	case 24: return Poruka.Info.Podatak_postoji;
	case 25: return Poruka.Info.Podatak_ne_postoji;
	case 26: return Poruka.Info.Lista_prazna;
	case 27: return Poruka.Info.Lista_nije_prazna;
	case 28: return Poruka.Info.Sortiraj;

	default: return L"Непознат статус код.";
	}
}
// funkcija koja ispisuje poruku o statusu
static inline void prijavi(KOD kod, const STRING datoteka, int linija, ...) {
	STATUS status;
	wchar_t w_datoteka[260];
	mbstowcs(w_datoteka, datoteka, 260); //multibyte string to wide char string, 260 je kapacitet w_datoteka

	if (kod >= 1 && kod < 10)
		status = Greska;
	else if (kod >= 10 && kod < 20)
		status = Upozorenje;
	else
		status = Info;

	va_list args;
	va_start(args, linija);

	const UTF8_STRING poruka = nadji_poruku(kod);
	switch (status)
	{
	case Info:
		fwprintf(stdout, L"[ИНФО] ");
		vfwprintf(stdout, poruka, args);
		fwprintf(stdout, L"\n");
		break;
	case Upozorenje:
		fwprintf(stderr, L"[УПОЗОРЕЊЕ] ");
		vfwprintf(stderr, poruka, args);
		fwprintf(stderr, L"\nДАТОТЕКА: %ls, ЛИНИЈА: %d\n", w_datoteka, linija);
		break;
	case Greska:
		fwprintf(stderr, L"[ГРЕШКА] ");
		vfwprintf(stderr, poruka, args);
		fwprintf(stderr, L"\nДАТОТЕКА: %ls, ЛИНИЈА: %d\n", w_datoteka, linija);
		fwprintf(stderr, L"Програм ће бити прекинут.\n");
		exit(EXIT_FAILURE);
		break;
	default:
		break;
	}

	va_end(args);
}

// definicije status kodova i poruka
const struct status_kodovi Kod = {
	.Greska = {.Kreiraj = 0, .Unisti = 1, .Ubaci = 2, .Lista_ne_postoji = 3, .Ucitavanje_datoteke = 4 },
	.Upozorenje = {.Sortiraj = 10,.Ubaci = 11,.Izbaci = 12, .Lista_prazna = 13 },
	.Info = {.Kreiraj = 20, .Unisti = 21, .Ubaci = 22, .Izbaci = 23, .Podatak_postoji = 24,
			.Podatak_ne_postoji = 25, .Lista_prazna = 26, .Lista_nije_prazna = 27, .Sortiraj = 28 }
};

const struct status_poruke Poruka = {
	.Greska = {
		.Kreiraj = L"Није могуће креирати листу.",
		.Unisti = L"Није могуће уништити листу.",
		.Ubaci = L"Није могуће убацити елемент у листу.",
		.Lista_ne_postoji = L"Није могуће извршити операцију јер листа не постоји.",
		.Ucitavanje_datoteke = L"Неуспешно учитавање датотеке.",
	},
	.Upozorenje = {
		.Sortiraj = L"Није могуће сортирати листу јер има мање од 2 елемента.",
		.Ubaci = L"Није могуће убацити елемент у листу јер је достигнут максимални капацитет.",
		.Izbaci = L"Није могуће избацити елемент са вредношћу %d јер не постоји у листи.",
		.Lista_prazna = L"Није могуће извршити операцију јер је листа празна.",
	},
	.Info = {
		.Kreiraj = L"Листа је успешно креирана.",
		.Unisti = L"Листа је успешно уништена.",
		.Ubaci = L"Елемент је успешно убачен у листу.",
		.Izbaci = L"Елемент са вредношћу %d је успешно избачен из листе.",
		.Podatak_postoji = L"Елемент %d постоји у листи.",
		.Podatak_ne_postoji = L"Елемент %d не постоји у листи.",
		.Lista_prazna = L"Листа је празна.",
		.Lista_nije_prazna = L"Листа није празна.",
		.Sortiraj = L"Листа је успешно сортирана %s.",
	}
};


