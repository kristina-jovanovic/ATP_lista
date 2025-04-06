#include "defs.h"
#include "lista.h"

bool kreiraj(LISTA* lista) {
	*lista = malloc(sizeof(struct lista));
	FILE* datoteka = fopen(naziv_datoteke, "ab");
	if (datoteka == NULL) {
		printf("Greska pri otvaranju datoteke!\n");
		return;
	}
	(*lista)->skladiste = datoteka; // ili staviti da je ovde naziv datoteke?
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
	return true;
}

bool unisti(LISTA* lista) {
	if (*lista == NULL || (*lista)->skladiste == NULL || (*lista)->skladiste == ErrorList) return false;
	remove(naziv_datoteke);
	(*lista)->broj_elemenata = 0;
	(*lista)->skladiste = NULL;
	return true;
}

bool ubaci_na_pocetak(LISTA* lista, PODATAK podatak) {
	if (*lista == NULL || (*lista)->broj_elemenata >= (*lista)->kapacitet) return false;

	FILE* datoteka = fopen(naziv_datoteke, "r+b"); //sa ab rezimom nije htelo
	if (datoteka == NULL) {
		datoteka = fopen(naziv_datoteke, "w+b");
		if (datoteka == NULL) {
			printf("Greska pri otvaranju datoteke!\n");
			return false;
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
	return true;
}

bool izbaci_sa_pocetka(LISTA* lista, PODATAK* podatak) {
	if (*lista == NULL || (*lista)->skladiste == NULL || (*lista)->skladiste == ErrorList
		|| (*lista)->broj_elemenata == 0) return false;

	FILE* datoteka = fopen(naziv_datoteke, "r+b");
	if (datoteka == NULL) {
		printf("Greska pri otvaranju datoteke!\n");
		return false;
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
	return true;
kraj_false:
	fclose(datoteka);
	return false;
}

void prikazi(LISTA lista) {
	printf("\n///// Lista: ");
	if ((lista == NULL) || (lista->skladiste == NULL) || (lista->skladiste == ErrorList)) {
		(lista == NULL || lista->skladiste == NULL) ? printf("< Null > \n") : printf("< ErrorList > \n");
		return;
	}
	FILE* datoteka = fopen(naziv_datoteke, "rb");
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
///////////
bool	sortiraj(LISTA*);

bool prazna(LISTA lista) {
	if (lista == NULL || lista->skladiste == NULL || lista->skladiste == ErrorList
		|| lista->broj_elemenata == 0) return true;
	return false;
}

bool sadrzi(LISTA lista, PODATAK podatak) {
	if (lista == NULL || lista->skladiste == NULL || lista->skladiste == ErrorList 
		|| lista->broj_elemenata == 0) return false;
	
	FILE* datoteka = fopen(naziv_datoteke, "rb");
	if (datoteka == NULL) {
		printf("Greska pri otvaranju datoteke!\n");
		return;
	}
	int adresa_glave, adresa_trenutnog;
	fseek(datoteka, sizeof(int), SEEK_SET); //pomeramo se za velicinu jednog int-a od pocetka
	fread(&adresa_glave, sizeof(int), 1, datoteka); //citamo adresu glave
	if (adresa_glave == -1) {
		printf("< Prazno >\n");
		return false;
	}
	ELEMENT trenutni;
	adresa_trenutnog = adresa_glave;
	do {
		fseek(datoteka, adresa_trenutnog, SEEK_SET);
		fread(&trenutni, sizeof(ELEMENT), 1, datoteka);
		if (trenutni.podatak == podatak) {
			fclose(datoteka);
			return true;
		}
		adresa_trenutnog = (int)(intptr_t)trenutni.sledeci;
	} while (adresa_trenutnog != adresa_glave);

	fclose(datoteka);
	return false;
}

//main
int main() {

	LISTA lista = NULL;
	kreiraj(&lista) ? printf("Lista je uspesno kreirana\n") : printf("Lista nije uspesno kreirana\n");
	//unisti(&lista) ? printf("Lista je uspesno unistena\n") : printf("Lista nije uspesno unistena\n");
	int a = 7;
	ubaci_na_pocetak(&lista, a) ? printf("Podatak %d je uspesno ubacen\n", a) : printf("Podatak %d nije uspesno ubacen\n", a);
	int b = 2;
	ubaci_na_pocetak(&lista, b) ? printf("Podatak %d je uspesno ubacen\n", b) : printf("Podatak %d nije uspesno ubacen\n", b);

	int c = 5;
	ubaci_na_pocetak(&lista, c) ? printf("Podatak %d je uspesno ubacen\n", c) : printf("Podatak %d nije uspesno ubacen\n", c);

	prikazi(lista);

	//int izbaceni;
	//izbaci_sa_pocetka(&lista, &izbaceni) ? printf("Podatak %d je uspesno izbacen\n", izbaceni) : printf("Podatak nije uspesno izbacen\n");
	//izbaci_sa_pocetka(&lista, &izbaceni) ? printf("Podatak %d je uspesno izbacen\n", izbaceni) : printf("Podatak nije uspesno izbacen\n");
	prikazi(lista);

	prazna(lista) ? printf("Lista je prazna\n") : printf("Lista nije prazna\n");
	sadrzi(lista,4) ? printf("Podatak %d postoji u listi\n", 4) : printf("Podatak %d ne postoji u listi\n", 4);


	return 0;
}