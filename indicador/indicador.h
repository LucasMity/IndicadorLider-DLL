// Autor: Lucas Mity Kamado
// Empresa: Lider Balancas
// Departamento: Engenharia

#ifndef INDICADOR_H
#define INDICADOR_H

#ifdef INDICADOR_EXPORTS
#define INDICADOR_API __declspec(dllexport)
#else
#define INDICADOR_API __declspec(dllimport)
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#include <process.h>
#include <fileapi.h>
#include "../serial/serial.h"
#include "../tcp/tcp.h"

// STATUS BALANCA
enum ind_status_balanca {
    STATUS_BALANCA_PESO_EXCESSO_POSITIVO    =   -3,
    STATUS_BALANCA_PESO_EXCESSO_NEGATIVO    =   -2,
    STATUS_BALANCA_DESCONHECIDO             =   -1,
    STATUS_BALANCA_ESTAVEL                  =    0,
    STATUS_BALANCA_INSTAVEL                 =    1,
    STATUS_BALANCA_VALOR_PICO               =    2,
    STATUS_BALANCA_PESO_MEDIO               =    3,
    STATUS_BALANCA_AGUARDANDO               =    4,
    STATUS_BALANCA_PERDA_COMUNICACAO        =    5,
    STATUS_BALANCA_IMPRIME                  =    6,

    CHAR_STATUS_BALANCA_ESTAVEL             =    'E',
    CHAR_STATUS_BALANCA_INSTAVEL            =    'I',
    CHAR_STATUS_BALANCA_VALOR_PICO          =    'P',
    CHAR_STATUS_BALANCA_PESO_MEDIO          =    'M',
    CHAR_STATUS_BALANCA_AGUARDANDO          =    'A',
    CHAR_STATUS_BALANCA_PERDA_COMUNICACAO   =    'R',
    CHAR_STATUS_BALANCA_IMPRIME             =    'G'
};

// INDICACAO DE PESO
enum ind_unidade_medida_id_peso {
    UNIDADE_MEDIDA_DESCONHECIDO = -1,
    UNIDADE_MEDIDA_KN           =  0,
    UNIDADE_MEDIDA_LB           =  1,
    UNIDADE_MEDIDA_KG           =  2,
    UNIDADE_MEDIDA_N            =  3,
    UNIDADE_MEDIDA_OZ           =  4
};

// STATUS BATERIA
enum ind_status_bateria {
    STATUS_BATERIA_DESCONHECIDO     =    -1,
    STATUS_BATERIA_CARREGANDO       =     0,
    STATUS_BATERIA_EM_BATERIA       =     1,
    STATUS_BATERIA_CARGA_COMPLETA   =     2,

    CHAR_STATUS_BATERIA_CARREGANDO       =     'C',
    CHAR_STATUS_BATERIA_EM_BATERIA       =     'B',
    CHAR_STATUS_BATERIA_CARGA_COMPLETA   =     'F'
};

// NIVEL BATERIA
enum ind_nivel_bateria {
    NIVEL_BATERIA_DESCONHECIDO  =  -1,
    NIVEL_BATERIA_BAIXA         =   0,
    NIVEL_BATERIA_REGULAR       =   1,
    NIVEL_BATERIA_BOA           =   2,
    NIVEL_BATERIA_COMPLETA      =   3
};

// STATUS PROTOCOLO
enum ind_status_protocolo {
    PROTOCOLO_DESCONHECIDO  = -1,
    PROTOCOLO_LIDER_1       =  0,
    PROTOCOLO_LIDER_2       =  1,
    PROTOCOLO_LIDER_3       =  2,
    PROTOCOLO_LIDER_4       =  3,
    PROTOCOLO_LIDER_10      = 10
};

// RETURN
enum ind_response_code {
    RESPONDING                  = 3,
    WAITING_RESPONSE            = 2,
    OK                          = 1,
    ERRO                        = 0,
    ERRO_TIMEOUT                = -1,
    ERRO_INVALID_ARG            = -2,
    ERRO_MEMORY                 = -3,
    ERRO_COMUN_OPEN             = -4,
    ERRO_COMUN_CLOSED           = -5,
    DEFAULT_RESPONSE            = ERRO_TIMEOUT,
};

