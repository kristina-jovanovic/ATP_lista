
#include "lista.h"
#define DIM 100
#define PRAZNO -1

//svaki red matrice ima prethodni, podatak i sledeci (3 kolone)
#define PRETHODNI(matrica,i) (((int(*)[3])(matrica))[i][0]) //matricu kastuj u (int(*)[3]) i uzmi [i][0]
#define PODATAK_MAT(matrica,i) (((int(*)[3])(matrica))[i][1])
#define SLEDECI(matrica,i) (((int(*)[3])(matrica))[i][2])

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

//prototipovi pomocnih funkcija
void rotiraj_udesno(int[][3], int, int);
void rotiraj_ulevo(int[][3], int, int);
SIGNAL bubble_sort(LISTA*, SMER_SORTIRANJA);
SIGNAL insertion_sort(LISTA*, SMER_SORTIRANJA);
SIGNAL selection_sort(LISTA*, SMER_SORTIRANJA);

SIGNAL kreiraj(LISTA* lista) {
	SIGNAL signal;
	signal.status = Greska;
	signal.poruka = poruka.GRESKA.kreiraj;
	*lista = malloc(sizeof(struct lista));
	if (*lista == NULL) return signal;
	(*lista)->skladiste = malloc(sizeof(int[DIM][3]));
	if ((*lista)->skladiste == NULL) return signal;
	//memset((*lista)->skladiste, PRAZNO, sizeof(int[DIM][3]));
	(*lista)->kapacitet = DIM;
	(*lista)->broj_elemenata = 0;

	signal.status = Info;
	signal.poruka = poruka.INFO.kreiraj;
	return signal;
}

SIGNAL unisti(LISTA* lista) {
	SIGNAL signal;
	signal.status = Greska;
	signal.poruka = poruka.GRESKA.unisti;
	if (*lista == NULL || (*lista)->skladiste == NULL || (*lista)->broj_elemenata == 0)
		return signal; // lista je prazna ili ne postoji

	free((*lista)->skladiste);
	(*lista)->skladiste = NULL;
	free(*lista);
	*lista = NULL;

	signal.status = Info;
	signal.poruka = poruka.INFO.unisti;
	return signal;
}

SIGNAL ubaci_na_pocetak(LISTA* lista, PODATAK podatak) {
	SIGNAL signal;
	signal.status = Greska;
	signal.poruka = poruka.GRESKA.ubaci;
	if (*lista == NULL || (*lista)->skladiste == NULL || (*lista)->broj_elemenata >= (*lista)->kapacitet)
		return signal;

	int(*matrica)[3] = (int(*)[3])(*lista)->skladiste;
	int novi_indeks = 0;

	if ((*lista)->broj_elemenata > 0) {
		//lista nije prazna
		rotiraj_udesno(matrica, (*lista)->broj_elemenata, 0);
		PRETHODNI(matrica, novi_indeks) = PRAZNO;
		PODATAK_MAT(matrica, novi_indeks) = podatak;
		int stara_glava_indeks = 1; //prvi element koji je pomeren udesno
		PRETHODNI(matrica, stara_glava_indeks) = novi_indeks;
		SLEDECI(matrica, novi_indeks) = stara_glava_indeks;
	}
	else {
		//lista je prazna, dodajemo prvi element
		PRETHODNI(matrica, novi_indeks) = PRAZNO;
		PODATAK_MAT(matrica, novi_indeks) = podatak;
		SLEDECI(matrica, novi_indeks) = PRAZNO;
	}

	(*lista)->broj_elemenata++;
	signal.status = Info;
	signal.poruka = poruka.INFO.ubaci;
	return signal;
}

