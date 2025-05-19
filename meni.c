
#include "meni.h"

void init() {
	_setmode(_fileno(stdout), _O_U8TEXT); // neophodno za ispis na cirilici 
	_setmode(_fileno(stderr), _O_U8TEXT); // neophodno za ispis na cirilici 
	const char* result = setlocale(LC_ALL, "Serbian (Latin)_Serbia.1252");
	//+ moraju da se koriste wide funckije - wprintf() i slicne 
}

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
	if (wcscmp(meni.naziv, L"Главни мени") == 0) {
		//u pitanju je glavni meni - prva stavka je unisti koju ne prikazujemo
		for (int i = 1;i < meni.broj_stavki;i++) {
			wprintf(L"%d. %ls\n", i, meni.stavke[i].opis);
		}
		wprintf(L"0. КРАЈ РАДА\n");
	}
	else {
		//u pitanju je neki od podmenija
		for (int i = 0;i < meni.broj_stavki;i++) {
			wprintf(L"%d. %ls\n", i + 1, meni.stavke[i].opis);
		}
		wprintf(L"0. ПОВРАТАК НА ПРЕТХОДНИ МЕНИ\n");
	}
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

	if (wcscmp(meni.naziv, L"Главни мени") == 0) {
		wprintf(L"Крај рада...\n");
	}
}

void obradi_stavku(MENI meni, int stavka) {
	if (stavka < 0 || stavka > meni.broj_stavki) {
		wprintf(L"\nПогрешна опција!\n");
		return;
	}
	if (wcscmp(meni.naziv, L"Главни мени") == 0) {
		//glavni meni - 0. stavka je unisti koju ne prikazujemo, ostale idu od 1 kao u prikazu
		if (meni.stavke[stavka].funkcija != NULL)
			meni.stavke[stavka].funkcija();
	}
	else {
		//podmeni - stavke idu od 0, za 1 manje u odnosu na prikaz
		if (stavka == 0) {
			return;
		}
		else {
			int indeks_stavke = stavka - 1;
			if (meni.stavke[indeks_stavke].funkcija != NULL)
				meni.stavke[indeks_stavke].funkcija();
		}
	}
}
