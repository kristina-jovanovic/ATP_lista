
#include "meni.h"
//#include "lista.h"


int main() {
	_setmode(_fileno(stdout), _O_U8TEXT); // neophodno za ispis na cirilici 
	_setmode(_fileno(stderr), _O_U8TEXT); // neophodno za ispis na cirilici 
	setlocale(LC_ALL, "");
	//+ moraju da se koriste wide funckije - wprintf() i slicne 

	MENI meni;
	kreiraj_meni(&meni, "../../meni_datoteke/meni.txt");
	pokreni_meni(meni);
	/*obradi_stavku(meni, 1);
	obradi_stavku(meni, 3);*/

	return 0;
}