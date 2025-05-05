
#include "lista.h"
#define DIM 100
#define PRAZNO -1

//svaki red matrice ima prethodni, podatak i sledeci (3 kolone)
#define PRETHODNI(matrica,i) (((int(*)[3])(matrica))[i][0]) //matricu kastuj u (int(*)[3]) i uzmi [i][0]
#define PODATAK_MAT(matrica,i) (((int(*)[3])(matrica))[i][1])
#define SLEDECI(matrica,i) (((int(*)[3])(matrica))[i][2])

//prototipovi pomocnih funkcija
void rotiraj_udesno(int[][3], int, int);
void rotiraj_ulevo(int[][3], int, int);
void bubble_sort(LISTA, SMER_SORTIRANJA);
void insertion_sort(LISTA, SMER_SORTIRANJA);
void selection_sort(LISTA, SMER_SORTIRANJA);

void kreiraj(LISTA* lista) {
	*lista = malloc(sizeof(struct lista));
	if (*lista == NULL) {
		PRIJAVI(Kod.Greska.Kreiraj);
		return;
	}
	(*lista)->skladiste = malloc(sizeof(int[DIM][3]));
	if ((*lista)->skladiste == NULL) {
		PRIJAVI(Kod.Greska.Kreiraj);
		return;
	}
	//memset((*lista)->skladiste, PRAZNO, sizeof(int[DIM][3]));
	(*lista)->kapacitet = DIM;
	(*lista)->broj_elemenata = 0;

	PRIJAVI(Kod.Info.Kreiraj);
}

void unisti(LISTA lista) {
	if (lista == NULL) {
		PRIJAVI(Kod.Greska.Lista_ne_postoji);
		return;
	}
	if (lista->skladiste == NULL || lista->broj_elemenata == 0) {
		PRIJAVI(Kod.Upozorenje.Lista_prazna);
		return;
	}

	free(lista->skladiste);
	lista->skladiste = NULL;
	free(lista);
	lista = NULL;

	PRIJAVI(Kod.Info.Unisti);
}

