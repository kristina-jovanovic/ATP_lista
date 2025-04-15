#include "defs.h"
#include "lista.h"

// ono sto je bilo POKAZIVAC zamenili smo sa ELEMENT*
// tehnicki je ostalo isto jer je POKAZIVAC bio definisan kao ELEMENT* a sad je void*

// pomocne funkcije
static void zameni(ELEMENT*, ELEMENT*);

/////////
//status
PORUKA poruka = {
	.GRESKA = {
		.kreiraj = "Doslo je do velikih problema prilikom KREIRANJA liste.\n",
		.unisti = "Doslo je do velikih problema prilikom UNISTAVANJA liste.\n",
		.ubaci = "Doslo je do velikih problema prilikom UBACIVANJA elementa u listu.\n",
		.izbaci = "Doslo je do velikih problema prilikom IZBACIVANJA elementa iz liste.\n",
		.sortiraj = "Doslo je do velikih problema prilikom SORTIRANJA elemenata liste.\n",
	},
	.UPOZORENJE = {
		.ubaci = "Doslo je do problema prilikom UBACIVANJA elementa u listu.\n",
		.izbaci = "Doslo je do problema prilikom IZBACIVANJA elementa iz liste.\n",
		.sortiraj = "Doslo je do problema prilikom SORTIRANJA elemenata liste.\n",
	},
	.INFO = {
		.kreiraj = "Lista je uspesno kreirana.\n",
		.unisti = "Lista je uspesno unistena.\n",
		.ubaci = "Element je uspesno ubacen u listu.\n",
		.izbaci = "Element je uspesno izbacen iz liste.\n",
		.podatak_postoji = "Podatak postoji u listi.\n",
		.podatak_ne_postoji = "Podatak ne postoji u listi.\n",
		.lista_prazna = "Lista je prazna.\n",
		.lista_nije_prazna = "Lista nije prazna.\n",
		.sortiraj = "Lista je uspesno sortirana.\n",
	}
};

void obrada_statusa(STATUS status, STRING poruka, STRING naziv_dat, int linija_koda) {
	char tip_poruke[20];
	switch (status) {
	case Info:
		strcpy(tip_poruke, "*** INFO:");
		fprintf(stdout, "%s %s", tip_poruke, poruka);
		break;
	case Upozorenje:
		strcpy(tip_poruke, "*** UPOZORENJE:");
		fprintf(stderr, "%s %s\nDATOTEKA: %s\nLINIJA: %d", tip_poruke, poruka, naziv_dat, linija_koda);
		break;
	case Greska:
		strcpy(tip_poruke, "*** GRESKA:");
		fprintf(stderr, "%s %s\nDATOTEKA: %s\nLINIJA: %d", tip_poruke, poruka, naziv_dat, linija_koda);
		fprintf(stderr, "\nProgram ce biti prekinut.");
		exit(EXIT_FAILURE);
		break;
	default:
		break;
	}
}
/////////

SIGNAL kreiraj(LISTA* lista) {
	SIGNAL signal;
	*lista = malloc(sizeof(struct lista));
	if (*lista == NULL) {
		signal.status = Greska;
		signal.poruka = poruka.GRESKA.kreiraj;
	}
	else {
		(*lista)->skladiste = NULL;
		(*lista)->kapacitet = 100;
		(*lista)->broj_elemenata = 0;

		signal.status = Info;
		signal.poruka = poruka.INFO.kreiraj;
	}
	return signal;
}

SIGNAL unisti(LISTA* lista) {
	SIGNAL signal;
	if ((*lista == NULL) || ((*lista)->skladiste) == NULL || (*lista == ErrorList)) {
		signal.status = Greska;
		signal.poruka = poruka.GRESKA.unisti;
		return signal;
	}
	ELEMENT* trenutni = (ELEMENT*)(*lista)->skladiste;
	if (trenutni == NULL) {
		signal.status = Greska;
		signal.poruka = poruka.GRESKA.unisti;
		return signal;
	}
	while (trenutni != NULL) {
		(*lista)->skladiste = ((ELEMENT*)((*lista)->skladiste))->sledeci;
		(*lista)->broj_elemenata--;
		trenutni->sledeci = NULL;
		//free(trenutni);
		trenutni = (*lista)->skladiste;
	}
	signal.status = Info;
	signal.poruka = poruka.INFO.unisti;
	return signal;
}

