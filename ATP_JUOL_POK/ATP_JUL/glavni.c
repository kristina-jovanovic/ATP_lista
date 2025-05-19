
#include "meni.h"
//#include "kalkulator/kalkulator.h"
#include "lista.h"

int main() {
	init();
    
	MENI meni;

	kreiraj_meni(&meni, "../../meni_datoteke/meni.txt");
	dodeli_funkciju_lista(&meni);
	pokreni_meni(meni);

	/*kreiraj_meni(&meni, "../../kalkulator/kalkulator.txt");
	dodeli_funkcije_kalkulator(&meni);
	pokreni_meni(meni);*/

	return 0;
}