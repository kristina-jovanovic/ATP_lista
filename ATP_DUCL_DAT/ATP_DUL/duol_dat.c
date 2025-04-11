#include "defs.h"
#include "lista.h"

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
		.ucitavanje = "Doslo je do problema prilikom UCITAVANJA elemenata liste iz datoteke.\n"
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

SIGNAL kreiraj(LISTA* lista) {
	SIGNAL signal;
	signal.status = Greska;
	signal.poruka = poruka.GRESKA.kreiraj;
	*lista = malloc(sizeof(struct lista));
	if (*lista == NULL) return signal;
	(*lista)->skladiste = "lista.dat";
	FILE* datoteka = fopen((*lista)->skladiste, "ab");
	if (datoteka == NULL) {
		printf("Greska pri otvaranju datoteke!\n");
		signal.status = Upozorenje;
		signal.poruka = poruka.UPOZORENJE.ucitavanje;
		return signal;
	}
	fseek(datoteka, 0, SEEK_END);
	int broj_bajtova = ftell(datoteka);
	if (broj_bajtova == 0) {
		(*lista)->broj_elemenata = 0;
	}
	else {
		(*lista)->broj_elemenata = (broj_bajtova - sizeof(int) * 2) / sizeof(ELEMENT);
	}
	(*lista)->kapacitet = 100;
	fclose(datoteka);

	signal.status = Info;
	signal.poruka = poruka.INFO.kreiraj;
	return signal;
}

SIGNAL unisti(LISTA* lista) {
	SIGNAL signal;
	signal.status = Greska;
	signal.poruka = poruka.GRESKA.unisti;
	if (*lista == NULL || (*lista)->skladiste == NULL || (*lista)->skladiste == ErrorList) return signal;
	remove((*lista)->skladiste);
	// da li ovako obrisati celu datoteku ili "izbaciti" sve tj. staviti da je glava=-1?
	// u tom slucaju bi sve fizicki ostalo i dalje u fajlu, tako da mislim da je mozda bolje obrisati ga skroz
	//(*lista)->skladiste = NULL;
	(*lista)->broj_elemenata = 0;

	signal.status = Info;
	signal.poruka = poruka.INFO.unisti;
	return signal;
}

