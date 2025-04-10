
#include "lista.h"
#define DIM 100
#define PRAZNO -1

//svaki red matrice ima prethodni, podatak i sledeci (3 kolone)
#define PRETHODNI(matrica,i) (((int(*)[3])(matrica))[i][0]) //matricu kastuj u (int(*)[3]) i uzmi [i][0]
#define PODATAK_MAT(matrica,i) (((int(*)[3])(matrica))[i][1])
#define SLEDECI(matrica,i) (((int(*)[3])(matrica))[i][2])

//prototipovi pomocnih funkcija
void rotiraj_udesno(int[][3], int);
void rotiraj_ulevo(int[][3], int);

bool kreiraj(LISTA* lista) {
	*lista = malloc(sizeof(struct lista));
	(*lista)->skladiste = malloc(sizeof(int[DIM][3]));
	//memset((*lista)->skladiste, PRAZNO, sizeof(int[DIM][3]));
	(*lista)->kapacitet = DIM;
	(*lista)->broj_elemenata = 0;
	return true;
}

bool unisti(LISTA* lista) {
	if (*lista == NULL || (*lista)->skladiste == NULL || (*lista)->broj_elemenata == 0) return false; // lista je prazna ili ne postoji

	free((*lista)->skladiste);
	(*lista)->skladiste = NULL;
	free(*lista);
	*lista = NULL;
	return true;
}

bool ubaci_na_pocetak(LISTA* lista, PODATAK podatak) {
	if (*lista == NULL || (*lista)->skladiste == NULL || (*lista)->broj_elemenata >= (*lista)->kapacitet) return false;

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
	return true;
}

bool izbaci_sa_pocetka(LISTA* lista, PODATAK* podatak) {
	if (*lista == NULL || (*lista)->skladiste == NULL || (*lista)->broj_elemenata == 0) return false;

	int(*matrica)[3] = (int(*)[3])(*lista)->skladiste;
	*podatak = matrica[0][1]; //ovaj element izbacujemo
	matrica[0][0] = PRAZNO;
	matrica[0][1] = PRAZNO;

	int nova_glava = matrica[0][2];
	matrica[0][2] = PRAZNO;
	matrica[nova_glava][0] = PRAZNO;

	rotiraj_ulevo(matrica, (*lista)->broj_elemenata);
	(*lista)->broj_elemenata--;
	return true;
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

bool sortiraj(LISTA* lista) {
	if (*lista == NULL || (*lista)->skladiste == NULL || (*lista)->broj_elemenata == 0) return false;
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

	return true;
}

bool prazna(LISTA lista) {
	if (lista == NULL || lista->skladiste == NULL || lista->broj_elemenata == 0) return true;
	return false;
}
bool sadrzi(LISTA lista, PODATAK podatak) {
	if (lista == NULL || lista->skladiste == NULL || lista->broj_elemenata == 0) return false;
	int(*matrica)[3] = (int(*)[3])lista->skladiste;
	int trenutni_red = 0; //uzimamo ceo prvi red
	do {
		if (matrica[trenutni_red][1] == podatak)
			return true;
		trenutni_red = matrica[trenutni_red][2]; // pomeramo se na sledeci
	} while (trenutni_red != PRAZNO);
	return false;
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
	kreiraj(&lista) ? printf("Lista uspesno kreirana!\n") : printf("Lista nije uspesno kreirana!\n");

	prazna(lista) ? printf("Lista je prazna!\n") : printf("Lista nije prazna!\n");
	sadrzi(lista, 5) ? printf("Podatak %d postoji u listi!\n", 5) : printf("Podatak %d ne postoji u listi!\n", 5);

	ubaci_na_pocetak(&lista, 5) ? printf("Podatak uspesno dodat!\n") : printf("Podatak nije uspesno dodat!\n");
	ubaci_na_pocetak(&lista, 7) ? printf("Podatak uspesno dodat!\n") : printf("Podatak nije uspesno dodat!\n");
	ubaci_na_pocetak(&lista, 1) ? printf("Podatak uspesno dodat!\n") : printf("Podatak nije uspesno dodat!\n");
	ubaci_na_pocetak(&lista, 9) ? printf("Podatak uspesno dodat!\n") : printf("Podatak nije uspesno dodat!\n");
	prikazi(lista);
	prazna(lista) ? printf("Lista je prazna!\n") : printf("Lista nije prazna!\n");
	sadrzi(lista, 5) ? printf("Podatak %d postoji u listi!\n", 5) : printf("Podatak %d ne postoji u listi!\n", 5);
	sadrzi(lista, 2) ? printf("Podatak %d postoji u listi!\n", 2) : printf("Podatak %d ne postoji u listi!\n", 2);

	int izbaceni;
	izbaci_sa_pocetka(&lista, &izbaceni) ? printf("Podatak %d je uspesno izbacen!\n", izbaceni) : printf("Podatak %d nije uspesno izbacen!\n", izbaceni);
	prikazi(lista);

	sortiraj(&lista);
	prikazi(lista);

	return 0;
}