SIGNAL ubaci(LISTA* lista, PODATAK podatak, NACIN nacin) {
	SIGNAL signal;
	signal.status = Greska;
	signal.poruka = poruka.GRESKA.ubaci;
	if (*lista == NULL || (*lista)->skladiste == NULL || (*lista)->broj_elemenata >= (*lista)->kapacitet)
		return signal;
	int(*matrica)[3] = (int(*)[3])(*lista)->skladiste;
	int novi_indeks;

	if (nacin == Vrednost) {
		//lista mora da bude sortirana
		sortiraj(lista, Rastuce, Bubble);
		//trazimo gde prosledjeni podatak pripada
		int trenutni_red = 0; //uzimamo ceo prvi red
		do {
			if (matrica[trenutni_red][1] > podatak) { // drugi element u redu je podatak
				novi_indeks = trenutni_red;
				break;
			}
			trenutni_red = matrica[trenutni_red][2]; // pomeramo se na sledeci
		} while (trenutni_red != PRAZNO);
		if (trenutni_red == PRAZNO) {
			goto kraj;
		}
		else {
			//treba da se rotira udesno od novog_indeksa do kraja, da se oslobodi mesto
			rotiraj_udesno(matrica, (*lista)->broj_elemenata, novi_indeks);

			SLEDECI(matrica, novi_indeks - 1) = novi_indeks;
			PRETHODNI(matrica, novi_indeks + 1) = novi_indeks;

			PRETHODNI(matrica, novi_indeks) = novi_indeks - 1;
			SLEDECI(matrica, novi_indeks) = novi_indeks + 1;
			PODATAK_MAT(matrica, novi_indeks) = podatak;
		}
	}
	if (nacin == Kraj) {
	kraj:
		novi_indeks = (*lista)->broj_elemenata;
		if (novi_indeks == 0) {
			goto pocetak;
		}
		PRETHODNI(matrica, novi_indeks) = novi_indeks - 1;
		PODATAK_MAT(matrica, novi_indeks) = podatak;
		SLEDECI(matrica, novi_indeks) = PRAZNO;
		SLEDECI(matrica, novi_indeks - 1) = novi_indeks;
		goto kraj_true;
	}
	if (nacin == Pocetak) {
		novi_indeks = 0;
		if ((*lista)->broj_elemenata > 0) {
			//lista nije prazna
			rotiraj_udesno(matrica, (*lista)->broj_elemenata, 0);
			PRETHODNI(matrica, novi_indeks) = PRAZNO;
			PODATAK_MAT(matrica, novi_indeks) = podatak;
			int stara_glava_indeks = 1; //prvi element koji je pomeren udesno
			PRETHODNI(matrica, stara_glava_indeks) = novi_indeks;
			SLEDECI(matrica, novi_indeks) = stara_glava_indeks;
		}
		else {
		pocetak:
			//lista je prazna, dodajemo prvi element
			PRETHODNI(matrica, novi_indeks) = PRAZNO;
			PODATAK_MAT(matrica, novi_indeks) = podatak;
			SLEDECI(matrica, novi_indeks) = PRAZNO;
		}
		goto kraj_true;
	}

kraj_true:
	(*lista)->broj_elemenata++;
	signal.status = Info;
	signal.poruka = poruka.INFO.ubaci;
	return signal;
}