SIGNAL ubaci_na_pocetak(LISTA* lista, PODATAK podatak) {
	SIGNAL signal;
	signal.status = Greska;
	signal.poruka = poruka.GRESKA.ubaci;
	if (*lista == NULL || (*lista)->broj_elemenata >= (*lista)->kapacitet) return signal;

	FILE* datoteka = fopen((*lista)->skladiste, "r+b"); //sa ab rezimom nije htelo
	if (datoteka == NULL) {
		datoteka = fopen((*lista)->skladiste, "w+b");
		if (datoteka == NULL) {
			printf("Greska pri otvaranju datoteke!\n");
			signal.status = Upozorenje;
			signal.poruka = poruka.UPOZORENJE.ucitavanje;
			return signal;
		}
	}
	//(*lista)->skladiste = datoteka;

	fseek(datoteka, 0, SEEK_END);	// postavljanje indikatora pozicije datoteke na kraj
	// Funkcija ftell() vraća koliko je bajtova od početka datoteke do indikatora tekuće pozicije u datoteci.
	// Ako se indikator pozicije prethodno postavi na kraj datoteke, na sledeći način možemo doći do 
	// broja elemenata koji se nalaze u datoteci.
	//int broj_slogova_u_datoteci = ftell(datoteka) / sizeof(ELEMENT);
	int broj_bajtova_u_datoteci = ftell(datoteka);
	int broj_bajtova_za_element = sizeof(ELEMENT);
	int glava = sizeof(int) * 2;

	ELEMENT novi = { NULL,podatak,NULL };

	if (broj_bajtova_u_datoteci == 0) { //lista je prazna, dodajemo prvi element
		//pre njega upisujemo velicinu sloga i broj bajta na kome ce biti prvi element (glava)

		novi.prethodni = (void*)(intptr_t)glava;
		novi.sledeci = (void*)(intptr_t)glava;

		fseek(datoteka, 0, SEEK_SET);
		fwrite(&broj_bajtova_za_element, sizeof(int), 1, datoteka);
		fwrite(&glava, sizeof(int), 1, datoteka);
		//fseek(datoteka, glava, SEEK_SET);
		fwrite(&novi, broj_bajtova_za_element, 1, datoteka);
	}
	else { //lista nije prazna
		fseek(datoteka, 0, SEEK_SET); //postavljamo indikator pozicije na pocetak
		fread(&broj_bajtova_za_element, sizeof(int), 1, datoteka);
		fread(&glava, sizeof(int), 1, datoteka);

		if (glava == -1) {
			//lista je ispraznjena, treba da dodamo opet prvi element
			glava = sizeof(int) * 2;
			novi.prethodni = (void*)(intptr_t)glava;
			novi.sledeci = (void*)(intptr_t)glava;
			fseek(datoteka, sizeof(int), SEEK_SET);
			fwrite(&glava, sizeof(int), 1, datoteka);
			fwrite(&novi, broj_bajtova_za_element, 1, datoteka);
			goto kraj_true;
		}

		ELEMENT stara_glava;
		fseek(datoteka, glava, SEEK_SET); //pozicioniramo se ispred elementa koji je trenutna glava
		fread(&stara_glava, broj_bajtova_za_element, 1, datoteka); // i citamo ga

		fseek(datoteka, 0, SEEK_END);
		int adresa_novog = ftell(datoteka); // ovo je naredna slobodna pozicija (kraj datoteke)

		ELEMENT poslednji;
		int adresa_poslednjeg = (int)(intptr_t)stara_glava.prethodni;
		fseek(datoteka, adresa_poslednjeg, SEEK_SET);
		fread(&poslednji, broj_bajtova_za_element, 1, datoteka);

		// azuriranje veza
		poslednji.sledeci = (void*)(intptr_t)adresa_novog;
		stara_glava.prethodni = (void*)(intptr_t)adresa_novog;
		novi.prethodni = (void*)(intptr_t)adresa_poslednjeg; //ovo je adresa
		novi.sledeci = (void*)(intptr_t)glava; //adresa glave

		//upisivanje novog elementa i izmena azuriranih
		fseek(datoteka, adresa_novog, SEEK_SET);
		fwrite(&novi, broj_bajtova_za_element, 1, datoteka);
		fseek(datoteka, glava, SEEK_SET);
		fwrite(&stara_glava, broj_bajtova_za_element, 1, datoteka);
		fseek(datoteka, adresa_poslednjeg, SEEK_SET);
		fwrite(&poslednji, broj_bajtova_za_element, 1, datoteka);

		//azuriranje adrese glave (nove) na pocetku datoteke u "metapodacima"
		glava = adresa_novog;
		fseek(datoteka, sizeof(int), SEEK_SET); //preskacemo sizeof(int) od pocetka jer je na toj prvoj poziciji sacuvana velicina elementa liste, a na drugoj je adresa glave
		fwrite(&glava, sizeof(int), 1, datoteka);
	}
kraj_true:
	(*lista)->broj_elemenata++;
	fclose(datoteka);
	signal.status = Info;
	signal.poruka = poruka.INFO.ubaci;
	return signal;
}

