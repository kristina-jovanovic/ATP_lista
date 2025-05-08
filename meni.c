
#include "meni.h"
#include "lista.h"

// specifikacija pomocnih funkcija
void ubaci_element();
void ubaci_element_na_pocetak();
void ubaci_element_na_kraj();
void ubaci_element_sortirano();
void izbaci_element();
void izbaci_element_sa_pocetka();
void izbaci_element_sa_kraja();
void izbaci_element_po_vrednosti();
void prikazi_elemente();
void sortiraj_elemente();
void sortiraj_elemente_rastuce_bubble();
void sortiraj_elemente_rastuce_insertion();
void sortiraj_elemente_rastuce_selection();
void sortiraj_elemente_opadajuce_bubble();
void sortiraj_elemente_opadajuce_insertion();
void sortiraj_elemente_opadajuce_selection();
void da_li_je_lista_prazna();
void pretrazi_element();
void pretrazi_element_sekvencijalno();
void pretrazi_element_binarno();

void dodeli_funkciju(MENI* meni, int i);

// globalna promenljiva
LISTA lista;

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
		dodeli_funkciju(meni, i);
	}

	fclose(datoteka);

	//inicijalizacija liste == da li ovde ili gde?
	if (strcmp(meni->naziv, L"Главни мени") == 0)
		kreiraj(&lista);
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
		//unisti(lista); // da li ovde? --implementacija preko datoteke onda nece cuvati podatke tj. obrisace ih na kraju svaki put
		// pa cemo zato rucno samo osloboditi memoriju u tom slucaju
		if (strcmp((STRING)lista->skladiste, "lista.dat") == 0) {
			lista->skladiste = NULL;
			lista->broj_elemenata = 0;
			lista = NULL;
		}
		else {
			unisti(lista);
		}
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

// implementacija pomocnih funkcija
void ubaci_element() {
	MENI meni_ubaci;
	kreiraj_meni(&meni_ubaci, "../../meni_datoteke/meni_ubaci.txt");
	pokreni_meni(meni_ubaci);
}

void izbaci_element() {
	MENI meni_izbaci;
	kreiraj_meni(&meni_izbaci, "../../meni_datoteke/meni_izbaci.txt");
	pokreni_meni(meni_izbaci);
}

void prikazi_elemente() {
	prikazi(lista);
}

void sortiraj_elemente() {
	MENI meni_sortiraj;
	kreiraj_meni(&meni_sortiraj, "../../meni_datoteke/meni_sortiraj.txt");
	pokreni_meni(meni_sortiraj);
}

void da_li_je_lista_prazna() {
	prazna(lista);
}

void pretrazi_element() {
	MENI meni_pretrazi;
	kreiraj_meni(&meni_pretrazi, "../../meni_datoteke/meni_pretrazivanje.txt");
	pokreni_meni(meni_pretrazi);
}

void ubaci_element_na_pocetak() {
	int broj;
	wprintf(L"Унесите целобројни елемент који желите да убаците: ");
	wscanf(L"%d", &broj);
	fflush(stdin);
	ubaci(lista, broj, Pocetak);
}

void ubaci_element_na_kraj() {
	int broj;
	wprintf(L"Унесите целобројни елемент који желите да убаците: ");
	wscanf(L"%d", &broj);
	fflush(stdin);
	ubaci(lista, broj, Kraj);
}

void ubaci_element_sortirano() {
	int broj;
	wprintf(L"Унесите целобројни елемент који желите да убаците: ");
	wscanf(L"%d", &broj);
	fflush(stdin);
	ubaci(lista, broj, Vrednost);
}

void izbaci_element_sa_pocetka() {
	int izbaceni = -1;
	izbaci(lista, &izbaceni, Pocetak);
}

void izbaci_element_sa_kraja() {
	int izbaceni = -1;
	izbaci(lista, &izbaceni, Kraj);
}

void izbaci_element_po_vrednosti() {
	int broj_za_izbacivanje;
	wprintf(L"Унесите целобројни елемент који желите да избаците: ");
	wscanf(L"%d", &broj_za_izbacivanje);
	fflush(stdin);
	izbaci(lista, &broj_za_izbacivanje, Vrednost);
}

void sortiraj_elemente_rastuce_bubble() {
	sortiraj(lista, Rastuce, Bubble);
}

void sortiraj_elemente_rastuce_insertion() {
	sortiraj(lista, Rastuce, Insertion);
}

void sortiraj_elemente_rastuce_selection() {
	sortiraj(lista, Rastuce, Selection);
}

void sortiraj_elemente_opadajuce_bubble() {
	sortiraj(lista, Opadajuce, Bubble);
}

void sortiraj_elemente_opadajuce_insertion() {
	sortiraj(lista, Opadajuce, Insertion);
}

void sortiraj_elemente_opadajuce_selection() {
	sortiraj(lista, Opadajuce, Selection);
}

void pretrazi_element_sekvencijalno() {
	int broj;
	wprintf(L"Унесите целобројни елемент који желите да претражите: ");
	wscanf(L"%d", &broj);
	fflush(stdin);
	sadrzi(lista, broj, Sekvencijalno);
}

void pretrazi_element_binarno() {
	int broj;
	wprintf(L"Унесите целобројни елемент који желите да претражите: ");
	wscanf(L"%d", &broj);
	fflush(stdin);
	sadrzi(lista, broj, Binarno);
}

void dodeli_funkciju(MENI* meni, int i) {
	// da ne bismo radili mnogo if-else grana, pravimo mapu sa parovima opisa i funkcija
	//proverimo koji tacno opis je procitan iz datoteke i u skladu sa tim dodelimo odgovarajucu funkciju
	static const STAVKA_MENIJA mapa[] = {
		{ L"Убаци елемент", ubaci_element },
		{ L"Избаци елемент", izbaci_element },
		{ L"Прикажи елементе", prikazi_elemente },
		{ L"Сортирај елементе", sortiraj_elemente },
		{ L"Да ли је листа празна", da_li_je_lista_prazna },
		{ L"Претражи одређени елемент", pretrazi_element },
		{ L"Убаци елемент на почетак", ubaci_element_na_pocetak },
		{ L"Убаци елемент на крај", ubaci_element_na_kraj },
		{ L"Убаци елемент сортирано на основу вредности", ubaci_element_sortirano},
		{ L"Избаци елемент са почетка", izbaci_element_sa_pocetka},
		{ L"Избаци елемент са краја", izbaci_element_sa_kraja},
		{ L"Избаци елемент одређене вредности", izbaci_element_po_vrednosti},
		{ L"Сортирај елементе растуће користећи Bubble sort", sortiraj_elemente_rastuce_bubble},
		{ L"Сортирај елементе растуће користећи Insertion sort", sortiraj_elemente_rastuce_insertion},
		{ L"Сортирај елементе растуће користећи Selection sort", sortiraj_elemente_rastuce_selection},
		{ L"Сортирај елементе опадајуће користећи Bubble sort", sortiraj_elemente_opadajuce_bubble},
		{ L"Сортирај елементе опадајуће користећи Insertion sort", sortiraj_elemente_opadajuce_insertion},
		{ L"Сортирај елементе опадајуће користећи Selection sort", sortiraj_elemente_opadajuce_selection},
		{ L"Претражи елемент секвенцијално", pretrazi_element_sekvencijalno },
		{ L"Претражи елемент бинарно", pretrazi_element_binarno },
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