SIGNAL izbaci(LISTA* lista, PODATAK* podatak, NACIN nacin) {
	SIGNAL signal;
	signal.status = Greska;
	signal.poruka = poruka.GRESKA.izbaci;
	if (*lista == NULL || (*lista)->skladiste == NULL || (*lista)->broj_elemenata == 0)
		return signal;

	int(*matrica)[3] = (int(*)[3])(*lista)->skladiste;
	int broj_el = (*lista)->broj_elemenata;

	if (nacin == Vrednost) {
		for (int i = 0;i < broj_el;i++) {
			int pod = PODATAK_MAT(matrica, i);
			if (pod == *podatak) {

				// ako je ovo poslednji element, idi u granu 'kraj'
				if (i == broj_el - 1) {
					goto kraj;
				}

				// 'brisemo' i-ti
				PRETHODNI(matrica, i) = PRAZNO;
				SLEDECI(matrica, i) = PRAZNO;
				PODATAK_MAT(matrica, i) = PRAZNO;

				// rotiramo ulevo od i-tog
				rotiraj_ulevo(matrica, broj_el, i);

				goto kraj_true;
			}
		}
	}
	if (nacin == Kraj) {
	kraj:
		if (broj_el == 1) {
			goto pocetak;
		}

		*podatak = matrica[broj_el - 1][1]; //ovaj element izbacujemo
		matrica[broj_el - 1][1] = PRAZNO;
		matrica[broj_el - 1][2] = PRAZNO;

		int prethodni = matrica[broj_el - 1][0];
		matrica[prethodni][2] = PRAZNO; //sledeci od prethodnog resetujemo na 'prazno'

		//kad brisemo sa kraja nema sta da rotiramo ulevo, samo iskljucimo poslednji element
		goto kraj_true;
	}
	if (nacin == Pocetak) {
	pocetak:
		*podatak = matrica[0][1]; //ovaj element izbacujemo
		matrica[0][0] = PRAZNO;
		matrica[0][1] = PRAZNO;
		int nova_glava = matrica[0][2];
		matrica[0][2] = PRAZNO;

		if (broj_el > 1) {
			matrica[nova_glava][0] = PRAZNO;
			rotiraj_ulevo(matrica, broj_el, 0);
		}
		goto kraj_true;
	}

kraj_false: // ovde stizemo jedino ako je prosledjen nacin 'Vrednost', a ta vrednost ne postoji u listi
	signal.status = Upozorenje;
	signal.poruka = poruka.UPOZORENJE.izbaci;
	return signal;
kraj_true:
	(*lista)->broj_elemenata--;
	signal.status = Info;
	signal.poruka = poruka.INFO.izbaci;
	return signal;
}

SIGNAL izbaci_sa_pocetka(LISTA* lista, PODATAK* podatak) {
	SIGNAL signal;
	signal.status = Greska;
	signal.poruka = poruka.GRESKA.izbaci;
	if (*lista == NULL || (*lista)->skladiste == NULL || (*lista)->broj_elemenata == 0)
		return signal;

	int(*matrica)[3] = (int(*)[3])(*lista)->skladiste;
	*podatak = matrica[0][1]; //ovaj element izbacujemo
	matrica[0][0] = PRAZNO;
	matrica[0][1] = PRAZNO;

	int nova_glava = matrica[0][2];
	matrica[0][2] = PRAZNO;
	matrica[nova_glava][0] = PRAZNO;

	rotiraj_ulevo(matrica, (*lista)->broj_elemenata, 0);
	(*lista)->broj_elemenata--;
	signal.status = Info;
	signal.poruka = poruka.INFO.izbaci;
	return signal;
}

void prikazi(LISTA lista) {
	printf("\nLista: ");
	if (lista == NULL || lista->skladiste == NULL) {
		printf("< NULL >\n");
		return;
	}
	if (lista->broj_elemenata == 0) {
		printf("< Prazna >\n");
		return;
	}
	int(*matrica)[3] = (int(*)[3])lista->skladiste;
	int trenutni_red = 0; //uzimamo ceo prvi red
	do {
		printf("%d ", matrica[trenutni_red][1]); // drugi element u redu je podatak
		trenutni_red = matrica[trenutni_red][2]; // pomeramo se na sledeci
	} while (trenutni_red != PRAZNO);
	printf("\n");
}

SIGNAL sortiraj(LISTA* lista, SMER_SORTIRANJA smer, ALGORITAM_SORTIRANJA algoritam) {
	SIGNAL signal;
	signal.status = Greska;
	signal.poruka = poruka.GRESKA.sortiraj;
	if (*lista == NULL || (*lista)->skladiste == NULL || (*lista)->broj_elemenata == 0)
		return signal;

	if (algoritam == Bubble)
		signal = bubble_sort(lista, smer);
	if (algoritam == Insertion)
		signal = insertion_sort(lista, smer);
	if (algoritam == Selection)
		signal = selection_sort(lista, smer);

	return signal;

}

SIGNAL prazna(LISTA lista) {
	SIGNAL signal;
	signal.status = Info;
	if (lista == NULL || lista->skladiste == NULL || lista->broj_elemenata == 0) {
		signal.poruka = poruka.INFO.lista_prazna;
	}
	else {
		signal.poruka = poruka.INFO.lista_nije_prazna;
	}
	return signal;
}

