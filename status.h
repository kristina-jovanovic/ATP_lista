#pragma once

#include "defs.h"
#include <stdarg.h>

#define KOD int

// tipovi

typedef enum { Info, Upozorenje, Greska } STATUS;

// funkcije
void prijavi(KOD kod, STRING naziv_datoteke, int linija, ...);
const UTF8_STRING nadji_poruku(KOD kod);

// makro koji automatizuje pozive funkcije prijavi()
#define PRIJAVI(kod, ...) prijavi((kod), __FILE__, __LINE__, __VA_ARGS__)

