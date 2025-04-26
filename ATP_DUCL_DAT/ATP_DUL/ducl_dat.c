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

//specifikacija pomocnih funkcija
SIGNAL izbaci_sa_pocetka(LISTA*, PODATAK*);
SIGNAL izbaci_sa_kraja(LISTA*, PODATAK*);
SIGNAL izbaci_po_vrednosti(LISTA*, PODATAK*);
void azuriraj_susede(FILE*, int, int, int);
void skrati_datoteku(FILE*, int);
SIGNAL bubble_sort(FILE*, int, int, SMER_SORTIRANJA);
SIGNAL insertion_sort(FILE*, int, int, SMER_SORTIRANJA);
SIGNAL selection_sort(FILE*, int, int, SMER_SORTIRANJA);

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

SIGNAL ubaci(LISTA* lista, PODATAK podatak, NACIN nacin) {
	SIGNAL signal;
	signal.status = Greska;
	signal.poruka = poruka.GRESKA.ubaci;
	if (*lista == NULL || (*lista)->broj_elemenata >= (*lista)->kapacitet) return signal;

	FILE* datoteka = fopen((*lista)->skladiste, "r+b");
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

	if (nacin == Vrednost) {
		if (broj_bajtova_u_datoteci == 0)
			goto pocetak; // ako je datoteka prazna, isto je kao da dodajemo prvi element (na pocetak)
		//lista nije prazna, tj. postoje barem metapodaci
		fseek(datoteka, 0, SEEK_SET); //postavljamo indikator pozicije na pocetak
		fread(&broj_bajtova_za_element, sizeof(int), 1, datoteka);
		fread(&glava, sizeof(int), 1, datoteka);
		if (glava == -1)
			goto ispraznjena_lista; //lista je prazna ali postoje metapodaci, opet je isto kao da dodajemo element na pocetak

		// ako se nalazimo u ovom delu koda, znaci da postoje neki elementi u listi
		// za ovaj nacin unosa elementa, lista treba da bude sortirana i zatim ubacujemo element tamo
		// gde pripada po vrednosti
		fflush(datoteka);
		fclose(datoteka);
		sortiraj(lista, Rastuce, Bubble); ////////////////
		datoteka = fopen((*lista)->skladiste, "r+b");
		if (datoteka == NULL) {
			printf("Greska pri otvaranju datoteke!\n");
			signal.status = Upozorenje;
			signal.poruka = poruka.UPOZORENJE.ucitavanje;
			return signal;
		}

		fseek(datoteka, 0, SEEK_END);
		int adresa_novog = ftell(datoteka); // ovo je naredna slobodna pozicija (kraj datoteke)

		//ako lista ima samo jedan element
		if ((*lista)->broj_elemenata == 1) {
			ELEMENT prvi;
			fseek(datoteka, glava, SEEK_SET);
			fread(&prvi, broj_bajtova_za_element, 1, datoteka);
			if (prvi.podatak >= podatak) {
				//znaci novi treba na pocetak
				goto pocetak;
			}
			else {
				//znaci novi treba na kraj
				goto kraj;
			}
		}

		ELEMENT trenutni;
		int adresa_trenutnog = glava;
		do {
			fseek(datoteka, adresa_trenutnog, SEEK_SET);
			fread(&trenutni, sizeof(ELEMENT), 1, datoteka);
			if (trenutni.podatak > podatak) {
				break;
			}
			adresa_trenutnog = (int)(intptr_t)trenutni.sledeci;
		} while (adresa_trenutnog != glava);
		if (adresa_trenutnog == glava)
			goto kraj;
		//treba da postavimo novi element pre trenutnog
		int adresa_prethodnog = (int)(intptr_t)trenutni.prethodni;
		ELEMENT prethodni;
		fseek(datoteka, adresa_prethodnog, SEEK_SET);
		fread(&prethodni, broj_bajtova_za_element, 1, datoteka);

		//azuriranje veza
		prethodni.sledeci = (void*)(intptr_t)adresa_novog;
		trenutni.prethodni = (void*)(intptr_t)adresa_novog;
		novi.prethodni = (void*)(intptr_t)adresa_prethodnog;
		novi.sledeci = (void*)(intptr_t)adresa_trenutnog;

		//upisivanje novog elementa i azuriranje trenutnog i prethodnog
		fseek(datoteka, adresa_prethodnog, SEEK_SET);
		fwrite(&prethodni, broj_bajtova_za_element, 1, datoteka);
		fseek(datoteka, adresa_novog, SEEK_SET);
		fwrite(&novi, broj_bajtova_za_element, 1, datoteka);
		fseek(datoteka, adresa_trenutnog, SEEK_SET);
		fwrite(&trenutni, broj_bajtova_za_element, 1, datoteka);
	}
	if (nacin == Kraj) {
		if (broj_bajtova_u_datoteci == 0)
			goto pocetak; // ako je datoteka prazna, isto je kao da dodajemo prvi element (na pocetak)
		//lista nije prazna
		fseek(datoteka, 0, SEEK_SET); //postavljamo indikator pozicije na pocetak
		fread(&broj_bajtova_za_element, sizeof(int), 1, datoteka);
		fread(&glava, sizeof(int), 1, datoteka);
	kraj:
		if (glava == -1)
			goto ispraznjena_lista; //lista je prazna ali postoje metapodaci, opet je isto kao da dodajemo element na pocetak

		// ako se nalazimo u ovom delu koda, znaci da postoje neki elementi u listi
		ELEMENT glava_el;
		fseek(datoteka, glava, SEEK_SET);
		fread(&glava_el, broj_bajtova_za_element, 1, datoteka);

		fseek(datoteka, 0, SEEK_END);
		int adresa_novog = ftell(datoteka); // ovo je naredna slobodna pozicija (kraj datoteke)

		int adresa_poslednjeg = (int)(intptr_t)glava_el.prethodni;
		if (adresa_poslednjeg == glava) {

			//azuriranje veza
			novi.sledeci = (void*)(intptr_t)glava;
			novi.prethodni = (void*)(intptr_t)adresa_poslednjeg;
			glava_el.prethodni = (void*)(intptr_t)adresa_novog;
			glava_el.sledeci = (void*)(intptr_t)adresa_novog;
		}
		else {
			ELEMENT poslednji;
			fseek(datoteka, adresa_poslednjeg, SEEK_SET);
			fread(&poslednji, broj_bajtova_za_element, 1, datoteka);

			//azuriranje veza
			novi.sledeci = (void*)(intptr_t)glava;
			novi.prethodni = (void*)(intptr_t)adresa_poslednjeg;
			glava_el.prethodni = (void*)(intptr_t)adresa_novog;
			poslednji.sledeci = (void*)(intptr_t)adresa_novog;

			fseek(datoteka, adresa_poslednjeg, SEEK_SET);
			fwrite(&poslednji, broj_bajtova_za_element, 1, datoteka);
		}


		//upisivanje novog elementa i izmena azuriranih
		fseek(datoteka, glava, SEEK_SET);
		fwrite(&glava_el, broj_bajtova_za_element, 1, datoteka);
		fseek(datoteka, adresa_novog, SEEK_SET);
		fwrite(&novi, broj_bajtova_za_element, 1, datoteka);

		//glava ostaje ista tako da ne moramo da menjamo to u metapodacima

	}
	if (nacin == Pocetak) {
	pocetak:
		if (broj_bajtova_u_datoteci == 0) {//lista je prazna, dodajemo prvi element
			//pre njega upisujemo velicinu sloga i broj bajta na kome ce biti prvi element (glava)
			novi.prethodni = (void*)(intptr_t)glava;
			novi.sledeci = (void*)(intptr_t)glava;

			fseek(datoteka, 0, SEEK_SET);
			fwrite(&broj_bajtova_za_element, sizeof(int), 1, datoteka);
			fwrite(&glava, sizeof(int), 1, datoteka);
			//fseek(datoteka, glava, SEEK_SET);
			fwrite(&novi, broj_bajtova_za_element, 1, datoteka);
		}
		else {
			//lista nije prazna
			fseek(datoteka, 0, SEEK_SET); //postavljamo indikator pozicije na pocetak
			fread(&broj_bajtova_za_element, sizeof(int), 1, datoteka);
			fread(&glava, sizeof(int), 1, datoteka);

			if (glava == -1) {
				//lista je ispraznjena, treba da dodamo opet prvi element
			ispraznjena_lista:
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

			//ako postoji samo jedan element (stara glava i prethodni su jednaki)
		/*	if (stara_glava.prethodni == poslednji.prethodni &&
				stara_glava.sledeci == poslednji.sledeci &&
				stara_glava.podatak == poslednji.podatak) {

			}*/
			if ((*lista)->broj_elemenata == 1) {
				// azuriranje veza
				stara_glava.prethodni = (void*)(intptr_t)adresa_novog;
				stara_glava.sledeci = (void*)(intptr_t)adresa_novog;
				novi.prethodni = (void*)(intptr_t)glava;
				novi.sledeci = (void*)(intptr_t)glava;

				//upisivanje novog elementa i izmena azuriranih
				fseek(datoteka, adresa_novog, SEEK_SET);
				fwrite(&novi, broj_bajtova_za_element, 1, datoteka);
				fseek(datoteka, glava, SEEK_SET);
				fwrite(&stara_glava, broj_bajtova_za_element, 1, datoteka);
			}
			else {
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
			}

			//azuriranje adrese glave (nove) na pocetku datoteke u "metapodacima"
			glava = adresa_novog;
			fseek(datoteka, sizeof(int), SEEK_SET); //preskacemo sizeof(int) od pocetka jer je na toj prvoj poziciji sacuvana velicina elementa liste, a na drugoj je adresa glave
			fwrite(&glava, sizeof(int), 1, datoteka);
		}
	}

kraj_true:
	(*lista)->broj_elemenata++;
	fclose(datoteka);
	signal.status = Info;
	signal.poruka = poruka.INFO.ubaci;
	return signal;
}

SIGNAL izbaci(LISTA* lista, PODATAK* podatak, NACIN nacin) {
	SIGNAL signal;
	signal.status = Greska;
	signal.poruka = poruka.GRESKA.izbaci;
	if (*lista == NULL || (*lista)->skladiste == NULL || (*lista)->skladiste == ErrorList
		|| (*lista)->broj_elemenata == 0) return signal;

	if (nacin == Pocetak)
		signal = izbaci_sa_pocetka(lista, podatak);
	if (nacin == Kraj)
		signal = izbaci_sa_kraja(lista, podatak);
	if (nacin == Vrednost)
		signal = izbaci_po_vrednosti(lista, podatak);

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

SIGNAL sortiraj(LISTA* lista, SMER_SORTIRANJA smer, ALGORITAM_SORTIRANJA algoritam) {
	SIGNAL signal;
	signal.status = Greska;
	signal.poruka = poruka.GRESKA.sortiraj;
	if (*lista == NULL || (*lista)->skladiste == NULL || (*lista)->skladiste == ErrorList)
		return signal;
	if ((*lista)->broj_elemenata == 0) {
		//nema elemenata u listi
		signal.status = Upozorenje;
		signal.poruka = poruka.UPOZORENJE.sortiraj;
		return signal;
	}

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

	//fclose(datoteka);

	if (algoritam == Bubble)
		signal = bubble_sort(datoteka, adresa_glave, broj_bajtova_za_element, smer);
	if (algoritam == Insertion)
		signal = insertion_sort(datoteka, adresa_glave, broj_bajtova_za_element, smer);
	if (algoritam == Selection)
		signal = selection_sort(datoteka, adresa_glave, broj_bajtova_za_element, smer);


	fclose(datoteka);
	return signal;
}

SIGNAL prazna(LISTA lista) {
	SIGNAL signal;
	signal.status = Info;
	if (lista == NULL || lista->skladiste == NULL || lista->skladiste == ErrorList
		|| lista->broj_elemenata == 0)
		signal.poruka = poruka.INFO.lista_prazna;
	else
		signal.poruka = poruka.INFO.lista_nije_prazna;
	return signal;
}

SIGNAL sadrzi(LISTA* lista, PODATAK podatak, VRSTA_PRETRAGE vrsta) {
	SIGNAL signal;
	signal.status = Info;
	if (lista == NULL || (*lista)->skladiste == NULL || (*lista)->skladiste == ErrorList
		|| (*lista)->broj_elemenata == 0) {
		signal.poruka = poruka.INFO.podatak_ne_postoji;
		return signal;
	}

	FILE* datoteka = fopen((*lista)->skladiste, "rb");
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

	//binarno pretrazivanje se ne moze vrsiti kada je lista implementirana preko datoteke, pa cemo raditi
	//iterativni pristup bez obzira na vrstu pretrazivanja koja je prosledjena

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

//implementacija pomocnih funkcija 

SIGNAL izbaci_sa_pocetka(LISTA* lista, PODATAK* podatak) {
	SIGNAL signal;

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

	int prethodni = (int)(intptr_t)prvi.prethodni;
	int sledeci = (int)(intptr_t)prvi.sledeci;
	int adresa_fizicki_poslednjeg = broj_bajtova_u_datoteci - broj_bajtova_za_element;

	if (prethodni == glava && sledeci == glava) { //ili (*lista)->broj_elemenata == 1
		// u listi se nalazi samo jedan element
		prvi.prethodni = (void*)(intptr_t)-1;
		prvi.sledeci = (void*)(intptr_t)-1;
		prvi.podatak = -1;
		//ovo iznad ni ne mora jer cemo svakako skratiti datoteku i obrisati ga

		glava = -1; //lista je prazna
		fseek(datoteka, sizeof(int), SEEK_SET);
		fwrite(&glava, sizeof(int), 1, datoteka);
		goto kraj_true;
	}
	else if (prethodni == sledeci && glava != adresa_fizicki_poslednjeg) {
		//ili (*lista)->broj_elemenata == 2
		ELEMENT drugi;
		fseek(datoteka, sledeci, SEEK_SET);
		fread(&drugi, broj_bajtova_za_element, 1, datoteka);

		//posto njegovi prethodni i sledeci vec pokazuju na glavu, samo cemo ga upisati na mesto prvog
		fseek(datoteka, glava, SEEK_SET);
		fwrite(&drugi, broj_bajtova_za_element, 1, datoteka);

		//adresa glave ostaje ista

		goto kraj_true;
	}

	if (glava == adresa_fizicki_poslednjeg) {
		azuriraj_susede(datoteka, glava, sledeci, broj_bajtova_za_element);

		glava = sledeci;
		fseek(datoteka, sizeof(int), SEEK_SET);
		fwrite(&glava, sizeof(int), 1, datoteka);

		goto kraj_true;
	}
	else {
		ELEMENT nova_glava;
		fseek(datoteka, sledeci, SEEK_SET);
		fread(&nova_glava, broj_bajtova_za_element, 1, datoteka);
		ELEMENT poslednji;
		fseek(datoteka, prethodni, SEEK_SET);
		fread(&poslednji, broj_bajtova_za_element, 1, datoteka);

		// 1) azuriraj susede originalnog prvog da pokazuju na novu glavu
		azuriraj_susede(datoteka, glava, sledeci, broj_bajtova_za_element);
		//?

		// 2) fizicko prevezivanje: premestamo fizicki poslednji na mesto glave
		ELEMENT fizicki_poslednji;
		fseek(datoteka, adresa_fizicki_poslednjeg, SEEK_SET);
		fread(&fizicki_poslednji, broj_bajtova_za_element, 1, datoteka);
		// azuriraj sve koji su pokazivali na fizicki poslednji da sada pokazuju na glavu, jer ce mu tu biti novo mesto
		azuriraj_susede(datoteka, adresa_fizicki_poslednjeg, glava, broj_bajtova_za_element);
		// upisi fizicki poslednji na mesto glave
		fseek(datoteka, glava, SEEK_SET);
		fwrite(&fizicki_poslednji, broj_bajtova_za_element, 1, datoteka);

		// 3) logicko prevezivanje: azuriraj veze pre i posle stare glave
		nova_glava.prethodni = (void*)(intptr_t)prethodni;
		poslednji.sledeci = (void*)(intptr_t)sledeci;
		fseek(datoteka, prethodni, SEEK_SET);
		fwrite(&poslednji, broj_bajtova_za_element, 1, datoteka);
		fseek(datoteka, sledeci, SEEK_SET);
		fwrite(&nova_glava, broj_bajtova_za_element, 1, datoteka);

		// postavi novu glavu
		glava = sledeci;
		fseek(datoteka, sizeof(int), SEEK_SET);
		fwrite(&glava, sizeof(int), 1, datoteka);

		goto kraj_true;
	}

kraj_true:
	(*lista)->broj_elemenata--;
	skrati_datoteku(datoteka, broj_bajtova_za_element);
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

SIGNAL izbaci_sa_kraja(LISTA* lista, PODATAK* podatak) {
	SIGNAL signal;

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

	int adresa_poslednjeg = (int)(intptr_t)prvi.prethodni;
	ELEMENT poslednji;
	fseek(datoteka, adresa_poslednjeg, SEEK_SET);
	fread(&poslednji, broj_bajtova_za_element, 1, datoteka);
	*podatak = poslednji.podatak;

	int adresa_fizicki_poslednjeg = broj_bajtova_u_datoteci - broj_bajtova_za_element;
	ELEMENT fizicki_poslednji;

	if ((*lista)->broj_elemenata == 1) {
		//lista ima jedan element, znaci samo cemo izmeniti metapodatak za glavu 
		glava = -1;
		fseek(datoteka, sizeof(int), SEEK_SET);
		fwrite(&glava, sizeof(int), 1, datoteka);
		goto kraj_true;
	}
	else {
		//ovaj deo radimo u svakom slucaju, i ako je poslednji ujedno i fizicki poslednji, i ukoliko nije
		int adresa_prethodnog = (int)(intptr_t)poslednji.prethodni;
		ELEMENT prethodni;
		fseek(datoteka, adresa_prethodnog, SEEK_SET);
		fread(&prethodni, broj_bajtova_za_element, 1, datoteka);
		prethodni.sledeci = (void*)(intptr_t)glava;

		if (adresa_prethodnog == glava) {
			//lista ima samo dva elementa, nakon izbacivanja ostace samo glava
			prethodni.prethodni = (void*)(intptr_t)glava;

			if (glava == adresa_fizicki_poslednjeg) {
				//u ovom slucaju moramo da azuriramo susede tj. fizicki premestimo glavu na mesto izbacenog
				//azuriraj_susede(datoteka, glava, adresa_prethodnog, broj_bajtova_za_element);

				prethodni.prethodni = (void*)(intptr_t)adresa_poslednjeg;
				prethodni.sledeci = (void*)(intptr_t)adresa_poslednjeg;

				//ovime zapravo upisujemo novu glavu
				fseek(datoteka, adresa_poslednjeg, SEEK_SET);
				fwrite(&prethodni, broj_bajtova_za_element, 1, datoteka);

				//azuriramo metapodatke
				glava = adresa_poslednjeg; // ovo mu je nova adresa
				fseek(datoteka, sizeof(int), SEEK_SET);
				fwrite(&glava, sizeof(int), 1, datoteka);

				goto kraj_true;
			}

		}
		else {
			prvi.prethodni = (void*)(intptr_t)adresa_prethodnog;
			fseek(datoteka, glava, SEEK_SET);
			fwrite(&prvi, broj_bajtova_za_element, 1, datoteka);

		}
		////////////////////////////
		fseek(datoteka, adresa_prethodnog, SEEK_SET);
		fwrite(&prethodni, broj_bajtova_za_element, 1, datoteka);

		if (adresa_poslednjeg != adresa_fizicki_poslednjeg) {
			fseek(datoteka, adresa_fizicki_poslednjeg, SEEK_SET);
			fread(&fizicki_poslednji, broj_bajtova_za_element, 1, datoteka);

			//upisujemo fizicki poslednji na mesto obrisanog i azuriramo njegovu adresu
			azuriraj_susede(datoteka, adresa_fizicki_poslednjeg, adresa_poslednjeg, broj_bajtova_za_element);
			// znaci susede fizicki poslednjeg smo preusmerili da pokazuju na njegovu novu adresu (adresu poslednjeg)
			// i sada upisemo taj element na novo mesto
			fseek(datoteka, adresa_poslednjeg, SEEK_SET);
			fwrite(&fizicki_poslednji, broj_bajtova_za_element, 1, datoteka);

			if (adresa_fizicki_poslednjeg == glava) {
				//u ovom slucaju moramo da promenimo i metapodatke
				glava = adresa_poslednjeg; // ovo mu je nova adresa

				fseek(datoteka, sizeof(int), SEEK_SET);
				fwrite(&glava, sizeof(int), 1, datoteka);
			}
		}
		goto kraj_true;
	}

kraj_true:
	skrati_datoteku(datoteka, broj_bajtova_za_element);
	fclose(datoteka);
	(*lista)->broj_elemenata--;
	signal.status = Info;
	signal.poruka = poruka.INFO.izbaci;
	return signal;
kraj_false:
	fclose(datoteka);
	signal.status = Upozorenje;
	signal.poruka = poruka.UPOZORENJE.izbaci;
	return signal;
}

SIGNAL izbaci_po_vrednosti(LISTA* lista, PODATAK* podatak) {
	SIGNAL signal;

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

	int adresa_trenutnog = glava;
	ELEMENT prvi;
	fseek(datoteka, glava, SEEK_SET);
	fread(&prvi, broj_bajtova_za_element, 1, datoteka);
	int adresa_poslednjeg = (int)(intptr_t)prvi.prethodni;

	do {
		ELEMENT trenutni;
		fseek(datoteka, adresa_trenutnog, SEEK_SET);
		fread(&trenutni, broj_bajtova_za_element, 1, datoteka);

		if (trenutni.podatak == *podatak) {
			//ovo je element koji treba da izbacimo, i to njegovo prvo pojavljivanje

			if ((*lista)->broj_elemenata == 1) {
				//ako je ovo jedini element u listi, izmenimo metapodatke i skratimo datoteku
				glava = -1;
				fseek(datoteka, sizeof(int), SEEK_SET);
				fwrite(&glava, sizeof(int), 1, datoteka);

				goto kraj_true;
			}
			if (adresa_trenutnog == glava) {
				//izbacuje se glava, pozvacemo funkciju za izbacivanje sa pocetka
				fclose(datoteka);
				signal = izbaci_sa_pocetka(lista, podatak);
				return signal;
			}
			if (adresa_trenutnog == adresa_poslednjeg) {
				//izbacuje se poslednji element, pozvacemo funkciju za izbacivanje sa kraja
				fclose(datoteka);
				signal = izbaci_sa_kraja(lista, podatak);
				return signal;
			}

			// ako se nalazimo ovde, znaci izbacuje se neki element koji nije ni prvi ni poslednji (sto takodje 
			// znaci da lista ima vise od 2 elementa)

			int adresa_prethodnog = (int)(intptr_t)trenutni.prethodni;
			ELEMENT prethodni;
			fseek(datoteka, adresa_prethodnog, SEEK_SET);
			fread(&prethodni, broj_bajtova_za_element, 1, datoteka);

			int adresa_sledeceg = (int)(intptr_t)trenutni.sledeci;
			ELEMENT sledeci;
			fseek(datoteka, adresa_sledeceg, SEEK_SET);
			fread(&sledeci, broj_bajtova_za_element, 1, datoteka);

			int adresa_fizicki_poslednjeg = broj_bajtova_u_datoteci - broj_bajtova_za_element;

			//1) prevezi pokazivace i upisi ih
			prethodni.sledeci = (void*)(intptr_t)adresa_sledeceg;
			sledeci.prethodni = (void*)(intptr_t)adresa_prethodnog;
			fseek(datoteka, adresa_prethodnog, SEEK_SET);
			fwrite(&prethodni, broj_bajtova_za_element, 1, datoteka);
			fseek(datoteka, adresa_sledeceg, SEEK_SET);
			fwrite(&sledeci, broj_bajtova_za_element, 1, datoteka);

			//2) fizicki poslednji prebaci na mesto trenutnog (izbacenog) i azuriraj mu susede
			//ovo mora ovde jer ako ga ucitamo ranije, on moze da se promeni u medjuvremenu a nama ostane ta stara vrednost 
			ELEMENT fizicki_poslednji;
			fseek(datoteka, adresa_fizicki_poslednjeg, SEEK_SET);
			fread(&fizicki_poslednji, broj_bajtova_za_element, 1, datoteka);

			fseek(datoteka, adresa_trenutnog, SEEK_SET);
			fwrite(&fizicki_poslednji, broj_bajtova_za_element, 1, datoteka);
			azuriraj_susede(datoteka, adresa_fizicki_poslednjeg, adresa_trenutnog, broj_bajtova_za_element);

			if (adresa_fizicki_poslednjeg == glava) {
				//ako je glava na fizicki poslednjem, moraju da se promene i metapodaci na njegovu novu adresu (adresu trenutnog)

				glava = adresa_trenutnog;
				fseek(datoteka, sizeof(int), SEEK_SET);
				fwrite(&glava, sizeof(int), 1, datoteka);
			}

			goto kraj_true;
		}
		adresa_trenutnog = (int)(intptr_t)trenutni.sledeci;
	} while (adresa_trenutnog != glava);

kraj_false:
	//ovde smo izmedju ostalog i ukoliko smo prosli kroz celu listu a nismo nasli taj podatak
	fclose(datoteka);
	signal.status = Upozorenje;
	signal.poruka = poruka.UPOZORENJE.izbaci;
	return signal;
kraj_true:
	skrati_datoteku(datoteka, broj_bajtova_za_element);
	fclose(datoteka);
	(*lista)->broj_elemenata--;
	signal.status = Info;
	signal.poruka = poruka.INFO.izbaci;
	return signal;
}


void azuriraj_susede(FILE* datoteka, int adresa, int nova_adresa, int velicina) { //kada pomeramo element na fizicki novu adresu
	ELEMENT element;
	fseek(datoteka, adresa, SEEK_SET);
	fread(&element, velicina, 1, datoteka);

	int prethodni = (int)(intptr_t)element.prethodni;
	int sledeci = (int)(intptr_t)element.sledeci;

	ELEMENT sused;

	// azuriraj prethodni
	if (prethodni != NULL) {
		fseek(datoteka, prethodni, SEEK_SET);
		fread(&sused, velicina, 1, datoteka);
		if ((int)(intptr_t)sused.sledeci == adresa) {
			sused.sledeci = (void*)(intptr_t)nova_adresa;
			fseek(datoteka, prethodni, SEEK_SET);
			fwrite(&sused, velicina, 1, datoteka);
		}
	}

	// azuriraj sledeci
	if (sledeci != NULL) {
		fseek(datoteka, sledeci, SEEK_SET);
		fread(&sused, velicina, 1, datoteka);
		if ((int)(intptr_t)sused.prethodni == adresa) {
			sused.prethodni = (void*)(intptr_t)nova_adresa;
			fseek(datoteka, sledeci, SEEK_SET);
			fwrite(&sused, velicina, 1, datoteka);
		}
	}
}

void skrati_datoteku(FILE* datoteka, int broj_bajtova_za_skracivanje) {
	fflush(datoteka);
	int file_descriptor = fileno(datoteka); // file descriptor
	fseek(datoteka, 0, SEEK_END);
	long nova_velicina = ftell(datoteka) - broj_bajtova_za_skracivanje;
	_chsize(file_descriptor, nova_velicina); // skracivanje datoteke
}

SIGNAL bubble_sort(FILE* datoteka, int adresa_glave, int broj_bajtova_za_element, SMER_SORTIRANJA smer) {
	SIGNAL signal;

	ELEMENT prvi, drugi;
	int adresa_prvog, adresa_drugog;
	adresa_prvog = adresa_glave;
	fseek(datoteka, adresa_prvog, SEEK_SET);
	fread(&prvi, broj_bajtova_za_element, 1, datoteka);

	do {
		adresa_drugog = (int)(intptr_t)prvi.sledeci;
		do {
			fseek(datoteka, adresa_drugog, SEEK_SET);
			fread(&drugi, broj_bajtova_za_element, 1, datoteka);
			if ((smer == Rastuce && prvi.podatak > drugi.podatak) ||
				(smer == Opadajuce && prvi.podatak < drugi.podatak)) {
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

	signal.status = Info;
	signal.poruka = poruka.INFO.sortiraj;
	return signal;
}

SIGNAL insertion_sort(FILE* datoteka, int adresa_glave, int velicina_elementa, SMER_SORTIRANJA smer) {
	SIGNAL signal;
	ELEMENT trenutni, pomerani, prethodni;

	// ucitaj glavu
	fseek(datoteka, adresa_glave, SEEK_SET);
	fread(&trenutni, velicina_elementa, 1, datoteka);
	int adr_poslednji = (int)(intptr_t)trenutni.prethodni;

	int adr_trenutni = trenutni.sledeci;

	// idemo od drugog elementa do kraja 
	while (adr_trenutni != adresa_glave) {

		// ucitaj trenutni čvor
		fseek(datoteka, adr_trenutni, SEEK_SET);
		fread(&trenutni, velicina_elementa, 1, datoteka);

		// pamtimo podatak i adresu trenutnog u slucaju da kasnije bude pomeranja
		int podatak = trenutni.podatak;
		int adr_pomeranja = adr_trenutni;

		int adr_prethodni = trenutni.prethodni;
		// pomeraj podatak unazad dok god je potrebno, dok prethodni ne bude jednak poslednjem (da bismo obuhvatili i glavu)
		while (adr_prethodni != adr_poslednji) {
			fseek(datoteka, adr_prethodni, SEEK_SET);
			fread(&prethodni, velicina_elementa, 1, datoteka);

			bool treba_pomeriti = (smer == Rastuce && prethodni.podatak > podatak) ||
				(smer == Opadajuce && prethodni.podatak < podatak);

			if (!treba_pomeriti)
				break;

			// pomeri podatak (prethodni na mesto trenutnog tj. adr_pomeranja)
			fseek(datoteka, adr_pomeranja, SEEK_SET);
			fread(&pomerani, velicina_elementa, 1, datoteka);
			//printf("Pomeram %d na mesto %d\n", prethodni.podatak, pomerani.podatak);
			pomerani.podatak = prethodni.podatak;
			fseek(datoteka, adr_pomeranja, SEEK_SET);
			fwrite(&pomerani, velicina_elementa, 1, datoteka);

			// pomeramo se jedno mesto unazad i u narednom ciklusu petlje proveravamo da li podatak treba 
			// pomeriti dalje unazad
			adr_pomeranja = adr_prethodni;
			adr_prethodni = prethodni.prethodni;
		}

		// upisemo sacuvani podatak na mesto gde smo stali u petlji
		fseek(datoteka, adr_pomeranja, SEEK_SET);
		fread(&pomerani, velicina_elementa, 1, datoteka);
		//printf("Pomeram %d na mesto %d\n", podatak, pomerani.podatak);
		pomerani.podatak = podatak;
		fseek(datoteka, adr_pomeranja, SEEK_SET);
		fwrite(&pomerani, velicina_elementa, 1, datoteka);

		// prelazimo na sledeci element
		fseek(datoteka, adr_trenutni, SEEK_SET);
		fread(&trenutni, velicina_elementa, 1, datoteka);
		adr_trenutni = trenutni.sledeci;
	}

	signal.status = Info;
	signal.poruka = poruka.INFO.sortiraj;
	return signal;
}

SIGNAL selection_sort(FILE* datoteka, int adresa_glave, int broj_bajtova_za_element, SMER_SORTIRANJA smer) {
	SIGNAL signal;
	ELEMENT i, j, min_max;
	int adr_i = adresa_glave;

	// idemo kroz listu, jedan po jedan
	do {
		fseek(datoteka, adr_i, SEEK_SET);
		fread(&i, broj_bajtova_za_element, 1, datoteka);

		int adr_min_max = adr_i;
		min_max = i;

		int adr_j = i.sledeci;

		while (adr_j != adresa_glave) {
			fseek(datoteka, adr_j, SEEK_SET);
			fread(&j, broj_bajtova_za_element, 1, datoteka);

			bool bolji = (smer == Rastuce && j.podatak < min_max.podatak) ||
				(smer == Opadajuce && j.podatak > min_max.podatak);

			if (bolji) {
				adr_min_max = adr_j;
				min_max = j;
			}
			adr_j = j.sledeci;
		}

		// ako treba, zameni podatke
		if (adr_min_max != adr_i) {
			int tmp = i.podatak;
			i.podatak = min_max.podatak;
			min_max.podatak = tmp;

			fseek(datoteka, adr_i, SEEK_SET);
			fwrite(&i, broj_bajtova_za_element, 1, datoteka);

			fseek(datoteka, adr_min_max, SEEK_SET);
			fwrite(&min_max, broj_bajtova_za_element, 1, datoteka);
		}

		adr_i = i.sledeci;

	} while (adr_i != adresa_glave);

	signal.status = Info;
	signal.poruka = poruka.INFO.sortiraj;
	return signal;
}

//main
int main() {

	LISTA lista = NULL;
	SIGNAL signal;
	signal = kreiraj(&lista);
	obrada_statusa(signal.status, signal.poruka, lista->skladiste, __LINE__);

	signal = unisti(&lista);
	obrada_statusa(signal.status, signal.poruka, lista->skladiste, __LINE__);

	signal = kreiraj(&lista);
	obrada_statusa(signal.status, signal.poruka, lista->skladiste, __LINE__);

	int a = 50;
	signal = ubaci(&lista, a, Kraj);
	obrada_statusa(signal.status, signal.poruka, lista->skladiste, __LINE__);

	int b = 30;
	signal = ubaci(&lista, b, Kraj);
	obrada_statusa(signal.status, signal.poruka, lista->skladiste, __LINE__);

	int c = 70;
	signal = ubaci(&lista, c, Vrednost);
	obrada_statusa(signal.status, signal.poruka, lista->skladiste, __LINE__);

	int d = 80;
	signal = ubaci(&lista, d, Pocetak);
	obrada_statusa(signal.status, signal.poruka, lista->skladiste, __LINE__);

	prikazi(lista);

	int izbaceni = 50;
	signal = izbaci(&lista, &izbaceni, Vrednost);
	obrada_statusa(signal.status, signal.poruka, lista->skladiste, __LINE__);
	printf("\nIzbaceni: %d\n", izbaceni);

	prikazi(lista);

	//lista->skladiste = NULL;
	signal = prazna(lista);
	obrada_statusa(signal.status, signal.poruka, lista->skladiste, __LINE__);

	signal = sadrzi(&lista, 7, Binarno);
	obrada_statusa(signal.status, signal.poruka, lista->skladiste, __LINE__);

	signal = sortiraj(&lista, Opadajuce, Selection);
	obrada_statusa(signal.status, signal.poruka, lista->skladiste, __LINE__);

	prikazi(lista);

	return 0;
}