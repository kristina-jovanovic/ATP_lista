#pragma once

#include "defs.h"
#include <stdarg.h>

// tipovi
typedef wchar_t* UTF8_STRING;

typedef enum { Info, Upozorenje, Greska } STATUS;

// funkcije
void prijavi(int kod, STRING naziv_datoteke, int linija, ...);
const UTF8_STRING nadji_poruku(int kod);

// makro koji automatizuje pozive funkcije prijavi()
#define PRIJAVI(kod, ...) prijavi((kod), __FILE__, __LINE__, __VA_ARGS__)

