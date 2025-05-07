
#include "meni.h"
//#include "lista.h"

void fja1() {
	wprintf(L"fja1\n");
}
void fja2() {
	wprintf(L"fja2\n");
}

// pomocne funkcije
void dodeli_funkciju(MENI* meni, int i) {
#pragma region MyRegion

	//if (wcscmp(meni->stavke[i].opis, L"Убаци елемент") == 0) { //kao strcmp() samo za wide char
	//	meni->stavke[i].funkcija = fja1;
	//}
	//else if (wcscmp(meni->stavke[i].opis, L"Избаци елемент") == 0) {
	//	meni->stavke[i].funkcija = fja2;
	//}
	////... ostale opcije
	//else {
	//	meni->stavke[i].funkcija = NULL;
	//}
#pragma endregion

	// da ne bismo radili mnogo if-else grana, pravimo mapu sa parovima opisa i funkcija
	//proverimo koji tacno opis je procitan iz datoteke i u skladu sa tim dodelimo odgovarajucu funkciju
	static const STAVKA_MENIJA mapa[] = {
		{ L"Убаци елемент", fja1 },
		{ L"Избаци елемент", fja2 },
		{ L"Прикажи елементе", fja1 },
		{ L"Сортирај елементе", fja1 },
		{ L"Да ли је листа празна", fja1 },
		{ L"Да ли одређени елемент постоји", fja1 },
	};

	int broj_opcija = sizeof(mapa) / sizeof(mapa[0]);

	for (int j = 0; j < broj_opcija; j++) {
		if (wcscmp(meni->stavke[i].opis, mapa[j].opis) == 0) {
			meni->stavke[i].funkcija = mapa[j].funkcija;
			return;
		}
	}
	// ako se opis ne poklapa ni sa jednom poznatom opcijom
	meni->stavke[i].funkcija = NULL;
}

void kreiraj_meni(MENI* meni, STRING naziv_datoteke) {

	FILE* datoteka = fopen(naziv_datoteke, "r, ccs=UTF-8");
	if (datoteka == NULL) {
		wprintf(L"Greska - Ucitavanje datoteke\n");
		return;
	}

	wchar_t linija[256];

	// naziv menija
	if (fgetws(linija, sizeof(linija) / sizeof(wchar_t), datoteka) == NULL) { //kao fgets() samo za wide char
		wprintf(L"Greska: prazna datoteka.\n");
		fclose(datoteka);
		return;
	}
	linija[wcscspn(linija, L"\n")] = L'\0'; // uklanjanje novog reda - wcscspn() vraca indeks prvog pojavljivanja '\n' u liniji
	wcscpy(meni->naziv, linija); // wide verzija strcpy() funkcije

	// broj stavki
	if (fgetws(linija, sizeof(linija) / sizeof(wchar_t), datoteka) == NULL) {
		wprintf(L"Greska: nema broja stavki.\n");
		fclose(datoteka);
		return;
	}
	meni->broj_stavki = _wtoi(linija);  // konverzija wide stringa u int

	// ucitavanje opisa stavki
	for (int i = 0; i < meni->broj_stavki; i++) {
		if (fgetws(linija, sizeof(linija) / sizeof(wchar_t), datoteka) == NULL) {
			wprintf(L"Greska: nedovoljno stavki.\n");
			break;
		}
		linija[wcscspn(linija, L"\n")] = L'\0'; // ukloni novi red
		wcscpy(meni->stavke[i].opis, linija);
		dodeli_funkciju(meni, i);
	}

	fclose(datoteka);
}

void prikazi_meni(MENI meni) {
	wprintf(L"\n==========================================\n");
	wprintf(L"%25ls\n", meni.naziv);
	for (int i = 0;i < meni.broj_stavki;i++) {
		wprintf(L"%d. %ls\n", i + 1, meni.stavke[i].opis);
	}
	wprintf(L"0. КРАЈ РАДА\n");
	wprintf(L"==========================================\n");
}

void obradi_opciju(MENI meni, int opcija) {
	if (opcija<0 || opcija > meni.broj_stavki) {
		wprintf(L"\nПогрешна опција!\n");
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