SIGNAL izbaci_sa_pocetka(LISTA* lista, PODATAK* podatak) {
	SIGNAL signal;
	signal.status = Greska;
	signal.poruka = poruka.GRESKA.izbaci;
	if (*lista == NULL || (*lista)->skladiste == NULL || (*lista)->skladiste == ErrorList
		|| (*lista)->broj_elemenata == 0) return signal;

	FILE* datoteka = fopen((*lista)->skladiste, "r+b");
	if (datoteka == NULL) {
		printf("Greska pri otvaranju datoteke!\n");
		signal.status = Upozorenje;
		signal.poruka = poruka.UPOZORENJE.ucitavanje;
		return signal;
	}

	fseek(datoteka, 0, SEEK_END);
	int broj_bajtova_u_datoteci = ftell(datoteka);

	if (broj_bajtova_u_datoteci == 0) goto kraj_false; // lista je prazna, ne sadrzi cak ni metapodatke, nema sta da se izbaci

	int broj_bajtova_za_element;
	int glava;
	fseek(datoteka, 0, SEEK_SET);
	fread(&broj_bajtova_za_element, sizeof(int), 1, datoteka);
	fread(&glava, sizeof(int), 1, datoteka);

	if (glava == -1) goto kraj_false; //lista je prazna

	ELEMENT prvi;
	fseek(datoteka, glava, SEEK_SET);
	fread(&prvi, broj_bajtova_za_element, 1, datoteka);
	*podatak = prvi.podatak;

	if ((int)(intptr_t)prvi.prethodni == glava && (int)(intptr_t)prvi.sledeci == glava) {
		// u listi se nalazi samo jedan element
		prvi.prethodni = (void*)(intptr_t)-1;
		prvi.sledeci = (void*)(intptr_t)-1;
		prvi.podatak = -1;
		glava = -1; //lista je prazna
		fseek(datoteka, sizeof(int), SEEK_SET);
		fwrite(&glava, sizeof(int), 1, datoteka);
		goto kraj_true;
	}

	ELEMENT poslednji;
	int adresa_poslednjeg = (int)(intptr_t)prvi.prethodni;
	fseek(datoteka, adresa_poslednjeg, SEEK_SET);
	fread(&poslednji, broj_bajtova_za_element, 1, datoteka);

	ELEMENT nova_glava;
	int adresa_nove_glave = (int)(intptr_t)prvi.sledeci;
	fseek(datoteka, adresa_nove_glave, SEEK_SET);
	fread(&nova_glava, broj_bajtova_za_element, 1, datoteka);

	poslednji.sledeci = (void*)(intptr_t)adresa_nove_glave;
	nova_glava.prethodni = (void*)(intptr_t)adresa_poslednjeg;
	// ovo je samo logicko brisanje, ali element ce ostati u memoriji i dalje... mozda ubaciti
	// u metapodatke 'prvi_slobodan' gde ce se cuvati njegov offset tako da mozemo da ponovo
	// koristimo taj prostor
	prvi.prethodni = (void*)(intptr_t)-1;
	prvi.sledeci = (void*)(intptr_t)-1;
	prvi.podatak = -1;

	fseek(datoteka, glava, SEEK_SET);
	fwrite(&prvi, broj_bajtova_za_element, 1, datoteka); //ovo je izbrisani
	fseek(datoteka, adresa_nove_glave, SEEK_SET);
	fwrite(&nova_glava, broj_bajtova_za_element, 1, datoteka);
	fseek(datoteka, adresa_poslednjeg, SEEK_SET);
	fwrite(&poslednji, broj_bajtova_za_element, 1, datoteka);

	glava = adresa_nove_glave;
	fseek(datoteka, sizeof(int), SEEK_SET);
	fwrite(&glava, sizeof(int), 1, datoteka);

kraj_true:
	(*lista)->broj_elemenata--;
	fclose(datoteka);
	signal.status = Info;
	signal.poruka = poruka.INFO.izbaci;
	return signal;
kraj_false: //lista je prazna, nema sta da se izbaci
	fclose(datoteka);
	signal.status = Upozorenje;
	signal.poruka = poruka.UPOZORENJE.izbaci;
	return signal;
}

