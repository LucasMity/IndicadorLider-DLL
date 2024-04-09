#ifndef SERIAL_H
#define SERIAL_H

#include "../indicador/indicador.h"

#define SERIAL_TIMEOUT 100

static HANDLE serialPort   = INVALID_HANDLE_VALUE;
//---- OPERACOES SOBREPOSTAS/ASSINCRONAS ----
static OVERLAPPED osWrite = {0};
static OVERLAPPED osReader = {0};
//---- OPERACOES SOBREPOSTAS/ASSINCRONAS ----

// Abertura da comunicacao e inicio da Critical Section, garanta que haja somente uma thread operando
int serial_open         ( const char* portName, int baudRate, int dataBits, int stopBits, int parity );
int serial_setParametrosConfig ( int baudRate, int dataBits, int stopBits, int parity );
int serial_isOpen       (  );
// Fechamento da comunicacao e fim da Critical Section, garanta que haja somente uma thread operando
int serial_close        (  );
char serial_receive     (  );
int serial_send         ( const char* data );

#endif