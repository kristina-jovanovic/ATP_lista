#include "defs.h"
#include "lista.h"

// pomocne funkcije
static void zameni(POKAZIVAC, POKAZIVAC);

bool kreiraj(LISTA* lista) {
	*lista = malloc(sizeof(struct lista));
	(*lista)->skladiste = NULL;
	(*lista)->kapacitet = 100;
	(*lista)->broj_elemenata = 0;
	return true;
}

bool unisti(LISTA* lista) {
	if ((*lista == NULL) || ((*lista)->skladiste) == NULL || (*lista == ErrorList)) return false;
	POKAZIVAC trenutni = (POKAZIVAC)(*lista)->skladiste;
	while (trenutni != NULL) {
		(*lista)->skladiste = ((POKAZIVAC)((*lista)->skladiste))->sledeci;
		(*lista)->broj_elemenata--;
		trenutni->sledeci = NULL;
		free(trenutni);
		trenutni = (*lista)->skladiste;
	}
	return true;
}

bool ubaci_na_pocetak(LISTA* lista, PODATAK podatak) {
	if (*lista == ErrorList || (*lista)->broj_elemenata >= (*lista)->kapacitet) return false;
	POKAZIVAC novi = malloc(sizeof(ELEMENT));
	novi->prethodni = NULL; //jer je ovo JUL
	novi->podatak = podatak;
	novi->sledeci = (*lista)->skladiste;
	(*lista)->skladiste = novi;
	(*lista)->broj_elemenata++;
	return true;
}

bool izbaci_sa_pocetka(LISTA* lista, PODATAK* podatak) {
	if ((*lista == NULL) || ((*lista)->skladiste) == NULL || (*lista == ErrorList)) return false;
	*podatak = ((POKAZIVAC)((*lista)->skladiste))->podatak;
	POKAZIVAC pom = (*lista)->skladiste;
	(*lista)->skladiste = ((POKAZIVAC)((*lista)->skladiste))->sledeci;
	(*lista)->broj_elemenata--;
	free(pom);
	pom = NULL;
	return true;
}

void prikazi(LISTA lista) {
	printf("\n///// Lista: ");
	if ((lista == NULL) || (lista->skladiste == NULL) || (lista->skladiste == ErrorList)) {
		(lista == NULL || lista->skladiste == NULL) ? printf("< Null > \n") : printf("< ErrorList > \n");
		return;
	}
	POKAZIVAC trenutni = lista->skladiste; // mora ovako, ako se ide direktno preko lista->skladiste, promene tj. pomeranja ostaju vidljiva, tj. na kraju ce lista->skladiste biti NULL
	while (trenutni != NULL) {
		printf("%d  ", trenutni->podatak);
		trenutni = trenutni->sledeci;
	}
	printf("\n");
}

bool sortiraj(LISTA* lista) {
	if ((*lista == NULL) || ((*lista)->skladiste == NULL) || ((*lista)->skladiste == ErrorList)) return false;
	POKAZIVAC prvi = (POKAZIVAC)(*lista)->skladiste, drugi = ((POKAZIVAC)((*lista)->skladiste))->sledeci;
	while (prvi->sledeci != NULL) {
		while (drugi != NULL) {
			if (prvi->podatak > drugi->podatak)
				zameni(prvi, drugi);
			drugi = drugi->sledeci;
		}
		prvi = prvi->sledeci;
		drugi = prvi->sledeci;
	}
	return true;
}

bool prazna(LISTA lista) {
	return (lista == NULL || lista->skladiste == NULL) ? true : false;
}

bool sadrzi(LISTA lista, PODATAK trazeni_podatak) {
	if ((lista == NULL) || (lista->skladiste == NULL) || (lista->skladiste == ErrorList)) return false;
	POKAZIVAC trenutni = lista->skladiste;
	while (trenutni != NULL) {
		if (trazeni_podatak == trenutni->podatak) break;
		trenutni = trenutni->sledeci;
	}
	return (trenutni != NULL) ? true : false; // ako smo dosli do kraja (trenutni == NULL) znaci da nismo nasli trazeni podatak
}

// implementacija pomocnih funkcija

static void zameni(POKAZIVAC p1, POKAZIVAC p2) {
	PODATAK pom = p1->podatak;
	p1->podatak = p2->podatak;
	p2->podatak = pom;
}

//main

int main(void) {
	LISTA lista;
	kreiraj(&lista) ? printf("Lista je uspesno kreirana.\n") : printf("Lista nije uspesno kreirana.\n");

	int a = 5;
	ubaci_na_pocetak(&lista, a) ? printf("Podatak je uspesno dodat.\n") : printf("Podatak nije uspesno dodat.\n");
	int b = 9;
	ubaci_na_pocetak(&lista, b) ? printf("Podatak je uspesno dodat.\n") : printf("Podatak nije uspesno dodat.\n");

	prikazi(lista);
	sortiraj(&lista);
	prikazi(lista);

	sadrzi(lista, 5) ? printf("Lista sadrzi podatak %d\n", 5) : printf("Lista ne sadrzi podatak %d\n", 5);

	int izbaceni;
	izbaci_sa_pocetka(&lista, &izbaceni);
	printf("Izbacen: %d\n", izbaceni);
	prikazi(lista);
	sadrzi(lista, 5) ? printf("Lista sadrzi podatak %d\n", 5) : printf("Lista ne sadrzi podatak %d\n", 5);
	prazna(lista) ? printf("Lista je prazna\n") : printf("Lista nije prazna\n");

	sadrzi(lista, 15) ? printf("Lista sadrzi podatak %d\n", 15) : printf("Lista ne sadrzi podatak %d\n", 15);

	unisti(&lista);
	prikazi(lista);
	prazna(lista) ? printf("Lista je prazna\n") : printf("Lista nije prazna\n");

}