SIGNAL ubaci(LISTA* lista, PODATAK podatak, NACIN nacin) {
	SIGNAL signal;
	signal.status = Greska;
	signal.poruka = poruka.GRESKA.ubaci;
	if (*lista == NULL || *lista == ErrorList || (*lista)->broj_elemenata >= (*lista)->kapacitet) return signal;

	ELEMENT* novi = malloc(sizeof(ELEMENT));
	if (novi == NULL) return signal;
	novi->prethodni = NULL; //jer je ovo JUL
	novi->podatak = podatak;

	ELEMENT* glava = (ELEMENT*)((*lista)->skladiste);
	if (nacin == Vrednost) {
		//lista treba da bude sortirana i da se element ubaci tamo gde pripada po vrednosti
		sortiraj(lista, Rastuce);
		//signal = sortiraj(lista, Rastuce);
		//if (signal.status == Greska) return signal;
		if (glava == NULL)
			goto pocetak;
		else {
			ELEMENT* prethodni = glava;
			ELEMENT* trenutni = glava->sledeci;
			while (trenutni != NULL) {
				if (trenutni->podatak > podatak) break;
				trenutni = trenutni->sledeci;
				prethodni = trenutni->sledeci;
			}
			//postaviti novi element izmedju 'prethodnog' i 'trenutnog'
			prethodni->sledeci = novi;
			novi->sledeci = trenutni;
		}
	}
	if (nacin == Kraj) {
		novi->sledeci = NULL; // novi ide na kraj
		if (glava == NULL) {
			//lista je prazna, dodajemo prvi element, isto je kao da ga dodajemo na pocetak
			goto pocetak;
		}
		else {
			while (glava->sledeci != NULL) {
				glava = glava->sledeci;
			}
			glava->sledeci = novi;
		}
	}
	if (nacin == Pocetak) {
	pocetak:
		novi->sledeci = (*lista)->skladiste;
		(*lista)->skladiste = (void*)novi;
	}

	(*lista)->broj_elemenata++;
	signal.status = Info;
	signal.poruka = poruka.INFO.ubaci;
	return signal;
}

SIGNAL izbaci_sa_pocetka(LISTA* lista, PODATAK* podatak) {
	SIGNAL signal;
	signal.status = Greska;
	signal.poruka = poruka.GRESKA.izbaci;
	if (*lista == NULL || (*lista)->skladiste == NULL || *lista == ErrorList) return signal;
	*podatak = ((ELEMENT*)((*lista)->skladiste))->podatak;
	ELEMENT* pom = (*lista)->skladiste;
	(*lista)->skladiste = ((ELEMENT*)((*lista)->skladiste))->sledeci;
	(*lista)->broj_elemenata--;
	free(pom);
	pom = NULL;

	signal.status = Info;
	signal.poruka = poruka.INFO.izbaci;
	return signal;
}