SIGNAL sadrzi(LISTA* lista, PODATAK podatak, VRSTA_PRETRAGE vrsta) {
	SIGNAL signal;
	signal.status = Info;
	signal.poruka = poruka.INFO.podatak_ne_postoji;
	if (lista == NULL || (*lista)->skladiste == NULL || (*lista)->broj_elemenata == 0)
		return signal;
	int(*matrica)[3] = (int(*)[3])(*lista)->skladiste;
	if (vrsta == Iterativno) {
		int trenutni_red = 0; //uzimamo ceo prvi red
		do {
			if (matrica[trenutni_red][1] == podatak) {
				signal.poruka = poruka.INFO.podatak_postoji;
				return signal;
			}
			trenutni_red = matrica[trenutni_red][2]; // pomeramo se na sledeci
		} while (trenutni_red != PRAZNO);
	}
	else {
		//binarno pretrazivanje, lista mora biti sortirana
		sortiraj(lista, Rastuce, Bubble);

		int levo = 0;
		int desno = (*lista)->broj_elemenata - 1;

		while (levo <= desno) {
			int sredina = levo + (desno - levo) / 2;

			int pod = PODATAK_MAT(matrica, sredina);
			if (pod == PRAZNO) {
				signal.poruka = poruka.INFO.podatak_ne_postoji;
				break;
			}
			if (pod == podatak) {
				signal.poruka = poruka.INFO.podatak_postoji;
				break;
			}
			if (podatak < pod)
				desno = sredina - 1;
			else
				levo = sredina + 1;
		}
		if (levo > desno) {
			signal.poruka = poruka.INFO.podatak_ne_postoji; // ako smo dosli do kraja a nismo ga nasli
		}
	}
	return signal;
}

//implementacija pomocnih funkcija
void rotiraj_udesno(int mat[][3], int n, int pocetni_indeks) {
	//oslobadja se mesto za unos novog elementa 
	for (int i = n;i > pocetni_indeks;i--) {
		//printf("i: %d %d %d\n", mat[i][0], mat[i][1], mat[i][2]);
		//printf("i-1: %d %d %d\n", mat[i - 1][0], mat[i - 1][1], mat[i - 1][2]);
		mat[i][0] = mat[i - 1][0];
		if (mat[i][0] != PRAZNO) mat[i][0]++; //pomerice se i indeksi ukoliko ne treba da bude prazno
		mat[i][1] = mat[i - 1][1];
		mat[i][2] = mat[i - 1][2];
		if (mat[i][2] != PRAZNO) mat[i][2]++; //pomerice se i indeksi ukoliko ne treba da bude prazno
		//printf("===i: %d %d %d\n", mat[i][0], mat[i][1], mat[i][2]);

	}
}
void rotiraj_ulevo(int mat[][3], int n, int pocetni_indeks) {
	//pomeraju se elementi ulevo nakon izbacivanja elementa
	for (int i = pocetni_indeks;i < n - 1;i++) {
		//printf("i: %d %d %d\n", mat[i][0], mat[i][1], mat[i][2]);
		//printf("i+1: %d %d %d\n", mat[i+1][0], mat[i+1][1], mat[i+1][2]);
		mat[i][0] = mat[i + 1][0];
		if (mat[i][0] != PRAZNO) mat[i][0]--; //pomerice se i indeksi ukoliko ne treba da bude prazno
		mat[i][1] = mat[i + 1][1];
		mat[i][2] = mat[i + 1][2];
		if (mat[i][2] != PRAZNO) mat[i][2]--; //pomerice se i indeksi ukoliko ne treba da bude prazno
		//printf("===i: %d %d %d\n", mat[i][0], mat[i][1], mat[i][2]);
	}
}

SIGNAL bubble_sort(LISTA* lista, SMER_SORTIRANJA smer) {
	SIGNAL signal;
	int(*matrica)[3] = (int(*)[3])(*lista)->skladiste;

	for (int i = 0;i < ((*lista)->broj_elemenata - 1);i++) {
		for (int j = i + 1;j < (*lista)->broj_elemenata;j++) {
			if ((smer == Opadajuce && matrica[i][1] < matrica[j][1]) ||
				(smer == Rastuce && matrica[i][1] > matrica[j][1])) {
				int pom = matrica[i][1];
				matrica[i][1] = matrica[j][1];
				matrica[j][1] = pom;
			}
		}
	}
	signal.status = Info;
	signal.poruka = poruka.INFO.sortiraj;
	return signal;
}

