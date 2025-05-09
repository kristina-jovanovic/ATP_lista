
// implementacija dela lista.h koji je zajednicki za svaki nacin implementacije liste
#include "lista.h"
#include "meni.h"

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
void dodeli_funkciju_lista(MENI* meni);

// globalna promenljiva
LISTA lista = NULL;

const UTF8_STRING nadji_poruku(KOD kod) {
	switch (kod) {
		// Greska
	case 0: return Poruka.Greska.Kreiraj;
	case 1: return Poruka.Greska.Unisti;
	case 2: return Poruka.Greska.Ubaci;
	case 3: return Poruka.Greska.Lista_ne_postoji;
	case 4: return Poruka.Greska.Ucitavanje_datoteke;

		// Upozorenje
	case 10: return Poruka.Upozorenje.Sortiraj;
	case 11: return Poruka.Upozorenje.Ubaci;
	case 12: return Poruka.Upozorenje.Izbaci;
	case 13: return Poruka.Upozorenje.Lista_prazna;

		// Info
	case 20: return Poruka.Info.Kreiraj;
	case 21: return Poruka.Info.Unisti;
	case 22: return Poruka.Info.Ubaci;
	case 23: return Poruka.Info.Izbaci;
	case 24: return Poruka.Info.Podatak_postoji;
	case 25: return Poruka.Info.Podatak_ne_postoji;
	case 26: return Poruka.Info.Lista_prazna;
	case 27: return Poruka.Info.Lista_nije_prazna;
	case 28: return Poruka.Info.Sortiraj;

	default: return L"Непознат статус код.";
	}
}

void prijavi(KOD kod, const STRING datoteka, int linija, ...) {
	STATUS status;
	wchar_t w_datoteka[260];
	mbstowcs(w_datoteka, datoteka, 260); //multibyte string to wide char string, 260 je kapacitet w_datoteka

	if (kod >= 1 && kod < 10)
		status = Greska;
	else if (kod >= 10 && kod < 20)
		status = Upozorenje;
	else
		status = Info;

	va_list args;
	va_start(args, linija);

	const UTF8_STRING poruka = nadji_poruku(kod);
	switch (status)
	{
	case Info:
		fwprintf(stdout, L"[ИНФО] ");
		vfwprintf(stdout, poruka, args);
		fwprintf(stdout, L"\n");
		break;
	case Upozorenje:
		fwprintf(stderr, L"[УПОЗОРЕЊЕ] ");
		vfwprintf(stderr, poruka, args);
		fwprintf(stderr, L"\nДАТОТЕКА: %ls, ЛИНИЈА: %d\n", w_datoteka, linija);
		break;
	case Greska:
		fwprintf(stderr, L"[ГРЕШКА] ");
		vfwprintf(stderr, poruka, args);
		fwprintf(stderr, L"\nДАТОТЕКА: %ls, ЛИНИЈА: %d\n", w_datoteka, linija);
		fwprintf(stderr, L"Програм ће бити прекинут.\n");
		exit(EXIT_FAILURE);
		break;
	default:
		break;
	}

	va_end(args);
}

const struct status_kodovi Kod = {
	.Greska = {.Kreiraj = 0, .Unisti = 1, .Ubaci = 2, .Lista_ne_postoji = 3, .Ucitavanje_datoteke = 4 },
	.Upozorenje = {.Sortiraj = 10,.Ubaci = 11,.Izbaci = 12, .Lista_prazna = 13 },
	.Info = {.Kreiraj = 20, .Unisti = 21, .Ubaci = 22, .Izbaci = 23, .Podatak_postoji = 24,
			.Podatak_ne_postoji = 25, .Lista_prazna = 26, .Lista_nije_prazna = 27, .Sortiraj = 28 }
};

const struct status_poruke Poruka = {
	.Greska = {
		.Kreiraj = L"Није могуће креирати листу.",
		.Unisti = L"Није могуће уништити листу.",
		.Ubaci = L"Није могуће убацити елемент у листу.",
		.Lista_ne_postoji = L"Није могуће извршити операцију јер листа не постоји.",
		.Ucitavanje_datoteke = L"Неуспешно учитавање датотеке.",
	},
	.Upozorenje = {
		.Sortiraj = L"Није могуће сортирати листу јер има мање од 2 елемента.",
		.Ubaci = L"Није могуће убацити елемент у листу јер је достигнут максимални капацитет.",
		.Izbaci = L"Није могуће избацити елемент са вредношћу %d јер не постоји у листи.",
		.Lista_prazna = L"Није могуће извршити операцију јер је листа празна.",
	},
	.Info = {
		.Kreiraj = L"Листа је успешно креирана.",
		.Unisti = L"Листа је успешно уништена.",
		.Ubaci = L"Елемент је успешно убачен у листу.",
		.Izbaci = L"Елемент са вредношћу %d је успешно избачен из листе.",
		.Podatak_postoji = L"Елемент %d постоји у листи.",
		.Podatak_ne_postoji = L"Елемент %d не постоји у листи.",
		.Lista_prazna = L"Листа је празна.",
		.Lista_nije_prazna = L"Листа није празна.",
		.Sortiraj = L"Листа је успешно сортирана %s.",
	}
};

// implementacija pomocnih funkcija
void ubaci_element() {
	MENI meni_ubaci;
	kreiraj_meni(&meni_ubaci, "../../meni_datoteke/meni_ubaci.txt");
	dodeli_funkciju_lista(&meni_ubaci);
	pokreni_meni(meni_ubaci);
}

void izbaci_element() {
	MENI meni_izbaci;
	kreiraj_meni(&meni_izbaci, "../../meni_datoteke/meni_izbaci.txt");
	dodeli_funkciju_lista(&meni_izbaci);
	pokreni_meni(meni_izbaci);
}

void prikazi_elemente() {
	prikazi(lista);
}

void sortiraj_elemente() {
	MENI meni_sortiraj;
	kreiraj_meni(&meni_sortiraj, "../../meni_datoteke/meni_sortiraj.txt");
	dodeli_funkciju_lista(&meni_sortiraj);
	pokreni_meni(meni_sortiraj);
}

void da_li_je_lista_prazna() {
	prazna(lista);
}

void pretrazi_element() {
	MENI meni_pretrazi;
	kreiraj_meni(&meni_pretrazi, "../../meni_datoteke/meni_pretrazivanje.txt");
	dodeli_funkciju_lista(&meni_pretrazi);
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

void dodeli_funkciju_lista(MENI* meni) {
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

	// ako prvi put dodeljujemo funkcije - kreiraj listu
	if (lista == NULL) {
		kreiraj(&lista);
	}
}