enum ind_tara {
    TARA_ACK        = 100,
    TARA_NOACK      = -100,
    TARA_ACK_0      = 0 + TARA_ACK,
    TARA_ACK_1      = 1 + TARA_ACK,
    TARA_ACK_2      = 2 + TARA_ACK,
    TARA_ACK_3      = 3 + TARA_ACK,
    TARA_ACK_4      = 4 + TARA_ACK,
    TARA_NOACK_0    = 0 + TARA_NOACK,
    TARA_NOACK_1    = -1 + TARA_NOACK,
    TARA_NOACK_2    = -2 + TARA_NOACK,
    TARA_NOACK_3    = -3 + TARA_NOACK,
    TARA_NOACK_4    = -4 + TARA_NOACK,
    TARA_NOACK_5    = -5 + TARA_NOACK,
    TARA_NOACK_6    = -6 + TARA_NOACK,
    TARA_NOACK_7    = -7 + TARA_NOACK,
};

enum ind_zero {
    ZERO_ACK        = 200,
    ZERO_NOACK      = -200,
    ZERO_ACK_0      = 0 + ZERO_ACK,
    ZERO_NOACK_0    = 0 + ZERO_NOACK,
};

enum ind_imprime {
    IMPRIME_ACK     = 300,
    IMPRIME_NOACK   = -300,
    IMPRIME_ACK_0   = 0 + IMPRIME_ACK,
    IMPRIME_ACK_1   = 1 + IMPRIME_ACK,
    IMPRIME_NOACK_0 = 0 + IMPRIME_NOACK,
};

#define CHAR_VAZIO (char)0
#define MAX_BUFFER_SIZE 100
#define MAX_QTD_VALUES 12
#define TRUE 1
#define FALSE 0

#define TIMEOUT 300
#define LONG_TIMEOUT 4*TIMEOUT
#define WAIT_RESPONSE 5
#define WAIT_READY 100

#define ID_MIN_USUARIO 1
#define ID_MAX_USUARIO 50
#define CAPACIDADE_MAXIMA_TABELA_PRODUTO 128
#define CODIGO_PRODUTO_LEN 14       
#define DESCRICAO_PRODUTO_LEN 21    
#define DATA_LEN 10                 
#define HORA_LEN 8                  

//--------------------------PRIVATE--------------------------

typedef struct dadosIndicador {
    double   pesoLiquido;
    double   pesoBruto;
    double   tara;
    int      precisaoDecimalPeso;
    int      statusBalanca;
    int      unidadeMedida;
    int      isModoContadoraPecas; // Booleano TRUE or FALSE
    int      protocolo;
    int      statusBateriaIndicador;
    int      nivelBateriaIndicador;
    int      nivelBateriaTransmissorTx1;
    int      nivelBateriaTransmissorTx2;
    char     codigoProduto[CODIGO_PRODUTO_LEN+1];
    char     data[DATA_LEN+1];
    char     hora[HORA_LEN+1];
} dadosIndicador;

static dadosIndicador indicador;

// VARIAVEIS DE CONTROLE
static int relatorioResponse    = DEFAULT_RESPONSE;
static int configResponse       = DEFAULT_RESPONSE;
static int produtoResponse      = DEFAULT_RESPONSE;
static int taraResponse         = DEFAULT_RESPONSE;
static int zeroResponse         = DEFAULT_RESPONSE;
static int imprResponse         = DEFAULT_RESPONSE;
static int isClosing = FALSE; // Indica que a comunicacao com o indicador esta prestes a fechar

static char* relatorioString = NULL;
static int relatorioStringLen = 0;

static char* configString = NULL;
static int configStringLen = 0;

static char* produtoString = NULL;
static int produtoStringLen = 0;

static HANDLE runThread;
static unsigned runThreadID;

//Protecao de variaveis compartilhadas entre threads
static CRITICAL_SECTION dadosIndicadorCriticalSection;