SIGNAL insertion_sort(LISTA* lista, SMER_SORTIRANJA smer) {
	SIGNAL signal;

	int(*matrica)[3] = (int(*)[3])(*lista)->skladiste;

	// krecemo od 2. elementa (indeks 1)
	for (int i = 1; i < (*lista)->broj_elemenata; i++) {
		int trenutni = matrica[i][1]; // trenutni podatak koji zelimo da "ubacimo"
		int j = i - 1;

		// pomeramo podatke koji su veci (ili manji) udesno
		while (j >= 0 && (
			(smer == Rastuce && matrica[j][1] > trenutni) ||
			(smer == Opadajuce && matrica[j][1] < trenutni))) {
			matrica[j + 1][1] = matrica[j][1];
			j--;
		}
		matrica[j + 1][1] = trenutni;
	}

	signal.status = Info;
	signal.poruka = poruka.INFO.sortiraj;
	return signal;
}

SIGNAL selection_sort(LISTA* lista, SMER_SORTIRANJA smer) {
	SIGNAL signal;
	int(*matrica)[3] = (int(*)[3])(*lista)->skladiste;

	for (int i = 0; i < (*lista)->broj_elemenata - 1; i++) {
		int indeks_min_max = i;

		for (int j = i + 1; j < (*lista)->broj_elemenata; j++) {
			if ((smer == Rastuce && matrica[j][1] < matrica[indeks_min_max][1]) ||
				(smer == Opadajuce && matrica[j][1] > matrica[indeks_min_max][1])) {
				indeks_min_max = j;
			}
		}

		if (indeks_min_max != i) {
			int pom = matrica[i][1];
			matrica[i][1] = matrica[indeks_min_max][1];
			matrica[indeks_min_max][1] = pom;
		}
	}
	signal.status = Info;
	signal.poruka = poruka.INFO.sortiraj;
	return signal;
}

//main

int main() {
	LISTA lista = NULL;
	SIGNAL signal;
	signal = kreiraj(&lista);
	obrada_statusa(signal.status, signal.poruka, NULL, __LINE__);

	signal = prazna(lista);
	obrada_statusa(signal.status, signal.poruka, NULL, __LINE__);

	signal = sadrzi(&lista, 5, Binarno);
	obrada_statusa(signal.status, signal.poruka, NULL, __LINE__);

	signal = ubaci(&lista, 5, Kraj);
	obrada_statusa(signal.status, signal.poruka, NULL, __LINE__);

	signal = ubaci(&lista, 7, Kraj);
	obrada_statusa(signal.status, signal.poruka, NULL, __LINE__);

	signal = ubaci(&lista, 6, Vrednost);
	obrada_statusa(signal.status, signal.poruka, NULL, __LINE__);

	signal = ubaci(&lista, 9, Pocetak);
	obrada_statusa(signal.status, signal.poruka, NULL, __LINE__);

	prikazi(lista);
	signal = prazna(lista);
	obrada_statusa(signal.status, signal.poruka, NULL, __LINE__);

	/*signal = sadrzi(&lista, 5, Binarno);
	obrada_statusa(signal.status, signal.poruka, NULL, __LINE__);

	signal = sadrzi(&lista, 2, Binarno);
	obrada_statusa(signal.status, signal.poruka, NULL, __LINE__);*/

	int izbaceni = 10;
	signal = izbaci(&lista, &izbaceni, Vrednost);
	//signal = izbaci_sa_pocetka(&lista, &izbaceni);
	obrada_statusa(signal.status, signal.poruka, NULL, __LINE__);
	printf("\nIzbaceni: %d\n", izbaceni);
	prikazi(lista);

	signal = sortiraj(&lista, Rastuce, Selection);
	obrada_statusa(signal.status, signal.poruka, NULL, __LINE__);
	prikazi(lista);

	return 0;
}