
#include "meni.h"
#include "kalkulator/kalkulator.h"
//#include "lista.h"

int main() {
	_setmode(_fileno(stdout), _O_U8TEXT); // neophodno za ispis na cirilici 
	_setmode(_fileno(stderr), _O_U8TEXT); // neophodno za ispis na cirilici 
	setlocale(LC_ALL, "");
	//+ moraju da se koriste wide funckije - wprintf() i slicne 

	MENI meni;

	/*kreiraj_meni(&meni, "../../meni_datoteke/meni.txt");
	dodeli_funkciju_lista(&meni);
	pokreni_meni(meni);*/

	kreiraj_meni(&meni, "../../kalkulator/kalkulator.txt");
	dodeli_funkcije_kalkulator(&meni);
	pokreni_meni(meni);

#pragma region test_cirilica
	//ne radi sa ovime, radi ovaj kod iznad i samo treba dodati utf-8 u Additional Options

	//wchar_t* test = L"Тест ћирилице";
	//const char* result = setlocale(LC_ALL, "Serbian (Cyrillic)_Serbia.1251"); //ne
	//const char* result = setlocale(LC_ALL, "Serbian (Latin)_Serbia.1250"); //ne
	//const char* result = setlocale(LC_ALL, "sr-SP-Cyrl"); //ne
	//const char* result = setlocale(LC_ALL, ".1251");  // ne
	//const char* result = setlocale(LC_ALL, ".UTF8");  // ne
	//wprintf(L"setlocale: %S\n", result);  
	//wprintf(L"%ls\n", test);     
	//       
#pragma endregion

	return 0;
}