static CRITICAL_SECTION zeroResponseCriticalSection;
static CRITICAL_SECTION taraResponseCriticalSection;
static CRITICAL_SECTION imprResponseCriticalSection;
static CRITICAL_SECTION relatorioResponseCriticalSection;
static CRITICAL_SECTION configResponseCriticalSection;
static CRITICAL_SECTION produtoResponseCriticalSection;
static CRITICAL_SECTION isClosingCriticalSection;

static int indicadorCriticalSectionInicialized = FALSE;
//-----------------------------------------------------------

// Abre a comunicacao Serial
// char* port = Porta serial
// int baudRate = Baud Rate da comunicacao
// int dataBits = 7 ou 8
// int stopBits = 1 ou 2
// int parity = 0(None), 1(Odd), 2(Even)
// Retorno >  0 = SUCESSO
// Retorno <= 0 = ERRO
INDICADOR_API int openSerial    ( const char* port, int baudRate, int dataBits, int stopBits, int parity  );
INDICADOR_API int setParametrosConfigSerial     ( int baudRate, int dataBits, int stopBits, int parity );

// Abre a comunicacao TCP
// char* ip = Endereco IP
// int baudRate = Port da comunicacao
// Retorno >  0 = SUCESSO
// Retorno <= 0 = ERRO
INDICADOR_API int openTcp       ( const char* ip  , int port      );

// Verifica se ha comunicacao aberta
// Retorno =  1 - Comunicacao Serial Aberta
// Retorno =  2 - Comunicacao TCP Aberta
// Retorno =  0 - Nenhuma comunicacao Aberta
INDICADOR_API int isOpen        (  );

// Fecha a comunicacao aberta
// Retorno >  0 = SUCESSO
// Retorno <= 0 = ERRO
INDICADOR_API int close         (  );

// Envia o comando de Zerar balanca
// Retorno >  0 = SUCESSO
// Retorno <= 0 = ERRO
INDICADOR_API int zerar         ( );

// Envia o comando de Tarar balanca
// Retorno >  0 = SUCESSO
// Retorno <= 0 = ERRO
INDICADOR_API int tarar         ( );

// Envia o comando de Tarar balanca
// tara = valor inteiro da tara
// Retorno >  0 = SUCESSO
// Retorno <= 0 = ERRO
INDICADOR_API int tararManual   ( int tara );

// Envia o comando de Imprimir ou Calcular media
// Retorno >  0 = SUCESSO
// Retorno <= 0 = ERRO
INDICADOR_API int imprimir_calcularMedia ( );

INDICADOR_API int perguntarPeso(  );

// ------------------- PRODUTO --------------------

// Função reinicia a busca por um espaço livre na memória
INDICADOR_API int ERF_tabelaProduto( );
INDICADOR_API int cadastrarProduto ( const char* codigo, const char* descricao );
INDICADOR_API int alterarProduto ( const char* codigo, const char* descricao );
INDICADOR_API int deletarProduto ( const char* codigo );

// Função para iniciar tabela nova (sobrescreve tabela do indicador)
INDICADOR_API int EXC_iniciarTabelaProduto();
INDICADOR_API int EXC_cadastrarProduto( const char* codigo, const char* descricao );
INDICADOR_API int EXC_finalizarTabelaProduto( int quantidadeProdutosTabelaEXC );

INDICADOR_API const char* getTabelaProduto ( );
INDICADOR_API int deletarTabelaProduto (  );
// ------------------- FIM PRODUTO --------------------

// ------------------- USUARIO ------------------------

INDICADOR_API const char* getUsuarioById ( int id );
INDICADOR_API const char* setUsuario ( const char* senhaMenu, int idUsuario, const char* senhaUsuario, const char* nomeUsuario );

// ------------------- CONFIGURACAO --------------------

INDICADOR_API const char* setConfiguracaoIndicador ( int funcao, const char* input );
INDICADOR_API const char* getConfiguracaoIndicador ( int funcao );
INDICADOR_API int saveConfiguracaoIndicador ( );
// ------------------- FIM CONFIGURACAO --------------------

