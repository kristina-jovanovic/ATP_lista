
#include "kalkulator.h"

// specifikacija pomocnih funkcija - omotaca operacija
void sabiranje();
void oduzimanje();
void mnozenje();
void deljenje();

int saberi(int broj1, int broj2) {
	return broj1 + broj2;
}

int oduzmi(int broj1, int broj2) {
	return broj1 - broj2;
}

int pomnozi(int broj1, int broj2) {
	return broj1 * broj2;
}

double podeli(int broj1, int broj2) {
	if (broj2 == 0) {
		return -1;
	}
	return (double)broj1 / broj2;
}

void dodeli_funkcije_kalkulator(MENI* meni) {
	static const STAVKA_MENIJA mapa[] = {
		{ L"Сабирање", sabiranje },
		{ L"Одузимање", oduzimanje },
		{ L"Множење", mnozenje },
		{ L"Дељење", deljenje },
	};

	int broj_opcija = sizeof(mapa) / sizeof(mapa[0]);
	bool postavljeno = false;

	for (int i = 0;i < broj_opcija;i++) {
		for (int j = 0; j < broj_opcija; j++) {
			if (wcscmp(meni->stavke[i].opis, mapa[j].opis) == 0) {
				meni->stavke[i].funkcija = mapa[j].funkcija;
				postavljeno = true;
				break;
			}
		}
		// ako se opis ne poklapa ni sa jednom poznatom opcijom
		if (!postavljeno)
			meni->stavke[i].funkcija = NULL;
		postavljeno = false; //resetujemo za sledeci krug
	}
}

// implementacija pomocnih funkcija
void sabiranje() {
	int broj1, broj2;
	wprintf(L"Унесите два цела броја која желите да саберете, одвојена размаком: ");
	wscanf(L"%d %d", &broj1, &broj2);
	fflush(stdin);
	int rezultat = saberi(broj1, broj2);
	wprintf(L"Резултат: %d\n", rezultat);
}

void oduzimanje() {
	int broj1, broj2;
	wprintf(L"Унесите два цела броја која желите да одузмете, одвојена размаком: ");
	wscanf(L"%d %d", &broj1, &broj2);
	fflush(stdin);
	int rezultat = oduzmi(broj1, broj2);
	wprintf(L"Резултат: %d\n", rezultat);
}

void mnozenje() {
	int broj1, broj2;
	wprintf(L"Унесите два цела броја која желите да помножите, одвојена размаком: ");
	wscanf(L"%d %d", &broj1, &broj2);
	fflush(stdin);
	int rezultat = pomnozi(broj1, broj2);
	wprintf(L"Резултат: %d\n", rezultat);
}

void deljenje() {
	int broj1, broj2;
	wprintf(L"Унесите два цела броја која желите да поделите, одвојена размаком: ");
	wscanf(L"%d %d", &broj1, &broj2);
	fflush(stdin);
	double rezultat = podeli(broj1, broj2);
	wprintf(L"Резултат: %lf\n", rezultat);
}
