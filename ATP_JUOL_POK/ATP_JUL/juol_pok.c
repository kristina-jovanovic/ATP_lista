
#include "lista.h"

// ono sto je bilo POKAZIVAC zamenili smo sa ELEMENT*
// tehnicki je ostalo isto jer je POKAZIVAC bio definisan kao ELEMENT* a sad je void*

// pomocne funkcije
static void zameni(ELEMENT*, ELEMENT*);
void bubble_sort(LISTA, SMER_SORTIRANJA);
void insertion_sort(LISTA, SMER_SORTIRANJA);
void selection_sort(LISTA, SMER_SORTIRANJA);

void kreiraj(LISTA* lista) {
	*lista = malloc(sizeof(struct lista));
	if (*lista == NULL) {
		PRIJAVI(Kod.Greska.Kreiraj);
	}
	else {
		(*lista)->skladiste = NULL;
		(*lista)->kapacitet = 100;
		(*lista)->broj_elemenata = 0;

		PRIJAVI(Kod.Info.Kreiraj);
	}
}

void unisti(LISTA lista) {
	if ((lista == NULL) || (lista->skladiste) == NULL || (lista == ErrorList)) {
		PRIJAVI(Kod.Greska.Lista_ne_postoji);
		return;
	}
	ELEMENT* trenutni = (ELEMENT*)lista->skladiste;
	if (trenutni == NULL) {
		PRIJAVI(Kod.Greska.Unisti);
		return;
	}
	while (trenutni != NULL) {
		lista->skladiste = ((ELEMENT*)(lista->skladiste))->sledeci;
		lista->broj_elemenata--;
		trenutni->sledeci = NULL;
		//free(trenutni);
		trenutni = lista->skladiste;
	}
	PRIJAVI(Kod.Info.Unisti);
}

void ubaci(LISTA lista, PODATAK podatak, NACIN nacin) {
	if (lista == NULL || lista == ErrorList) {
		PRIJAVI(Kod.Greska.Lista_ne_postoji);
		return;
	}
	if (lista->broj_elemenata >= lista->kapacitet) {
		PRIJAVI(Kod.Upozorenje.Ubaci);
		return;
	}

	ELEMENT* novi = malloc(sizeof(ELEMENT));
	novi->prethodni = NULL; //jer je ovo JUL
	novi->podatak = podatak;

	ELEMENT* glava = (ELEMENT*)(lista->skladiste);
	if (nacin == Vrednost) {
		//lista treba da bude sortirana i da se element ubaci tamo gde pripada po vrednosti
		sortiraj(lista, Rastuce, Bubble);
		//poruka = sortiraj(lista, Rastuce);
		//if (poruka.status == Greska) return poruka;
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
		novi->sledeci = lista->skladiste;
		lista->skladiste = (void*)novi;
	}

	lista->broj_elemenata++;
	PRIJAVI(Kod.Info.Ubaci);
}