void prikazi(LISTA lista) {
	printf("\n///// Lista: ");
	if ((lista == NULL) || (lista->skladiste == NULL) || (lista->skladiste == ErrorList)) {
		(lista == NULL || lista->skladiste == NULL) ? printf("< Null > \n") : printf("< ErrorList > \n");
		return;
	}
	FILE* datoteka = fopen(lista->skladiste, "rb");
	if (datoteka == NULL) {
		printf("Greska pri otvaranju datoteke!\n");
		return;
	}

	fseek(datoteka, 0, SEEK_END);
	int broj_bajtova = ftell(datoteka);
	if (broj_bajtova <= (sizeof(int) * 2)) {
		printf("< Prazno >\n");
		fclose(datoteka);
		return;
	}

	int adresa_glave, adresa_trenutnog;
	fseek(datoteka, sizeof(int), SEEK_SET); //pomeramo se za velicinu jednog int-a od pocetka
	fread(&adresa_glave, sizeof(int), 1, datoteka); //citamo adresu glave
	if (adresa_glave <= -1) {
		printf("< Prazno >\n");
		fclose(datoteka);
		return;
	}
	ELEMENT trenutni;
	adresa_trenutnog = adresa_glave;
	do {
		fseek(datoteka, adresa_trenutnog, SEEK_SET);
		fread(&trenutni, sizeof(ELEMENT), 1, datoteka);
		printf("%d ", trenutni.podatak);
		adresa_trenutnog = (int)(intptr_t)trenutni.sledeci;
	} while (adresa_trenutnog != adresa_glave);
	printf("(ciklicno)\n");

	fclose(datoteka);
}

SIGNAL sortiraj(LISTA* lista) {
	SIGNAL signal;
	signal.status = Greska;
	signal.poruka = poruka.GRESKA.sortiraj;
	if (*lista == NULL || (*lista)->skladiste == NULL || (*lista)->skladiste == ErrorList
		|| (*lista)->broj_elemenata == 0) return signal;

	FILE* datoteka = fopen((*lista)->skladiste, "r+b");
	if (datoteka == NULL) {
		printf("Greska pri otvaranju datoteke!\n");
		signal.status = Upozorenje;
		signal.poruka = poruka.UPOZORENJE.ucitavanje;
		return signal;
	}

	fseek(datoteka, 0, SEEK_END);
	int broj_bajtova = ftell(datoteka);
	if (broj_bajtova <= (sizeof(int) * 2)) {
		//printf("< Prazno >\n");
		fclose(datoteka);
		signal.status = Upozorenje;
		signal.poruka = poruka.UPOZORENJE.sortiraj;
		return signal;
	}

	int broj_bajtova_za_element, adresa_glave, adresa_prvog, adresa_drugog;
	fseek(datoteka, 0, SEEK_SET); //pomeramo se na pocetak
	fread(&broj_bajtova_za_element, sizeof(int), 1, datoteka);
	fread(&adresa_glave, sizeof(int), 1, datoteka); //citamo adresu glave
	if (adresa_glave <= -1) {
		//printf("< Prazno >\n");
		fclose(datoteka);
		signal.status = Upozorenje;
		signal.poruka = poruka.UPOZORENJE.sortiraj;
		return signal;
	}
	ELEMENT prvi, drugi;
	adresa_prvog = adresa_glave;
	fseek(datoteka, adresa_prvog, SEEK_SET);
	fread(&prvi, broj_bajtova_za_element, 1, datoteka);

	do {
		adresa_drugog = (int)(intptr_t)prvi.sledeci;
		do {
			fseek(datoteka, adresa_drugog, SEEK_SET);
			fread(&drugi, broj_bajtova_za_element, 1, datoteka);
			if (prvi.podatak < drugi.podatak) {
				//potrebno je zameniti vrednosti
				int pom = prvi.podatak;
				prvi.podatak = drugi.podatak;
				drugi.podatak = pom;

				//azuriranje vrednosti u datoteci
				fseek(datoteka, adresa_prvog, SEEK_SET);
				fwrite(&prvi, broj_bajtova_za_element, 1, datoteka);
				fseek(datoteka, adresa_drugog, SEEK_SET);
				fwrite(&drugi, broj_bajtova_za_element, 1, datoteka);
			}
			adresa_drugog = (int)(intptr_t)drugi.sledeci;
		} while (adresa_drugog != adresa_glave);
		adresa_prvog = (int)(intptr_t)prvi.sledeci;
		fseek(datoteka, adresa_prvog, SEEK_SET);
		fread(&prvi, broj_bajtova_za_element, 1, datoteka);
	} while ((int)(intptr_t)prvi.sledeci != adresa_glave);

	fclose(datoteka);
	signal.status = Info;
	signal.poruka = poruka.INFO.sortiraj;
	return signal;
}