void ubaci(LISTA lista, PODATAK podatak, NACIN nacin) {
	if (lista == NULL || lista->skladiste == NULL) {
		PRIJAVI(Kod.Greska.Lista_ne_postoji);
		return;
	}
	if (lista->broj_elemenata >= lista->kapacitet) {
		PRIJAVI(Kod.Upozorenje.Ubaci);
		return;
	}
	int(*matrica)[3] = (int(*)[3])lista->skladiste;
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
			rotiraj_udesno(matrica, lista->broj_elemenata, novi_indeks);

			SLEDECI(matrica, novi_indeks - 1) = novi_indeks;
			PRETHODNI(matrica, novi_indeks + 1) = novi_indeks;

			PRETHODNI(matrica, novi_indeks) = novi_indeks - 1;
			SLEDECI(matrica, novi_indeks) = novi_indeks + 1;
			PODATAK_MAT(matrica, novi_indeks) = podatak;
		}
	}
	if (nacin == Kraj) {
	kraj:
		novi_indeks = lista->broj_elemenata;
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
		if (lista->broj_elemenata > 0) {
			//lista nije prazna
			rotiraj_udesno(matrica, lista->broj_elemenata, 0);
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
	lista->broj_elemenata++;
	PRIJAVI(Kod.Info.Ubaci);
}

void izbaci(LISTA lista, PODATAK* podatak, NACIN nacin) {
	if (lista == NULL || lista->skladiste == NULL) {
		PRIJAVI(Kod.Greska.Lista_ne_postoji);
		return;
	}
	if (lista->broj_elemenata == 0) {
		PRIJAVI(Kod.Upozorenje.Lista_prazna);
		return;
	}

	int(*matrica)[3] = (int(*)[3])lista->skladiste;
	int broj_el = lista->broj_elemenata;

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
	PRIJAVI(Kod.Upozorenje.Izbaci);
	return;
kraj_true:
	lista->broj_elemenata--;
	PRIJAVI(Kod.Info.Izbaci, *podatak);
}

void prikazi(LISTA lista) {
	wprintf(L"\nЛиста: ");
	if (lista == NULL || lista->skladiste == NULL) {
		wprintf(L"< NULL >\n");
		return;
	}
	if (lista->broj_elemenata == 0) {
		wprintf(L"< Празна >\n");
		return;
	}
	int(*matrica)[3] = (int(*)[3])lista->skladiste;
	int trenutni_red = 0; //uzimamo ceo prvi red
	do {
		wprintf(L"%d ", matrica[trenutni_red][1]); // drugi element u redu je podatak
		trenutni_red = matrica[trenutni_red][2]; // pomeramo se na sledeci
	} while (trenutni_red != PRAZNO);
	wprintf(L"\n");
}

void sortiraj(LISTA lista, SMER_SORTIRANJA smer, ALGORITAM_SORTIRANJA algoritam) {
	if (lista == NULL || lista->skladiste == NULL || lista->broj_elemenata == 0) {
		PRIJAVI(Kod.Greska.Lista_ne_postoji);
		return;
	}
	if (lista->broj_elemenata < 2) {
		PRIJAVI(Kod.Upozorenje.Sortiraj);
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
	bool prazna;
	if (lista == NULL || lista->skladiste == NULL || lista->broj_elemenata == 0) {
		PRIJAVI(Kod.Info.Lista_prazna);
		prazna = true;
	}
	else {
		PRIJAVI(Kod.Info.Lista_nije_prazna);
		prazna = false;
	}
	return prazna;
}

bool sadrzi(LISTA lista, PODATAK podatak, VRSTA_PRETRAGE vrsta) {
	if (lista == NULL || lista->skladiste == NULL || lista->broj_elemenata == 0) {
		PRIJAVI(Kod.Info.Podatak_ne_postoji, podatak);
		return false;
	}
	bool sadrzi;
	int(*matrica)[3] = (int(*)[3])lista->skladiste;
	if (vrsta == Iterativno) {
		int trenutni_red = 0; //uzimamo ceo prvi red
		do {
			if (matrica[trenutni_red][1] == podatak) {
				PRIJAVI(Kod.Info.Podatak_postoji, podatak);
				sadrzi = true;
				break;
			}
			trenutni_red = matrica[trenutni_red][2]; // pomeramo se na sledeci
		} while (trenutni_red != PRAZNO);
	}
	else {
		//binarno pretrazivanje, lista mora biti sortirana
		sortiraj(lista, Rastuce, Bubble);

		int levo = 0;
		int desno = lista->broj_elemenata - 1;

		while (levo <= desno) {
			int sredina = levo + (desno - levo) / 2;

			int pod = PODATAK_MAT(matrica, sredina);
			if (pod == PRAZNO) {
				sadrzi = false; //dosli smo do kraja tj praznog elementa
				break;
			}
			if (pod == podatak) {
				sadrzi = true;
				break;
			}
			if (podatak < pod)
				desno = sredina - 1;
			else
				levo = sredina + 1;
		}
		if (levo > desno) {
			sadrzi = false; // ako smo dosli do kraja a nismo ga nasli
		}
	}
	if (sadrzi)
		PRIJAVI(Kod.Info.Podatak_postoji, podatak);
	else
		PRIJAVI(Kod.Info.Podatak_ne_postoji, podatak);

	return sadrzi;
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

void bubble_sort(LISTA lista, SMER_SORTIRANJA smer) {
	int(*matrica)[3] = (int(*)[3])lista->skladiste;

	for (int i = 0;i < (lista->broj_elemenata - 1);i++) {
		for (int j = i + 1;j < lista->broj_elemenata;j++) {
			if ((smer == Opadajuce && matrica[i][1] < matrica[j][1]) ||
				(smer == Rastuce && matrica[i][1] > matrica[j][1])) {
				int pom = matrica[i][1];
				matrica[i][1] = matrica[j][1];
				matrica[j][1] = pom;
			}
		}
	}
	PRIJAVI(Kod.Info.Sortiraj, (smer == Rastuce ? L"растуће" : L"опадајуће"));
}

void insertion_sort(LISTA lista, SMER_SORTIRANJA smer) {
	int(*matrica)[3] = (int(*)[3])lista->skladiste;

	// krecemo od 2. elementa (indeks 1)
	for (int i = 1; i < lista->broj_elemenata; i++) {
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

	PRIJAVI(Kod.Info.Sortiraj, (smer == Rastuce ? L"растуће" : L"опадајуће"));
}

void selection_sort(LISTA lista, SMER_SORTIRANJA smer) {
	int(*matrica)[3] = (int(*)[3])lista->skladiste;

	for (int i = 0; i < lista->broj_elemenata - 1; i++) {
		int indeks_min_max = i;

		for (int j = i + 1; j < lista->broj_elemenata; j++) {
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
	PRIJAVI(Kod.Info.Sortiraj, (smer == Rastuce ? L"растуће" : L"опадајуће"));
}

//main

int main() {
	_setmode(_fileno(stdout), _O_U8TEXT); // neophodno za ispis na cirilici 
	_setmode(_fileno(stderr), _O_U8TEXT); // neophodno za ispis na cirilici 
	setlocale(LC_ALL, "");
	//+ moraju da se koriste wide funckije - wprintf() i slicne 

	LISTA lista = NULL;
	kreiraj(&lista);

	prazna(lista);

	sadrzi(lista, 5, Binarno);

	ubaci(lista, 5, Kraj);

	ubaci(lista, 7, Kraj);

	ubaci(lista, 6, Vrednost);

	ubaci(lista, 9, Pocetak);

	prikazi(lista);
	prazna(lista);

	/*signal = sadrzi(lista, 5, Binarno);
	obrada_statusa(signal.status, signal.poruka, NULL, __LINE__);

	signal = sadrzi(lista, 2, Binarno);
	obrada_statusa(signal.status, signal.poruka, NULL, __LINE__);*/

	int izbaceni = 10;
	izbaci(lista, &izbaceni, Vrednost);
	//signal = izbaci_sa_pocetka(lista, &izbaceni);
	prikazi(lista);

	sortiraj(lista, Rastuce, Selection);
	prikazi(lista);

	return 0;
}