SIGNAL izbaci(LISTA* lista, PODATAK* podatak, NACIN nacin) {
	SIGNAL signal;
	signal.status = Greska;
	signal.poruka = poruka.GRESKA.izbaci;
	if (*lista == NULL || (*lista)->skladiste == NULL || *lista == ErrorList) return signal;
	if ((*lista)->broj_elemenata == 0) {
		signal.status = Upozorenje;
		signal.poruka = poruka.UPOZORENJE.izbaci;
		return signal;
	}

	ELEMENT* pom = (*lista)->skladiste;

	if (nacin == Vrednost) {
		//brisemo element sa vrednoscu podatka prosledjenog parametrom
		if (((ELEMENT*)((*lista)->skladiste))->podatak == *podatak)
			goto pocetak;
		else {
			ELEMENT* prethodni = pom;
			pom = pom->sledeci;
			while (pom != NULL) {
				if (pom->podatak == *podatak)
					break;
				pom = pom->sledeci;
				prethodni = prethodni->sledeci;
			}
			if (pom == NULL) {
				//znaci nismo pronasli taj element
				goto kraj_false;
			}
			else {
				prethodni->sledeci = pom->sledeci; //znaci brisemo pom
				pom = NULL;
				free(pom);
				goto kraj_true;
			}
		}
	}
	if (nacin == Kraj) {
		if ((*lista)->broj_elemenata == 1) {
			*podatak = ((ELEMENT*)((*lista)->skladiste))->podatak;
			goto pocetak; // kao da brisemo sa pocetka ako imamo samo 1 element
		}
		else {
			while (((ELEMENT*)(pom->sledeci))->sledeci != NULL) {
				//stajemo na pretposlednjem elementu
				pom = pom->sledeci;
			}
			*podatak = ((ELEMENT*)pom->sledeci)->podatak;
			pom->sledeci = NULL; // brisemo vezu ka poslednjem cvoru sa pretposlednjeg
			free(pom->sledeci);
			goto kraj_true;
		}
	}
	if (nacin == Pocetak) {
		*podatak = ((ELEMENT*)((*lista)->skladiste))->podatak;
	pocetak:
		(*lista)->skladiste = ((ELEMENT*)((*lista)->skladiste))->sledeci;
		free(pom);
		pom = NULL;
		goto kraj_true;
	}

kraj_true:
	(*lista)->broj_elemenata--;
	if ((*lista)->broj_elemenata == 0) (*lista)->skladiste == NULL;
	signal.status = Info;
	signal.poruka = poruka.INFO.izbaci;
	return signal;
kraj_false:
	signal.status = Upozorenje;
	signal.poruka = poruka.UPOZORENJE.izbaci;
	return signal;
}

void prikazi(LISTA lista) {
	printf("\n///// Lista: ");
	if ((lista == NULL) || (lista->skladiste == NULL) || (lista->skladiste == ErrorList)) {
		(lista == NULL || lista->skladiste == NULL) ? printf("< Null > \n") : printf("< ErrorList > \n");
		return;
	}
	if (lista->broj_elemenata == 0) {
		printf("< Prazna >\n");
		return;
	}
	ELEMENT* trenutni = lista->skladiste; // mora ovako, ako se ide direktno preko lista->skladiste, promene tj. pomeranja ostaju vidljiva, tj. na kraju ce lista->skladiste biti NULL
	while (trenutni != NULL) {
		printf("%d  ", trenutni->podatak);
		trenutni = trenutni->sledeci;
	}
	printf("\n\n");
}

SIGNAL sortiraj(LISTA* lista, SMER_SORTIRANJA smer) {
	SIGNAL signal;
	signal.status = Greska;
	signal.poruka = poruka.GRESKA.sortiraj;
	if (*lista == NULL || (*lista)->skladiste == NULL || (*lista)->skladiste == ErrorList) return signal;
	ELEMENT* prvi = (ELEMENT*)(*lista)->skladiste;
	ELEMENT* drugi = ((ELEMENT*)(*lista)->skladiste)->sledeci;
	if (prvi == NULL || drugi == NULL) {
		signal.status = Upozorenje;
		signal.poruka = poruka.UPOZORENJE.sortiraj;
		return signal;
	}
	while (prvi->sledeci != NULL) {
		while (drugi != NULL) {
			if (smer == Rastuce && (prvi->podatak > drugi->podatak))
				zameni(prvi, drugi);
			if (smer == Opadajuce && (prvi->podatak < drugi->podatak))
				zameni(prvi, drugi);
			drugi = drugi->sledeci;
		}
		prvi = prvi->sledeci;
		drugi = prvi->sledeci;
	}

	signal.status = Info;
	signal.poruka = poruka.INFO.sortiraj;
	return signal;
}

