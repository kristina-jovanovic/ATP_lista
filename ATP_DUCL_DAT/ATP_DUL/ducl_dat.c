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
void skrati_datoteku(FILE*, int);

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
		sortiraj(lista, Rastuce); ////////////////
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

		ELEMENT poslednji;
		int adresa_poslednjeg = (int)(intptr_t)glava_el.prethodni;
		fseek(datoteka, adresa_poslednjeg, SEEK_SET);
		fread(&poslednji, broj_bajtova_za_element, 1, datoteka);

		//azuriranje veza
		novi.sledeci = (void*)(intptr_t)glava;
		novi.prethodni = (void*)(intptr_t)adresa_poslednjeg;
		poslednji.sledeci = (void*)(intptr_t)adresa_novog;
		glava_el.prethodni = (void*)(intptr_t)adresa_novog;

		//upisivanje novog elementa i izmena azuriranih
		fseek(datoteka, adresa_novog, SEEK_SET);
		fwrite(&novi, broj_bajtova_za_element, 1, datoteka);
		fseek(datoteka, glava, SEEK_SET);
		fwrite(&glava_el, broj_bajtova_za_element, 1, datoteka);
		fseek(datoteka, adresa_poslednjeg, SEEK_SET);
		fwrite(&poslednji, broj_bajtova_za_element, 1, datoteka);

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

	if ((int)(intptr_t)prvi.prethodni == glava && (int)(intptr_t)prvi.sledeci == glava) { //ili (*lista)->broj_elemenata == 1
		// u listi se nalazi samo jedan element
		prvi.prethodni = (void*)(intptr_t)-1;
		prvi.sledeci = (void*)(intptr_t)-1;
		prvi.podatak = -1;
		glava = -1; //lista je prazna
		fseek(datoteka, sizeof(int), SEEK_SET);
		fwrite(&glava, sizeof(int), 1, datoteka);
		goto kraj_true;
	}
	if ((*lista)->broj_elemenata == 2) {
		ELEMENT drugi;
		int adresa_drugog = (int)(intptr_t)prvi.sledeci;
		fseek(datoteka, adresa_drugog, SEEK_SET);
		fread(&drugi, broj_bajtova_za_element, 1, datoteka);
		//treba da pokazuje sam na sebe kada glavu izbacimo, a on ce se pomeriti na mesto glave
		//problem kad je glava na kraju
		drugi.sledeci = (void*)(intptr_t)glava;
		drugi.prethodni = (void*)(intptr_t)glava;
		fseek(datoteka, glava, SEEK_SET);
		fwrite(&drugi, broj_bajtova_za_element, 1, datoteka);
		goto kraj_true;
	}
	// ako smo ovde, lista ima bar 3 elementa
	ELEMENT poslednji;
	int adresa_poslednjeg = (int)(intptr_t)prvi.prethodni;
	fseek(datoteka, adresa_poslednjeg, SEEK_SET);
	fread(&poslednji, broj_bajtova_za_element, 1, datoteka);

	ELEMENT nova_glava;
	int adresa_nove_glave = (int)(intptr_t)prvi.sledeci;
	fseek(datoteka, adresa_nove_glave, SEEK_SET);
	fread(&nova_glava, broj_bajtova_za_element, 1, datoteka);

	// ovo je samo logicko brisanje, ali element ce ostati u memoriji i dalje... mozda ubaciti
	// u metapodatke 'prvi_slobodan' gde ce se cuvati njegov offset tako da mozemo da ponovo
	// koristimo taj prostor
	// URADICEMO SLEDECE - poslednji element cemo prebaciti na mesto izbacenog i skratiti datoteku
	// za velicinu tog jednog elementa (poslednjeg)

	//prvi.prethodni = (void*)(intptr_t)-1;
	//prvi.sledeci = (void*)(intptr_t)-1;
	//prvi.podatak = -1;

	//poslednji.sledeci = (void*)(intptr_t)adresa_nove_glave;
	//prvi = poslednji;
	//nova_glava.prethodni = (void*)(intptr_t)glava; //ovde treba da bude adresa_poslednjeg, ali je on prebacen
	//na prvo mesto (na mesto elementa koji je izbacen) tako da je to zapravo glava (stara)

	nova_glava.prethodni = (void*)(intptr_t)adresa_poslednjeg;
	poslednji.sledeci = (void*)(intptr_t)adresa_nove_glave;

	int adresa_fizicki_poslednjeg = broj_bajtova_u_datoteci - broj_bajtova_za_element;
	fseek(datoteka, adresa_fizicki_poslednjeg, SEEK_SET);
	ELEMENT fizicki_poslednji;
	fread(&fizicki_poslednji, broj_bajtova_za_element, 1, datoteka);
	int adresa_za_premestanje = glava; //odatle se izbacuje element
	//potrebno je da proverimo sve elemente ciji prethodni ili sledeci pokazivaci pokazuju na ovaj
	// fizicki poslednji element, i da to preusmerimo na adresa_za_premestanje
	int fp_prethodni_adresa = (int)(intptr_t)fizicki_poslednji.prethodni;
	ELEMENT fp_prethodni;
	fseek(datoteka, fp_prethodni_adresa, SEEK_SET);
	fread(&fp_prethodni, broj_bajtova_za_element, 1, datoteka);
	int fp_sledeci_adresa = (int)(intptr_t)fizicki_poslednji.sledeci;
	ELEMENT fp_sledeci;
	fseek(datoteka, fp_sledeci_adresa, SEEK_SET);
	fread(&fp_sledeci, broj_bajtova_za_element, 1, datoteka);

	fp_prethodni.sledeci = (void*)(intptr_t)glava;
	fp_sledeci.prethodni = (void*)(intptr_t)glava;

	fseek(datoteka, fp_prethodni_adresa, SEEK_SET);
	fwrite(&fp_prethodni, broj_bajtova_za_element, 1, datoteka);
	fseek(datoteka, fp_sledeci_adresa, SEEK_SET);
	fwrite(&fp_sledeci, broj_bajtova_za_element, 1, datoteka);
	////
	fseek(datoteka, glava, SEEK_SET);
	fwrite(&fizicki_poslednji, broj_bajtova_za_element, 1, datoteka);
	if (nova_glava.prethodni == fizicki_poslednji.prethodni &&
		nova_glava.sledeci == fizicki_poslednji.sledeci &&
		nova_glava.podatak == fizicki_poslednji.podatak) {
		//znaci nova_glava==fizicki_poslednji i nju treba da premestimo na glavu,
		//tj. glava ce da ostane na istom mestu
		goto kraj_true;
	}
	else {
		glava = adresa_nove_glave;
		fseek(datoteka, sizeof(int), SEEK_SET);
		fwrite(&glava, sizeof(int), 1, datoteka);
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

SIGNAL izbaci_sa_kraja1(LISTA* lista, PODATAK* podatak) {
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

	/////////////
	// izbacujemo sa kraja (fizicki poslednji)
	ELEMENT prvi;
	fseek(datoteka, glava, SEEK_SET);
	fread(&prvi, broj_bajtova_za_element, 1, datoteka);

	if ((*lista)->broj_elemenata == 1) {
		//brisemo jedini element koji postoji
		glava = -1;
		fseek(datoteka, sizeof(int), SEEK_SET);
		fwrite(&glava, broj_bajtova_za_element, 1, datoteka);
		goto kraj_true;
	}
	else {
		ELEMENT poslednji;
		int adresa_poslednjeg = (int)(intptr_t)prvi.prethodni;
		fseek(datoteka, adresa_poslednjeg, SEEK_SET);
		fread(&poslednji, broj_bajtova_za_element, 1, datoteka);

		int adresa_fizicki_posledjeg = broj_bajtova_u_datoteci - broj_bajtova_za_element;
		ELEMENT fizicki_poslednji;
		fseek(datoteka, adresa_fizicki_posledjeg, SEEK_SET);
		fread(&fizicki_poslednji, broj_bajtova_za_element, 1, datoteka);

		if ((*lista)->broj_elemenata == 2) {
			if (glava == adresa_fizicki_posledjeg) {
				//glava je na poslednjem mestu
				glava = (int)(intptr_t)fizicki_poslednji.sledeci;
				//prvi ce da pokazuje sam na sebe jer ostaje jedini u listi
				prvi.sledeci = (void*)(intptr_t)glava;
				prvi.prethodni = (void*)(intptr_t)glava;

				fseek(datoteka, sizeof(int), SEEK_SET);
				fwrite(&glava, broj_bajtova_za_element, 1, datoteka);
				fseek(datoteka, glava, SEEK_SET);
				fwrite(&prvi, broj_bajtova_za_element, 1, datoteka);

				goto kraj_true;
			}
			if (adresa_poslednjeg == adresa_fizicki_posledjeg) {
				ELEMENT pre_poslednjeg;
				fseek(datoteka, (int)(intptr_t)poslednji.prethodni, SEEK_SET);
				fread(&pre_poslednjeg, broj_bajtova_za_element, 1, datoteka);

				pre_poslednjeg.sledeci = poslednji.sledeci; //tj prvi
				prvi.prethodni = poslednji.prethodni;

				//azuriranje
				fseek(datoteka, (int)(intptr_t)poslednji.prethodni, SEEK_SET);
				fwrite(&pre_poslednjeg, broj_bajtova_za_element, 1, datoteka);
				fseek(datoteka, glava, SEEK_SET);
				fwrite(&prvi, broj_bajtova_za_element, 1, datoteka);

				goto kraj_true;
			}
		}
		else {
		}

	}

	////////////

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

SIGNAL sortiraj(LISTA* lista, SMER_SORTIRANJA smer) {
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
	ELEMENT prvi, drugi;
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

void skrati_datoteku(FILE* datoteka, int broj_bajtova_za_skracivanje) {
	fflush(datoteka);
	int file_descriptor = fileno(datoteka); // file descriptor
	fseek(datoteka, 0, SEEK_END);
	long nova_velicina = ftell(datoteka) - broj_bajtova_za_skracivanje;
	_chsize(file_descriptor, nova_velicina); // skracivanje datoteke
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

	int a = 7;
	signal = ubaci(&lista, a, Kraj);
	obrada_statusa(signal.status, signal.poruka, lista->skladiste, __LINE__);

	int b = 2;
	signal = ubaci(&lista, b, Kraj);
	obrada_statusa(signal.status, signal.poruka, lista->skladiste, __LINE__);

	/*int c = 5;
	signal = ubaci(&lista, c, Vrednost);
	obrada_statusa(signal.status, signal.poruka, lista->skladiste, __LINE__);*/

	prikazi(lista);

	int izbaceni;
	//signal = izbaci_sa_kraja1(&lista, &izbaceni);
	//obrada_statusa(signal.status, signal.poruka, lista->skladiste, __LINE__);

	/*int izbaceni;
	signal = izbaci_sa_pocetka(&lista, &izbaceni);
	obrada_statusa(signal.status, signal.poruka, lista->skladiste, __LINE__);*/

	//izbaci_sa_pocetka(&lista, &izbaceni) ? printf("Podatak %d je uspesno izbacen\n", izbaceni) : printf("Podatak nije uspesno izbacen\n");
	//prikazi(lista);

	//lista->skladiste = NULL;
	signal = prazna(lista);
	obrada_statusa(signal.status, signal.poruka, lista->skladiste, __LINE__);

	signal = sadrzi(&lista, 7, Binarno);
	obrada_statusa(signal.status, signal.poruka, lista->skladiste, __LINE__);

	signal = sortiraj(&lista, Opadajuce);
	obrada_statusa(signal.status, signal.poruka, lista->skladiste, __LINE__);

	prikazi(lista);

	return 0;
}