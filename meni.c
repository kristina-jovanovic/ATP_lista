
#include "meni.h"

void kreiraj_meni(MENI* meni, STRING naziv_datoteke) {

	FILE* datoteka = fopen(naziv_datoteke, "r, ccs=UTF-8");
	if (datoteka == NULL) {
		wprintf(L"Грешка при учитавању датотеке.\n");
		return;
	}

	wchar_t linija[256];

	// naziv menija
	if (fgetws(linija, sizeof(linija) / sizeof(wchar_t), datoteka) == NULL) { //kao fgets() samo za wide char
		wprintf(L"Грешка: датотека је празна.\n");
		fclose(datoteka);
		return;
	}
	linija[wcscspn(linija, L"\n")] = L'\0'; // uklanjanje novog reda - wcscspn() vraca indeks prvog pojavljivanja '\n' u liniji
	wcscpy(meni->naziv, linija); // wide verzija strcpy() funkcije

	// broj stavki
	if (fgetws(linija, sizeof(linija) / sizeof(wchar_t), datoteka) == NULL) {
		wprintf(L"Грешка: нема броја ставки.\n");
		fclose(datoteka);
		return;
	}
	meni->broj_stavki = _wtoi(linija);  // konverzija wide stringa u int

	// ucitavanje opisa stavki
	for (int i = 0; i < meni->broj_stavki; i++) {
		if (fgetws(linija, sizeof(linija) / sizeof(wchar_t), datoteka) == NULL) {
			wprintf(L"Грешка: недовољно ставки.\n");
			break;
		}
		linija[wcscspn(linija, L"\n")] = L'\0'; // ukloni novi red
		wcscpy(meni->stavke[i].opis, linija);
	}

	fclose(datoteka);
}

void prikazi_meni(MENI meni) {
	wprintf(L"\n========================================================\n");
	wprintf(L"%32ls\n", meni.naziv);
	for (int i = 0;i < meni.broj_stavki;i++) {
		wprintf(L"%d. %ls\n", i + 1, meni.stavke[i].opis);
	}
	wprintf(L"0. ПОВРАТАК/КРАЈ РАДА\n");
	wprintf(L"========================================================\n");
}

void pokreni_meni(MENI meni) {
	int stavka = 0;
	do {
		prikazi_meni(meni);
		wprintf(L"Унесите редни број жељене ставке: ");
		wscanf(L"%d", &stavka);
		fflush(stdin);
		obradi_stavku(meni, stavka);
	} while (stavka != 0);

	if (strcmp(meni.naziv, L"Главни мени") == 0) {
		wprintf(L"Гашење програма...\n");
	}
}

void obradi_stavku(MENI meni, int stavka) {
	if (stavka < 0 || stavka > meni.broj_stavki) {
		wprintf(L"\nПогрешна опција!\n");
		return;
	}
	if (stavka == 0) {
		//kraj rada ili vracanje u prethodni meni
		return;
	}
	int indeks_stavke = stavka - 1;
	meni.stavke[indeks_stavke].funkcija();
}
