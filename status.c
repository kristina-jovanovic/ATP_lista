#include "defs.h"
#include "status.h"

// Definicija svih poruka
PORUKA poruke[BROJ_KODOVA] = {
	// info poruke
	[INFO_KREIRAJ] = { Info, "Lista je uspesno kreirana." },
	[INFO_UNISTI] = { Info, "Lista je uspesno unistena." },
	[INFO_UBACI] = { Info, "Element je uspesno ubacen u listu."},
	[INFO_IZBACI] = { Info, "Element sa vrednoscu %d je uspesno izbacen iz liste."},
	[INFO_PODATAK_POSTOJI] = { Info, "Element %d postoji u listi."},
	[INFO_PODATAK_NE_POSTOJI] = { Info, "Element %d ne postoji u listi."},
	[INFO_LISTA_PRAZNA] = { Info, "Lista je prazna."},
	[INFO_LISTA_NIJE_PRAZNA] = { Info, "Lista nije prazna."},
	[INFO_SORTIRAJ] = { Info, "Lista je uspesno sortirana %s."},
	// upozorenje poruke
	[UPOZORENJE_LISTA_PRAZNA] = { Upozorenje, "Nije moguce izvrsiti operaciju jer je lista prazna."},
	[UPOZORENJE_IZBACI] = { Upozorenje, "Nije moguce izbaciti element sa vrednoscu %d jer ne postoji u listi."},
	[UPOZORENJE_LISTA_SORTIRAJ] = { Upozorenje, "Nije moguce sortirati listu jer ima manje od 2 elementa."},
	//greska poruke
	[GRESKA_UCITAVANJE_DATOTEKE] = { Greska, "Neuspesno ucitavanje datoteke."},
	[GRESKA_KREIRAJ] = { Greska, "Nije moguce kreirati datoteku."},
	[GRESKA_UNISTI] = { Greska, "Nije moguce unistiti listu."},
	[GRESKA_UBACI] = { Greska, "Nije moguce ubaciti element u listu."},
	[GRESKA_LISTA_NE_POSTOJI] = { Greska, "Nije moguce izvrsiti operaciju jer lista ne postoji."},
};

void prijavi(KOD_PORUKE kod, STRING naziv_datoteke, int linija) {
	if (kod >= BROJ_KODOVA) return;

	PORUKA poruka = poruke[kod];
	STRING tip_poruke;

	switch (poruka.status)
	{
	case Info: tip_poruke = "*** INFO: ";
		break;
	case Upozorenje: tip_poruke = "*** UPOZORENJE: ";
		break;
	case Greska: tip_poruke = "*** GRESKA: ";
		break;
	default: tip_poruke = "NEIDENTIFIKOVANO ";
		break;
	}

	if (poruka.status == Info) {
		fprintf(stdout, "%s %s\n", tip_poruke, poruka.tekst);
	}
	else {
		fprintf(stderr, "%s %s\nDATOTEKA: %s\nLINIJA: %d\n", tip_poruke, poruka.tekst, naziv_datoteke, linija);
		if (p.status == Greska) {
			fprintf(stderr, "Program ce biti prekinut.\n");
			exit(EXIT_FAILURE);
		}
	}
}
