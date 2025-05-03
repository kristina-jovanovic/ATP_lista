
#include "meni.h"

void fja() {
	printf("fja\n");
}

void kreiraj_meni(MENI* meni, STRING naziv_datoteke) {
	// ovo treba da se ucitava iz datoteke ------??
	meni->naziv = "GLAVNI MENI";
	meni->broj_stavki = 2;
	STAVKA_MENIJA s1 = { .opis = "Ubaci", .funkcija = fja };
	STAVKA_MENIJA s2 = { .opis = "Izbaci", .funkcija = fja };
	meni->stavke[0] = s1;
	meni->stavke[1] = s2;
}

void prikazi_meni(MENI meni) {
	printf("\n==========================================\n");
	printf("%25s\n", meni.naziv);
	for (int i = 0;i < meni.broj_stavki;i++) {
		printf("%d. %s\n", i + 1, meni.stavke[i].opis);
	}
	printf("0. KRAJ RADA\n");
	printf("==========================================\n");
}

void obradi_opciju(MENI meni, int opcija) {
	if (opcija<0 || opcija > meni.broj_stavki) {
		printf("\nPogresna opcija!\n");
		return;
	}
	if (opcija == 0) {
		//kraj rada ili vracanje u prethodni meni
		// .......
		return;
	}
	int indeks_opcije = opcija - 1;
	meni.stavke[indeks_opcije].funkcija();
}