// tipo = 0 - Relatorio por numero de impressoes
// input = "[int]"
// ---
// tipo = 1 - Relatorio por data
// input = "dd/mm/aaaa,dd/mm/aaaa"
// ---
// tipo = 2 - Relatorio por codigo
// input = "[codigo]"
// ---
// tipo = 3 - Relatorio por usuario
// input = "[usuarioId]"
// ---
// tipo = 4 - Relatorio por data e codigo
// input = "dd/mm/aaaa,dd/mm/aaaa,[codigo]"
// ---
// tipo = 5 - Relatorio por data e usuario
// input = "dd/mm/aaaa,dd/mm/aaaa,[usuarioId]"
// ---
INDICADOR_API const char* gerarRelatorio( int tipo, const char* input );

INDICADOR_API double getPesoLiquido                 (  );
INDICADOR_API double getPesoBruto                   (  );
INDICADOR_API double getTara                        (  );
INDICADOR_API int    getPrecisaoDecimalPeso         (  );
INDICADOR_API int    getStatusBalanca               (  );
INDICADOR_API int    getUnidadeMedida               (  );
INDICADOR_API int    isModoContadoraDePecas         (  );
INDICADOR_API int    getProtocolo                   (  );
INDICADOR_API int    getStatusBateriaIndicador      (  );
INDICADOR_API int    getNivelBateriaIndicador       (  );
INDICADOR_API int    getNivelBateriaTransmissorTx1  (  );
INDICADOR_API int    getNivelBateriaTransmissorTx2  (  );
//---- FUNCOES PROTOCOLO LIDER 10 -----------------------
INDICADOR_API const char*  getCodigoProduto         (  );
INDICADOR_API const char*  getData                  (  );
INDICADOR_API const char*  getHora                  (  );
//---- FIM FUNCOES PROTOCOLO LIDER 10 -------------------
INDICADOR_API dadosIndicador getTodosDadosIndicador (  );

INDICADOR_API void freeStringMemory                 (  );
//--------------------------PRIVATE--------------------------

// Envia data pela comunicacao aberta
// Retorno >  0 = SUCESSO
// Retorno <= 0 = ERRO
static int indicadorSend    ( const char* data );

// Recebe dados pela comunicacao aberta
// os dados recebidos sao armazenados em char* buffer
static char indicadorReceive (  );

// Limpa todas as variaveis de indicador
// Retorno >  0 = SUCESSO
// Retorno <= 0 = ERRO
static void clear    ( dadosIndicador *ind );

static void freeStringResponse ( char **dataString, int *dataStringLen );

// Verifica o Checksum dos dados armazenados 
// em char* buffer
// Retorno >  0 = SUCESSO
// Retorno <= 0 = ERRO
static int checksum ( char* buffer, char separator );

static int checksum2Compl ( char* buffer, char separator );

static int startIndicador();
static unsigned __stdcall runIndicador( void* pArguments );
static void deleteCharFromString( char* string, char chdelete );
static int  contaCasasDecimaisNumeroString( const char* numero );
static int  verificaData(const char* data);
static int  verificaHora(const char* hora);
static int  verificaCodigoProduto(const char* codigoProduto);
static void filterStringValues( char* buffer );

static void inicializarCriticalSection();
static void deleteCriticalSection();

static void setDadosIndicador  ( dadosIndicador value  );

static void setZeroResponse ( int value );
static int  getZeroResponse (  );
static void setTaraResponse ( int value );
static int  getTaraResponse (  );
static void setImprResponse ( int value );
static int  getImprResponse (  );
static void setRelatorioResponse ( int value, int prioridade );
static int  getRelatorioResponse (  );
static void setConfigResponse ( int value );
static int  getConfigResponse (  );
static void setProdutoResponse ( int value, int prioridade );
static int  getProdutoResponse (  );

static void setIsClosing ( int value );
static int  getIsClosing (  );
//-----------------------------------------------------------

#endif // INDICADOR_H