SIGNAL prazna(LISTA lista) {
	SIGNAL signal;
	signal.status = Info;
	if (lista == NULL || lista->skladiste == NULL || lista->broj_elemenata == 0)
		signal.poruka = poruka.INFO.lista_prazna;
	else
		signal.poruka = poruka.INFO.lista_nije_prazna;
	return signal;
}

SIGNAL sadrzi(LISTA lista, PODATAK trazeni_podatak) {
	SIGNAL signal;
	signal.status = Info;
	signal.poruka = poruka.INFO.podatak_ne_postoji;
	if (lista == NULL || lista->skladiste == NULL || lista->skladiste == ErrorList
		|| lista->broj_elemenata == 0) return signal;
	ELEMENT* trenutni = lista->skladiste;
	while (trenutni != NULL) {
		if (trazeni_podatak == trenutni->podatak) break;
		trenutni = trenutni->sledeci;
	}
	(trenutni != NULL) ? (signal.poruka = poruka.INFO.podatak_postoji) : (signal.poruka = poruka.INFO.podatak_ne_postoji); // ako smo dosli do kraja (trenutni == NULL) znaci da nismo nasli trazeni podatak
	return signal;
}

// implementacija pomocnih funkcija

static void zameni(ELEMENT* p1, ELEMENT* p2) {
	PODATAK pom = p1->podatak;
	p1->podatak = p2->podatak;
	p2->podatak = pom;
}

//main

int main(void) {
	LISTA lista;
	SIGNAL signal;
	signal = kreiraj(&lista);
	obrada_statusa(signal.status, signal.poruka, NULL, __LINE__);

	int a = 5;
	signal = ubaci(&lista, a, Kraj);
	obrada_statusa(signal.status, signal.poruka, NULL, __LINE__);

	int b = 9;
	signal = ubaci(&lista, b, Pocetak);
	obrada_statusa(signal.status, signal.poruka, NULL, __LINE__);

	int c = 7;
	signal = ubaci(&lista, c, Vrednost);
	obrada_statusa(signal.status, signal.poruka, NULL, __LINE__);

	prikazi(lista);
	signal = sortiraj(&lista, Opadajuce);
	obrada_statusa(signal.status, signal.poruka, NULL, __LINE__);
	prikazi(lista);

	signal = sadrzi(lista, 5);
	obrada_statusa(signal.status, signal.poruka, NULL, __LINE__);

	int izbaceni = 7;
	signal = izbaci(&lista, &izbaceni, Vrednost);
	obrada_statusa(signal.status, signal.poruka, NULL, __LINE__);
	printf("Izbacen: %d\n", izbaceni);

	/*int izbaceni;
	signal = izbaci(&lista, &izbaceni, Kraj);
	obrada_statusa(signal.status, signal.poruka, NULL, __LINE__);
	printf("Izbacen: %d\n", izbaceni);
	signal = izbaci(&lista, &izbaceni, Kraj);
	obrada_statusa(signal.status, signal.poruka, NULL, __LINE__);
	printf("Izbacen: %d\n", izbaceni);
	signal = izbaci(&lista, &izbaceni, Kraj);
	obrada_statusa(signal.status, signal.poruka, NULL, __LINE__);
	printf("Izbacen: %d\n", izbaceni);
	signal = izbaci(&lista, &izbaceni, Kraj);
	obrada_statusa(signal.status, signal.poruka, NULL, __LINE__);
	printf("Izbacen: %d\n", izbaceni);*/

	prikazi(lista);

	signal = sadrzi(lista, 5);
	obrada_statusa(signal.status, signal.poruka, NULL, __LINE__);

	signal = prazna(lista);
	obrada_statusa(signal.status, signal.poruka, NULL, __LINE__);

	signal = sadrzi(lista, 15);
	obrada_statusa(signal.status, signal.poruka, NULL, __LINE__);

	signal = unisti(&lista);
	obrada_statusa(signal.status, signal.poruka, NULL, __LINE__);

	prikazi(lista);
	signal = prazna(lista);
	obrada_statusa(signal.status, signal.poruka, NULL, __LINE__);

	return 0;
}