SIGNAL prazna(LISTA lista) {
	SIGNAL signal;
	signal.status = Info;
	if (lista == NULL || lista->skladiste == NULL || lista->skladiste == ErrorList
		|| lista->broj_elemenata == 0)
		signal.poruka = poruka.INFO.lista_prazna;

	signal.poruka = poruka.INFO.lista_nije_prazna;
	return signal;
}

SIGNAL sadrzi(LISTA lista, PODATAK podatak) {
	SIGNAL signal;
	signal.status = Info;
	if (lista == NULL || lista->skladiste == NULL || lista->skladiste == ErrorList
		|| lista->broj_elemenata == 0) {
		signal.poruka = poruka.INFO.podatak_ne_postoji;
		return signal;
	}

	FILE* datoteka = fopen(lista->skladiste, "rb");
	if (datoteka == NULL) {
		printf("Greska pri otvaranju datoteke!\n");
		signal.status = Upozorenje;
		signal.poruka = poruka.UPOZORENJE.ucitavanje;
		return signal;
	}
	int adresa_glave, adresa_trenutnog;
	fseek(datoteka, sizeof(int), SEEK_SET); //pomeramo se za velicinu jednog int-a od pocetka
	fread(&adresa_glave, sizeof(int), 1, datoteka); //citamo adresu glave
	if (adresa_glave == -1) {
		printf("< Prazno >\n");
		signal.poruka = poruka.INFO.podatak_ne_postoji;
		return signal;
	}
	ELEMENT trenutni;
	adresa_trenutnog = adresa_glave;
	do {
		fseek(datoteka, adresa_trenutnog, SEEK_SET);
		fread(&trenutni, sizeof(ELEMENT), 1, datoteka);
		if (trenutni.podatak == podatak) {
			fclose(datoteka);
			signal.poruka = poruka.INFO.podatak_postoji;
			return signal;
		}
		adresa_trenutnog = (int)(intptr_t)trenutni.sledeci;
	} while (adresa_trenutnog != adresa_glave);

	fclose(datoteka);
	signal.poruka = poruka.INFO.podatak_ne_postoji;
	return signal;
}

//main
int main() {

	LISTA lista = NULL;
	SIGNAL signal;
	signal = kreiraj(&lista);
	obrada_statusa(signal.status, signal.poruka, lista->skladiste, __LINE__);
	//unisti(&lista) ? printf("Lista je uspesno unistena\n") : printf("Lista nije uspesno unistena\n");
	int a = 7;
	signal = ubaci_na_pocetak(&lista, a);
	obrada_statusa(signal.status, signal.poruka, lista->skladiste, __LINE__);

	int b = 2;
	signal = ubaci_na_pocetak(&lista, b);
	obrada_statusa(signal.status, signal.poruka, lista->skladiste, __LINE__);

	int c = 5;
	signal = ubaci_na_pocetak(&lista, c);
	obrada_statusa(signal.status, signal.poruka, lista->skladiste, __LINE__);

	prikazi(lista);

	//int izbaceni;
	//izbaci_sa_pocetka(&lista, &izbaceni) ? printf("Podatak %d je uspesno izbacen\n", izbaceni) : printf("Podatak nije uspesno izbacen\n");
	//izbaci_sa_pocetka(&lista, &izbaceni) ? printf("Podatak %d je uspesno izbacen\n", izbaceni) : printf("Podatak nije uspesno izbacen\n");
	//prikazi(lista);

	//lista->skladiste = NULL;
	signal = prazna(lista);
	obrada_statusa(signal.status, signal.poruka, lista->skladiste, __LINE__);

	signal = sadrzi(lista, 4);
	obrada_statusa(signal.status, signal.poruka, lista->skladiste, __LINE__);

	signal = sortiraj(&lista);
	obrada_statusa(signal.status, signal.poruka, lista->skladiste, __LINE__);

	prikazi(lista);

	return 0;
}