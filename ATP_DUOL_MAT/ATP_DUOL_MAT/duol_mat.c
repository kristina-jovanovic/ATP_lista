
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
void rotiraj_udesno(int[][3], int);
void rotiraj_ulevo(int[][3], int);

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
		rotiraj_udesno(matrica, (*lista)->broj_elemenata);
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

	rotiraj_ulevo(matrica, (*lista)->broj_elemenata);
	(*lista)->broj_elemenata--;
	signal.status = Info;
	signal.poruka = poruka.INFO.izbaci;
	return signal;
}

void prikazi(LISTA lista) {
	printf("\nLista: ");
	if (lista == NULL || lista->skladiste == NULL) {
		printf("< NULL >");
		return;
	}
	if (lista->broj_elemenata == 0) {
		printf("< Prazna >");
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

SIGNAL sortiraj(LISTA* lista) {
	SIGNAL signal;
	signal.status = Greska;
	signal.poruka = poruka.GRESKA.sortiraj;
	if (*lista == NULL || (*lista)->skladiste == NULL || (*lista)->broj_elemenata == 0)
		return signal;
	int(*matrica)[3] = (int(*)[3])(*lista)->skladiste;

	for (int i = 0;i < ((*lista)->broj_elemenata - 1);i++) {
		for (int j = i + 1;j < (*lista)->broj_elemenata;j++) {
			if (matrica[i][1] < matrica[j][1]) {
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

SIGNAL sadrzi(LISTA lista, PODATAK podatak) {
	SIGNAL signal;
	signal.status = Info;
	signal.poruka = poruka.INFO.podatak_ne_postoji;
	if (lista == NULL || lista->skladiste == NULL || lista->broj_elemenata == 0)
		return signal;
	int(*matrica)[3] = (int(*)[3])lista->skladiste;
	int trenutni_red = 0; //uzimamo ceo prvi red
	do {
		if (matrica[trenutni_red][1] == podatak) {
			signal.poruka = poruka.INFO.podatak_postoji;
			return signal;
		}
		trenutni_red = matrica[trenutni_red][2]; // pomeramo se na sledeci
	} while (trenutni_red != PRAZNO);
	return signal;
}

//implementacija pomocnih funkcija
void rotiraj_udesno(int mat[][3], int n) {
	//oslobadja se mesto za unos novog elementa na pocetak
	for (int i = n;i > 0;i--) {
		mat[i][0] = mat[i - 1][0];
		if (mat[i][0] != PRAZNO) mat[i][0]++; //pomerice se i indeksi ukoliko ne treba da bude prazno
		mat[i][1] = mat[i - 1][1];
		mat[i][2] = mat[i - 1][2];
		if (mat[i][2] != PRAZNO) mat[i][2]++; //pomerice se i indeksi ukoliko ne treba da bude prazno
	}
}
void rotiraj_ulevo(int mat[][3], int n) {
	//pomeraju se elementi ulevo nakon izbacivanja prvog elementa
	for (int i = 0;i < n;i++) {
		mat[i][0] = mat[i + 1][0];
		if (mat[i][0] != PRAZNO) mat[i][0]--; //pomerice se i indeksi ukoliko ne treba da bude prazno
		mat[i][1] = mat[i + 1][1];
		mat[i][2] = mat[i + 1][2];
		if (mat[i][2] != PRAZNO) mat[i][2]--; //pomerice se i indeksi ukoliko ne treba da bude prazno
	}
}

//main

int main() {
	LISTA lista = NULL;
	SIGNAL signal;
	signal = kreiraj(&lista);
	obrada_statusa(signal.status, signal.poruka, NULL, __LINE__);

	signal = prazna(lista);
	obrada_statusa(signal.status, signal.poruka, NULL, __LINE__);

	signal = sadrzi(lista, 5);
	obrada_statusa(signal.status, signal.poruka, NULL, __LINE__);

	signal = ubaci_na_pocetak(&lista, 5);
	obrada_statusa(signal.status, signal.poruka, NULL, __LINE__);

	signal = ubaci_na_pocetak(&lista, 7);
	obrada_statusa(signal.status, signal.poruka, NULL, __LINE__);

	signal = ubaci_na_pocetak(&lista, 1);
	obrada_statusa(signal.status, signal.poruka, NULL, __LINE__);

	signal = ubaci_na_pocetak(&lista, 9);
	obrada_statusa(signal.status, signal.poruka, NULL, __LINE__);

	prikazi(lista);
	signal = prazna(lista);
	obrada_statusa(signal.status, signal.poruka, NULL, __LINE__);

	signal = sadrzi(lista, 5);
	obrada_statusa(signal.status, signal.poruka, NULL, __LINE__);

	signal = sadrzi(lista, 2);
	obrada_statusa(signal.status, signal.poruka, NULL, __LINE__);


	int izbaceni;
	signal = izbaci_sa_pocetka(&lista, &izbaceni);
	obrada_statusa(signal.status, signal.poruka, NULL, __LINE__);
	prikazi(lista);

	signal = sortiraj(&lista);
	obrada_statusa(signal.status, signal.poruka, NULL, __LINE__);
	prikazi(lista);

	return 0;
}