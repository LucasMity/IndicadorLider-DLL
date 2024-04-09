#ifndef TCP_H
#define TCP_H

#include "../indicador/indicador.h"

#define TCP_TIMEOUT_MSEC 100

static SOCKET ConnectSocket = INVALID_SOCKET;

// Abertura da comunicacao e inicio de Critical Section, garanta que haja somente uma thread operando
int tcp_open        ( const char* ipHost, int port );
int tcp_isOpen      (  );
// Fechamento da comunicacao e fim de Critical Section, garanta que haja somente uma thread operando
int tcp_close       (  );
char tcp_receive    (  );
int tcp_send        ( const char* data );

#endif