void izbaci(LISTA lista, PODATAK* podatak, NACIN nacin) {
	if (lista == NULL || lista == ErrorList) {
		PRIJAVI(Kod.Greska.Lista_ne_postoji);
		return;
	}
	if (lista->skladiste == NULL || lista->broj_elemenata == 0) {
		PRIJAVI(Kod.Upozorenje.Lista_prazna);
	}

	ELEMENT* pom = lista->skladiste;

	if (nacin == Vrednost) {
		//brisemo element sa vrednoscu podatka prosledjenog parametrom
		if (((ELEMENT*)(lista->skladiste))->podatak == *podatak)
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
		if (lista->broj_elemenata == 1) {
			*podatak = ((ELEMENT*)(lista->skladiste))->podatak;
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
		*podatak = ((ELEMENT*)(lista->skladiste))->podatak;
	pocetak:
		lista->skladiste = ((ELEMENT*)(lista->skladiste))->sledeci;
		free(pom);
		pom = NULL;
		goto kraj_true;
	}

kraj_true:
	lista->broj_elemenata--;
	if (lista->broj_elemenata == 0) lista->skladiste = NULL;
	PRIJAVI(Kod.Info.Izbaci, *podatak);
	return;
kraj_false:
	PRIJAVI(Kod.Upozorenje.Izbaci, *podatak);
	return;
}

void prikazi(LISTA lista) {
	wprintf(L"\nЛиста: ");
	if ((lista == NULL) || (lista->skladiste == NULL) || (lista->skladiste == ErrorList)) {
		(lista == NULL || lista->skladiste == NULL) ? wprintf(L"< Null > \n\n") : wprintf(L"< ErrorList > \n\n");
		return;
	}
	if (lista->broj_elemenata == 0) {
		wprintf(L"< Празна >\n\n");
		return;
	}
	ELEMENT* trenutni = lista->skladiste; // mora ovako, ako se ide direktno preko lista->skladiste, promene tj. pomeranja ostaju vidljiva, tj. na kraju ce lista->skladiste biti NULL
	while (trenutni != NULL) {
		wprintf(L"%d  ", trenutni->podatak);
		trenutni = trenutni->sledeci;
	}
	wprintf(L"\n\n");
}

void sortiraj(LISTA lista, SMER_SORTIRANJA smer, ALGORITAM_SORTIRANJA algoritam) {
	if (lista == NULL || lista->skladiste == ErrorList) {
		PRIJAVI(Kod.Greska.Lista_ne_postoji);
		return;
	}
	if (lista->skladiste == NULL || lista->broj_elemenata == 0) {
		PRIJAVI(Kod.Upozorenje.Lista_prazna);
		return;
	}

	if (algoritam == Bubble)
		bubble_sort(lista, smer);
	if (algoritam == Insertion)
		insertion_sort(lista, smer);
	if (algoritam == Selection)
		selection_sort(lista, smer);
}

bool prazna(LISTA lista) {
	if (lista == NULL || lista->skladiste == NULL || lista->broj_elemenata == 0) {
		PRIJAVI(Kod.Info.Lista_prazna);
		return true;
	}
	else {
		PRIJAVI(Kod.Info.Lista_nije_prazna);
		return false;
	}
}

bool sadrzi(LISTA lista, PODATAK trazeni_podatak, VRSTA_PRETRAGE vrsta_pretrage) {
	if (lista == NULL || lista->skladiste == NULL || lista->skladiste == ErrorList
		|| lista->broj_elemenata == 0) {
		PRIJAVI(Kod.Info.Podatak_ne_postoji, trazeni_podatak);
		return false;
	}

	//binarno pretrazivanje se ne moze raditi kada je lista implementirana preko pokazivaca
	//tako da cemo raditi iterativni pristup bez obzira na to koja vrsta pretrage je prosledjena

	ELEMENT* trenutni = lista->skladiste;
	while (trenutni != NULL) {
		if (trazeni_podatak == trenutni->podatak) break;
		trenutni = trenutni->sledeci;
	}
	if (trenutni != NULL) { // ako smo dosli do kraja (trenutni == NULL) znaci da nismo nasli trazeni podatak
		PRIJAVI(Kod.Info.Podatak_postoji, trazeni_podatak);
		return true;
	}
	else {
		PRIJAVI(Kod.Info.Podatak_ne_postoji, trazeni_podatak);
		return false;
	}
}

// implementacija pomocnih funkcija

static void zameni(ELEMENT* p1, ELEMENT* p2) {
	PODATAK pom = p1->podatak;
	p1->podatak = p2->podatak;
	p2->podatak = pom;
}

void bubble_sort(LISTA lista, SMER_SORTIRANJA smer) {
	ELEMENT* prvi = (ELEMENT*)lista->skladiste;
	ELEMENT* drugi = ((ELEMENT*)lista->skladiste)->sledeci;
	if (prvi == NULL || drugi == NULL) {
		PRIJAVI(Kod.Upozorenje.Sortiraj);
		return;
	}
	while (prvi->sledeci != NULL) {
		while (drugi != NULL) {
			if ((smer == Rastuce) && (prvi->podatak > drugi->podatak))
				zameni(prvi, drugi);
			if ((smer == Opadajuce) && (prvi->podatak < drugi->podatak))
				zameni(prvi, drugi);
			drugi = drugi->sledeci;
		}
		prvi = prvi->sledeci;
		drugi = prvi->sledeci;
	}
	PRIJAVI(Kod.Info.Sortiraj, (smer == Rastuce ? L"растуће" : L"опадајуће"));
}

void insertion_sort(LISTA lista, SMER_SORTIRANJA smer) {
	if (lista->broj_elemenata < 2) {
		//ako imamo 0 ili 1 element, nema sta da se sortira
		PRIJAVI(Kod.Upozorenje.Sortiraj);
		return;
	}

	ELEMENT* sortirano = NULL;
	ELEMENT* trenutni = lista->skladiste;

	while (trenutni) {
		ELEMENT* sledeci = (ELEMENT*)trenutni->sledeci;

		if (!sortirano || ((smer == Rastuce && sortirano->podatak > trenutni->podatak) ||
			(smer == Opadajuce && sortirano->podatak < trenutni->podatak))) {
			trenutni->sledeci = sortirano;
			sortirano = trenutni;
		}
		else {
			ELEMENT* pom = sortirano;
			while (pom->sledeci &&
				((smer == Rastuce && ((ELEMENT*)(pom->sledeci))->podatak <= trenutni->podatak) ||
					(smer == Opadajuce && ((ELEMENT*)(pom->sledeci))->podatak >= trenutni->podatak)))
				pom = pom->sledeci;

			trenutni->sledeci = pom->sledeci;
			pom->sledeci = trenutni;
		}

		trenutni = sledeci;
	}
	lista->skladiste = sortirano;
	PRIJAVI(Kod.Info.Sortiraj, (smer == Rastuce ? L"растуће" : L"опадајуће"));
}

void selection_sort(LISTA lista, SMER_SORTIRANJA smer) {
	if (lista->broj_elemenata < 2) {
		//ako imamo 0 ili 1 element, nema sta da se sortira
		PRIJAVI(Kod.Upozorenje.Sortiraj);
		return;
	}

	for (ELEMENT* i = (ELEMENT*)lista->skladiste; i != NULL; i = i->sledeci) {
		ELEMENT* pom = i; //ako sortiramo rastuce, ovo je minimum, a ako sortiramo opadajuce, ovo je maksimum
		for (ELEMENT* j = (ELEMENT*)i->sledeci; j != NULL; j = j->sledeci) {
			if ((smer == Rastuce && j->podatak < pom->podatak) ||
				(smer == Opadajuce && j->podatak > pom->podatak))
				pom = j;
		}
		if (pom != i) {
			int tmp = i->podatak;
			i->podatak = pom->podatak;
			pom->podatak = tmp;
		}
	}
	PRIJAVI(Kod.Info.Sortiraj, (smer == Rastuce ? L"растуће" : L"опадајуће"));
}

//main

//int main(void) {
//
//#pragma region Liste
//	_setmode(_fileno(stdout), _O_U8TEXT); // neophodno za ispis na cirilici 
//	_setmode(_fileno(stderr), _O_U8TEXT); // neophodno za ispis na cirilici 
//	setlocale(LC_ALL, "");
//	//+ moraju da se koriste wide funckije - wprintf() i slicne 
//
//	//LISTA lista;
//	//kreiraj(&lista);
//
//	//int a = 5;
//	//ubaci(lista, a, Kraj);
//
//	//int b = 9;
//	//ubaci(lista, b, Pocetak);
//
//	//int c = 7;
//	//ubaci(lista, c, Kraj);
//
//	//prikazi(lista);
//	//sortiraj(lista, Rastuce, Selection);
//	//prikazi(lista);
//
//	//sadrzi(lista, 5, Binarno);
//
//	//int izbaceni = 5;
//	//izbaci(lista, &izbaceni, Vrednost);
//
//	//prikazi(lista);
//
//	//sadrzi(lista, 5, Binarno);
//
//	//prazna(lista);
//
//	//sadrzi(lista, 15, Binarno);
//
//	//unisti(lista);
//
//	//prikazi(lista);
//	//prazna(lista);
//
//#pragma endregion
//
//
//	/*MENI meni;
//	kreiraj_meni(&meni, "../../meni.txt");
//	prikazi_meni(meni);
//	obradi_opciju(meni, 1);*/
//
//	return 0;
//}