#ifndef INDICADOR_EXPORTS
#define INDICADOR_EXPORTS
#endif

#include "indicador.h"

// --------------------- ABERTURA E FECHAMENTO DE COMUNICACAO ---------------------

INDICADOR_API int openSerial ( const char* port, int baudRate, int dataBits, int stopBits, int parity )
{
    int result;
    if ( !isOpen() )
    {
        inicializarCriticalSection();
        result = serial_open( port, baudRate, dataBits, stopBits, parity );
        if (result == OK) // Se abertura OK, inicia thread recebedora de dados
        { 
            result = startIndicador(); // Inicializa thread
            if (result != OK) { close(); } // Aborta abertura, caso se der erro   
        }
        return result;
    } 
    else { return ERRO_COMUN_OPEN; }    
}

int setParametrosConfigSerial( int baudRate, int dataBits, int stopBits, int parity )
{
    switch (isOpen()) 
    {
        case 0:
            return ERRO_COMUN_CLOSED;
            break;
        case 1:
            return serial_setParametrosConfig(baudRate, dataBits, stopBits, parity);
            break;
        case 2:
        default:
            return ERRO;
            break;
    } 
}

INDICADOR_API int openTcp ( const char* ip  , int port )
{
    int result;
    if ( !isOpen() ) 
    {
        inicializarCriticalSection();
        result = tcp_open( ip, port );
        if (result == OK) // Se abertura OK, inicia thread recebedora de dados 
        {
            result = startIndicador(); // Inicializa thread
            if (result != OK) { close(); } // Aborta abertura, caso se der erro
        }
        return result;
    } else { return ERRO_COMUN_OPEN; }           
}

INDICADOR_API int isOpen (  ) // retorna 1 se serial aberto, 2 se tcp aberto e 0 se fechado.
{ 
    return (serial_isOpen() == OK ? 1 : ( tcp_isOpen() == OK ? 2 : 0 ) );
}

INDICADOR_API int close (  )
{
    if (isOpen()) {

        setIsClosing(TRUE);
        WaitForSingleObject( runThread, INFINITE );
        CloseHandle(runThread);
        setIsClosing(FALSE);

        freeStringResponse(&relatorioString, &relatorioStringLen);
        freeStringResponse(&produtoString, &produtoStringLen);
        freeStringResponse(&configString, &configStringLen);
        clear(&indicador);
        
        if ( serial_isOpen() )
        { 
            deleteCriticalSection();
            return serial_close();
        } 
        else if ( tcp_isOpen() ) 
        {
            deleteCriticalSection();
            return tcp_close();
        } 

    } else { return ERRO_COMUN_CLOSED; }
}
// --------------------- FIM ABERTURA E FECHAMENTO DE COMUNICACAO ---------------------

// --------------------- ZERO, TARA, IMPRIMIR, #PESO ---------------------

INDICADOR_API int zerar ( )
{
    if (!isOpen()) {return ERRO_COMUN_CLOSED;}
        
    int resp, elapsed_time = 0;
    setZeroResponse(WAITING_RESPONSE);
    resp = indicadorSend( "#ZERO\r\n" ); // Envia comando
    if (resp == OK) // espera resposta se comando for enviado com sucesso
    { 
        while ((resp = getZeroResponse()) == WAITING_RESPONSE && (elapsed_time * WAIT_RESPONSE < TIMEOUT))
        {
            Sleep(WAIT_RESPONSE);
            elapsed_time++;
        }
        resp = (resp == WAITING_RESPONSE ? ERRO_TIMEOUT : resp);
    }
    setZeroResponse(DEFAULT_RESPONSE);
    return resp;
}

INDICADOR_API int tarar (  )
{
    if (!isOpen()) {return ERRO_COMUN_CLOSED;}
        
    int resp, elapsed_time = 0;

    setTaraResponse(WAITING_RESPONSE);
    resp = indicadorSend( "#TARA\r\n" );

    if (resp == OK) 
    {
        while ((resp = getTaraResponse()) == WAITING_RESPONSE && (elapsed_time * WAIT_RESPONSE < TIMEOUT)) 
        {
            Sleep(WAIT_RESPONSE);
            elapsed_time++;
        }
        resp = (resp == WAITING_RESPONSE ? ERRO_TIMEOUT : resp);
    }
    setTaraResponse(DEFAULT_RESPONSE);
    return resp;
}

INDICADOR_API int tararManual ( int tara )
{
    if (!isOpen()) {return ERRO_COMUN_CLOSED;}
        
    int resp, elapsed_time = 0;
    char data[MAX_BUFFER_SIZE];

    sprintf(data, "#TARA,%d\r\n", tara);

    setTaraResponse(WAITING_RESPONSE);
    resp = indicadorSend( data );

    if (resp == OK) 
    {
        while ((resp = getTaraResponse()) == WAITING_RESPONSE && (elapsed_time * WAIT_RESPONSE < TIMEOUT)) 
        {
            Sleep(WAIT_RESPONSE);
            elapsed_time++;
        }
        resp = (resp == WAITING_RESPONSE ? ERRO_TIMEOUT : resp);
    }
    setTaraResponse(DEFAULT_RESPONSE);
    return resp;
}

INDICADOR_API int imprimir_calcularMedia ( )
{
    if (!isOpen()) {return ERRO_COMUN_CLOSED;}
        
    int resp, elapsed_time = 0;

    setImprResponse(WAITING_RESPONSE);
    resp = indicadorSend( "#IMPRIME\r\n" );
    if (resp == OK)
    {
        while ((resp = getImprResponse()) == WAITING_RESPONSE && (elapsed_time * WAIT_RESPONSE < TIMEOUT))
        {
            Sleep(WAIT_RESPONSE);
            elapsed_time++;
        }
        resp = (resp == WAITING_RESPONSE ? ERRO_TIMEOUT : resp);
    }
    setImprResponse(DEFAULT_RESPONSE);

    return resp;
}

INDICADOR_API int perguntarPeso(  )
{
    if (!isOpen()) {return ERRO_COMUN_CLOSED;}
        
    return indicadorSend( "#PESO\r\n" );
}
// --------------------- FIM ZERO, TARA, IMPRIMIR, #PESO ---------------------

// --------------------- PRODUTO ---------------------

INDICADOR_API int ERF_tabelaProduto (  )
{
    if (!isOpen()) { return ERRO_COMUN_CLOSED; }
    int resp, elapsed_time = 0;
    setProdutoResponse(WAITING_RESPONSE, 1);
    resp = indicadorSend("#ERF\r\n");
    if (resp == OK)
    {
        while ((resp = getProdutoResponse()) == WAITING_RESPONSE && (elapsed_time * WAIT_RESPONSE < TIMEOUT))
        {
            Sleep(WAIT_RESPONSE);
            elapsed_time++;
        }
        resp = (resp == WAITING_RESPONSE ? ERRO_TIMEOUT : resp);
    }
    setProdutoResponse(DEFAULT_RESPONSE, 1);
    return resp;
}

INDICADOR_API int cadastrarProduto ( const char* codigo, const char* descricao )
{
    if (!isOpen()) {return ERRO_COMUN_CLOSED;}
    if (strlen(codigo) > CODIGO_PRODUTO_LEN || strlen(descricao) > DESCRICAO_PRODUTO_LEN) {return ERRO_INVALID_ARG;}
        
    int resp, elapsed_time = 0;
    char data[MAX_BUFFER_SIZE];

    sprintf(data, "#CP,%s,%s*\r\n", codigo, descricao);
    setProdutoResponse(WAITING_RESPONSE, 1);
    resp = indicadorSend(data); // Enviando os dados de cadastro
    if (resp == OK) 
    {
        while ((resp = getProdutoResponse()) == WAITING_RESPONSE && (elapsed_time * WAIT_RESPONSE < TIMEOUT))
        {
            Sleep(WAIT_RESPONSE);
            elapsed_time++;
        }
        resp = (resp == WAITING_RESPONSE ? ERRO_TIMEOUT : resp);
    }
    setProdutoResponse(DEFAULT_RESPONSE, 1);
    return resp;
}

INDICADOR_API int alterarProduto ( const char* codigo, const char* descricao )
{
    if (!isOpen()) {return ERRO_COMUN_CLOSED;}
    if (strlen(codigo) > CODIGO_PRODUTO_LEN || strlen(descricao) > DESCRICAO_PRODUTO_LEN) {return ERRO_INVALID_ARG;}
        
    int resp, elapsed_time = 0;
    char data[MAX_BUFFER_SIZE];

    sprintf(data, "#CPED,%s,%s*\r\n", codigo, descricao);
    setProdutoResponse(WAITING_RESPONSE, 1);
    resp = indicadorSend(data); // Enviando os dados de cadastro alterado
    if (resp == OK) 
    {
        while ((resp = getProdutoResponse()) == WAITING_RESPONSE && (elapsed_time * WAIT_RESPONSE < TIMEOUT))
        {
            Sleep(WAIT_RESPONSE);
            elapsed_time++;
        }
        resp = (resp == WAITING_RESPONSE ? ERRO_TIMEOUT : resp);
    }
    setProdutoResponse(DEFAULT_RESPONSE, 1);
    return resp;
}

INDICADOR_API int deletarProduto ( const char* codigo )
{
    if (!isOpen()) {return ERRO_COMUN_CLOSED;}
    if (strlen(codigo) > CODIGO_PRODUTO_LEN ) {return ERRO_INVALID_ARG;}

    int resp, elapsed_time = 0;
    char data[MAX_BUFFER_SIZE];

    sprintf(data, "#DELC,%s*\r\n", codigo);
    setProdutoResponse(WAITING_RESPONSE, 1);
    resp = indicadorSend( data );
    if (resp == OK) 
    {
        resp = getProdutoResponse();
        while ((resp == WAITING_RESPONSE || resp == RESPONDING) && (elapsed_time * WAIT_RESPONSE < LONG_TIMEOUT)) 
        {

            Sleep(WAIT_RESPONSE);
            elapsed_time++;
            resp = getProdutoResponse();
        }
        resp = (resp == WAITING_RESPONSE ? ERRO_TIMEOUT : resp);   
    }
    setProdutoResponse(DEFAULT_RESPONSE, 1);
    return resp;
}

INDICADOR_API int EXC_iniciarTabelaProduto ()
{
    if (!isOpen()) { return ERRO_COMUN_CLOSED; }
    int resp, elapsed_time = 0;
    setProdutoResponse(WAITING_RESPONSE, 1);
    resp = indicadorSend("#EXC\r\n");
    if (resp == OK)
    {
        while ((resp = getProdutoResponse()) == WAITING_RESPONSE && (elapsed_time * WAIT_RESPONSE < TIMEOUT))
        {
            Sleep(WAIT_RESPONSE);
            elapsed_time++;
        }
        resp = (resp == WAITING_RESPONSE ? ERRO_TIMEOUT : resp);
    }
    setProdutoResponse(DEFAULT_RESPONSE, 1);
    return resp;
}

INDICADOR_API int EXC_cadastrarProduto ( const char* codigo, const char* descricao )
{
    if (!isOpen()) {return ERRO_COMUN_CLOSED;}
    if (strlen(codigo) > CODIGO_PRODUTO_LEN || strlen(descricao) > DESCRICAO_PRODUTO_LEN) {return ERRO_INVALID_ARG;}
        
    int resp, elapsed_time = 0;
    char data[MAX_BUFFER_SIZE];

    sprintf(data, "#EX,%s,%s*\r\n", codigo, descricao);
    setProdutoResponse(WAITING_RESPONSE, 1);
    resp = indicadorSend(data); // Enviando os dados de cadastro
    if (resp == OK) 
    {
        while ((resp = getProdutoResponse()) == WAITING_RESPONSE && (elapsed_time * WAIT_RESPONSE < TIMEOUT))
        {
            Sleep(WAIT_RESPONSE);
            elapsed_time++;
        }
        resp = (resp == WAITING_RESPONSE ? ERRO_TIMEOUT : resp);
    }
    setProdutoResponse(DEFAULT_RESPONSE, 1);
    return resp;
}

INDICADOR_API int EXC_finalizarTabelaProduto( int quantidadeProdutosTabelaEXC )
{
    if (!isOpen()) {return ERRO_COMUN_CLOSED;}
    if (quantidadeProdutosTabelaEXC > CAPACIDADE_MAXIMA_TABELA_PRODUTO || quantidadeProdutosTabelaEXC < 0) { return ERRO_INVALID_ARG; }
        
    int resp, elapsed_time = 0;
    char data[MAX_BUFFER_SIZE];

    sprintf(data, "#EXEND,%d*\r\n", quantidadeProdutosTabelaEXC);
    setProdutoResponse(WAITING_RESPONSE, 1);
    resp = indicadorSend(data); // Enviando os dados de cadastro
    if (resp == OK) 
    {
        while ((resp = getProdutoResponse()) == WAITING_RESPONSE && (elapsed_time * WAIT_RESPONSE < TIMEOUT))
        {
            Sleep(WAIT_RESPONSE);
            elapsed_time++;
        }
        resp = (resp == WAITING_RESPONSE ? ERRO_TIMEOUT : resp);
    }
    setProdutoResponse(DEFAULT_RESPONSE, 1);
    return resp;
}

INDICADOR_API const char* getTabelaProduto ( )
{
    if (!isOpen()) {return "#ERRO: -5";}
 
    int resp, elapsed_time = 0;
    freeStringResponse(&produtoString, &produtoStringLen);

    setProdutoResponse(WAITING_RESPONSE, 1);
    resp = indicadorSend( "#RWCP*\r\n" );
    if (resp == OK)
    {
        while( ((resp = getProdutoResponse()) == WAITING_RESPONSE || resp == RESPONDING) && (elapsed_time * WAIT_RESPONSE < LONG_TIMEOUT)) 
        {
            Sleep(WAIT_RESPONSE);
            elapsed_time++;
            if (resp == RESPONDING) { // REINICIA TIMER
                setProdutoResponse(WAITING_RESPONSE, 0);
                elapsed_time = 0;
            }
        }
        resp = (resp == WAITING_RESPONSE ? ERRO_TIMEOUT : resp);
    }
    setProdutoResponse(DEFAULT_RESPONSE, 1);

    switch (resp)
    {
        case ERRO:
            return "#ERRO: 0";
        case ERRO_TIMEOUT:
            return "#ERRO: -1";
        case ERRO_MEMORY:
            return "#ERRO: -3";
        default:
            return produtoString;
    } 
}

INDICADOR_API int deletarTabelaProduto (  )
{
    if (!isOpen()) {return ERRO_COMUN_CLOSED;}
    
    int resp, elapsed_time = 0;

    setProdutoResponse(WAITING_RESPONSE, 1);
    resp = indicadorSend( "#CLRCP\r\n" );
    if (resp == OK)
    {
        while ((resp = getProdutoResponse()) == WAITING_RESPONSE && (elapsed_time * WAIT_RESPONSE < LONG_TIMEOUT))
        {
            Sleep(WAIT_RESPONSE);
            elapsed_time++;
        }
        resp = (resp == WAITING_RESPONSE ? ERRO_TIMEOUT : resp);   
    }
    setProdutoResponse(DEFAULT_RESPONSE, 1);
    return resp;
}
// --------------------- FIM PRODUTO ---------------------

// --------------------- USUARIO ---------------------

INDICADOR_API const char* getUsuarioById ( int id )
{
    if (!isOpen()) {return "#ERRO: -5";}

    int resp, elapsed_time = 0;
    char data[MAX_BUFFER_SIZE];
    freeStringResponse(&configString, &configStringLen);

    sprintf(data, "#F113%d\r\n", id); 
           
    setConfigResponse(WAITING_RESPONSE);
    resp = indicadorSend( data );
    if (resp == OK)
    {
        while ((resp = getConfigResponse()) == WAITING_RESPONSE && (elapsed_time * WAIT_RESPONSE < TIMEOUT))
        {
            Sleep(WAIT_RESPONSE);
            elapsed_time++;
        }
        resp = (resp == WAITING_RESPONSE ? ERRO_TIMEOUT : resp);
    }
    setConfigResponse(DEFAULT_RESPONSE);

    switch (resp)
    {
        case ERRO:
            return "#ERRO: 0";
        case ERRO_TIMEOUT:
            return "#ERRO: -1";
        case ERRO_MEMORY:
            return "#ERRO: -3";
        default:
            return configString;
    } 
}

INDICADOR_API const char* setUsuario ( const char* senhaMenu, int idUsuario, const char* senhaUsuario, const char* nomeUsuario )
{
    if (!isOpen()) {return "#ERRO: -5";}

    int resp, elapsed_time = 0;
    char data[MAX_BUFFER_SIZE];
    freeStringResponse(&configString, &configStringLen);

    sprintf(data, "#F113,%s,%d,%s,%s*\r\n", senhaMenu, idUsuario, senhaUsuario, nomeUsuario); 
           
    setConfigResponse(WAITING_RESPONSE);
    resp = indicadorSend( data );
    if (resp == OK)
    {
        while ((resp = getConfigResponse()) == WAITING_RESPONSE && (elapsed_time * WAIT_RESPONSE < TIMEOUT))
        {
            Sleep(WAIT_RESPONSE);
            elapsed_time++;
        }
        resp = (resp == WAITING_RESPONSE ? ERRO_TIMEOUT : resp);
    }
    setConfigResponse(DEFAULT_RESPONSE);

    switch (resp)
    {
        case ERRO:
            return "#ERRO: 0";
        case ERRO_TIMEOUT:
            return "#ERRO: -1";
        case ERRO_MEMORY:
            return "#ERRO: -3";
        default:
            return configString;
    } 
}
// --------------------- FIM USUARIO ---------------------

// --------------------- CONFIGURACAO ---------------------

// modoInput = 0 - intInput utilizada
// modoInput = 1 - charInput utilizada
INDICADOR_API const char* setConfiguracaoIndicador ( int funcao, const char* input )
{
    if (!isOpen()) {return "#ERRO: -5";}

    int inputLen;
    int resp, elapsed_time = 0;
    char data[MAX_BUFFER_SIZE];
    freeStringResponse(&configString, &configStringLen);

    if (input != NULL)
    {
        inputLen = strlen(input);
    } else
    {
        return "#ERRO: -2";
    }

    if ( funcao == 100 || funcao == 102 || funcao == 103 || funcao == 104 ||
         funcao == 105 || funcao == 106 || funcao == 107 || funcao == 108 ||
         funcao == 110 || funcao == 114 || funcao == 115 || funcao == 117 ||
         funcao == 118 || funcao == 120 || funcao == 200 || funcao == 201 ||
         funcao == 202 || funcao == 203 || funcao == 204 || funcao == 205 ||
         funcao == 206 || funcao == 207 || funcao == 208 || funcao == 209 ||
         funcao == 210 || funcao == 211 || funcao == 213 || funcao == 215 ||
         funcao == 216 || funcao == 217 || funcao == 219 || funcao == 300 ||
         funcao == 301 || funcao == 303 || funcao == 304 || funcao == 305 ||
         funcao == 307 || funcao == 308 || funcao == 309 || funcao == 310 ||
         funcao == 311 || funcao == 312 || funcao == 314 || funcao == 315 ||
         funcao == 316 || funcao == 400 || funcao == 407 || funcao == 408 ||
         funcao == 416 || funcao == 417 || funcao == 418 || funcao == 419 ||
         funcao == 450 || funcao == 451 || funcao == 460 || funcao == 502 ||
         funcao == 600 || funcao == 602 || funcao == 604 || funcao == 605 ||
         funcao == 700 )
    {
        sprintf(data, "#F%d%s\r\n", funcao, input); 
    } else if ( funcao == 109 || funcao == 112 || funcao == 212 || funcao == 302 ||
                funcao == 401 || funcao == 402 || funcao == 403 || funcao == 404 ||
                funcao == 405 || funcao == 406 || funcao == 409 || funcao == 410 ||
                funcao == 411 || funcao == 412 || funcao == 413 || funcao == 414 ||
                funcao == 415 || funcao == 452 || funcao == 453 || funcao == 454 ||
                funcao == 500 || funcao == 501 || funcao == 503 || funcao == 504 ||
                funcao == 601 ) 
    {
        sprintf(data, "#F%d,%s*\r\n", funcao, input);
    } else
    {
        return "#ERRO: -2";
    }
           
    setConfigResponse(WAITING_RESPONSE);
    resp = indicadorSend( data );
    if (resp == OK)
    {
        while ((resp = getConfigResponse()) == WAITING_RESPONSE && (elapsed_time * WAIT_RESPONSE < TIMEOUT))
        {
            Sleep(WAIT_RESPONSE);
            elapsed_time++;
        }
        resp = (resp == WAITING_RESPONSE ? ERRO_TIMEOUT : resp);
    }
    setConfigResponse(DEFAULT_RESPONSE);

    switch (resp)
    {
        case ERRO:
            return "#ERRO: 0";
        case ERRO_TIMEOUT:
            return "#ERRO: -1";
        case ERRO_MEMORY:
            return "#ERRO: -3";
        default:
            return configString;
    } 
}

INDICADOR_API const char* getConfiguracaoIndicador ( int funcao )
{
    if (!isOpen()) {return "#ERRO: -5";}

    int resp, elapsed_time = 0;
    char data[MAX_BUFFER_SIZE];
    freeStringResponse(&configString, &configStringLen);

    if ( funcao == 198 )
        strcpy(data, "#SF\r\n");
    else
        sprintf(data, "#F%d\r\n", funcao);

    setConfigResponse(WAITING_RESPONSE);
    resp = indicadorSend( data );
    if (resp == OK)
    {
        while ((resp = getConfigResponse()) == WAITING_RESPONSE && (elapsed_time * WAIT_RESPONSE < TIMEOUT))
        {
            Sleep(WAIT_RESPONSE);
            elapsed_time++;
        }
        resp = (resp == WAITING_RESPONSE ? ERRO_TIMEOUT : resp);
    }
    setConfigResponse(DEFAULT_RESPONSE);

    switch (resp)
    {
        case ERRO:
            return "#ERRO: 0";
        case ERRO_TIMEOUT:
            return "#ERRO: -1";
        case ERRO_MEMORY:
            return "#ERRO: -3";
        default:
            return configString;
    } 
}

INDICADOR_API int saveConfiguracaoIndicador(  )
{
    if (!isOpen()) {return ERRO_COMUN_CLOSED;}

    int resp, elapsed_time = 0;

    setConfigResponse(WAITING_RESPONSE);
    resp = indicadorSend( "#F000\r\n" );

    if (resp == OK)
    {
        while ((resp = getConfigResponse()) == WAITING_RESPONSE && (elapsed_time * WAIT_RESPONSE < LONG_TIMEOUT))
        {
            Sleep(WAIT_RESPONSE);
            elapsed_time++;
        }
        resp = (resp == WAITING_RESPONSE ? ERRO_TIMEOUT : resp);
    }
    setConfigResponse(DEFAULT_RESPONSE);
    return resp;
}
// --------------------- FIM CONFIGURACAO ---------------------

// --------------------- RELATORIO ---------------------

INDICADOR_API const char* gerarRelatorio( int tipo, const char* input )
{
    if (!isOpen()) {return "#ERRO: -5";}

    int resp, elapsed_time = 0;
    char data[MAX_BUFFER_SIZE];
    freeStringResponse(&relatorioString, &relatorioStringLen);

    switch(tipo)
    {
        case 0:
            sprintf(data, "#SNDRL,%s*\r\n", input);
            break;
        case 1:
            sprintf(data, "#SNDDT,%s*\r\n", input);
            break;
        case 2:
            sprintf(data, "#SNDRCP,%s*\r\n", input);
            break;
        case 3:
            sprintf(data, "#SNDRUSER,%s*\r\n", input);
            break;
        case 4:
            sprintf(data, "#SNDDTCOD,%s*\r\n", input);
            break;
        case 5:
            sprintf(data, "#SNDDTUSR,%s*\r\n", input);
            break;
        default:
            return "#ERRO: -2";
    }
    setRelatorioResponse(WAITING_RESPONSE, 1);
    resp = indicadorSend( data );

    if (resp == OK)
    {
        while( ((resp = getRelatorioResponse()) == WAITING_RESPONSE || resp == RESPONDING) && (elapsed_time * WAIT_RESPONSE < LONG_TIMEOUT))
        {
            Sleep(WAIT_RESPONSE);
            elapsed_time++;
            if( resp == RESPONDING ) { // REINICIA TIMER
                setRelatorioResponse(WAITING_RESPONSE, 0);
                elapsed_time = 0;
            }
        }
        resp = (resp == WAITING_RESPONSE ? ERRO_TIMEOUT : resp);
    }
    setRelatorioResponse(DEFAULT_RESPONSE, 1);

    switch (resp)
    {
        case ERRO:
            return "#ERRO: 0";
        case ERRO_TIMEOUT:
            return "#ERRO: -1";
        case ERRO_MEMORY:
            return "#ERRO: -3";
        default:
            return relatorioString;
    } 
}
// --------------------- FIM RELATORIO ---------------------

INDICADOR_API void freeStringMemory (  )
{
    if (getConfigResponse() == DEFAULT_RESPONSE)
    {
        freeStringResponse(&configString, &configStringLen);
    } 
    if (getRelatorioResponse() == DEFAULT_RESPONSE)
    {
        freeStringResponse(&relatorioString, &relatorioStringLen);
    }
    if (getProdutoResponse() == DEFAULT_RESPONSE)
    {
        freeStringResponse(&produtoString, &produtoStringLen);
    }  
}

//-------------------------- PRIVATE --------------------------

static int indicadorSend ( const char* data   )
{
    if ( serial_isOpen() ) { return serial_send( data ); } 
    else if ( tcp_isOpen() ) { return tcp_send( data ); } 
    else { return ERRO; }
}

static char indicadorReceive (  )
{
    if ( serial_isOpen() ) { return serial_receive(); } 
    else if ( tcp_isOpen() ) { return tcp_receive(); } 
    else { return CHAR_VAZIO; }
}

static void clear ( dadosIndicador *ind )
{
    ind->pesoLiquido = 0;
    ind->pesoBruto = 0;
    ind->tara = 0;
    ind->precisaoDecimalPeso = 0;
    ind->statusBalanca = STATUS_BALANCA_DESCONHECIDO;
    ind->unidadeMedida = UNIDADE_MEDIDA_DESCONHECIDO;
    ind->isModoContadoraPecas = FALSE;
    ind->protocolo = PROTOCOLO_DESCONHECIDO;
    ind->statusBateriaIndicador = STATUS_BATERIA_DESCONHECIDO;
    ind->nivelBateriaIndicador = NIVEL_BATERIA_DESCONHECIDO;
    ind->nivelBateriaTransmissorTx1 = NIVEL_BATERIA_DESCONHECIDO;
    ind->nivelBateriaTransmissorTx2 = NIVEL_BATERIA_DESCONHECIDO;
    memset(ind->codigoProduto, '\0', CODIGO_PRODUTO_LEN+1);
    memset(ind->data, '\0', DATA_LEN+1);
    memset(ind->hora, '\0', HORA_LEN+1);
}

static void freeStringResponse ( char **dataString, int *dataStringLen )
{
    free(*dataString); // Limpando ponteiro para string
    *dataString = NULL;
    *dataStringLen = 0;
}

static int startIndicador()
{
    clear(&indicador);
    if (isOpen()) 
    {
        runThread = (HANDLE)_beginthreadex(NULL, 0, &runIndicador, NULL, 0, &runThreadID);
        if (runThread <= 0) {return ERRO;}
        else {return OK;}
    } 
    else { return ERRO; }     
}

static unsigned __stdcall runIndicador( void* pArguments )
{
    char buffer[MAX_BUFFER_SIZE];
    int pos = 0, bufferLen = 0, respAux;
   
    while ( isOpen() && !(getIsClosing()) ) 
    {
        pos = 0; 
        memset(buffer, '\0', MAX_BUFFER_SIZE); // Limpando buffer

        while((buffer[pos] = indicadorReceive()) != CHAR_VAZIO && pos < MAX_BUFFER_SIZE) // Recebe bytes do indicador
        {
            if (buffer[pos] == 0x02 || buffer[pos] == '\n') { break; }
            else { pos++; }
        } // PROBLEMAS COM TEMPO DE ENVIO DO INDICADOR

        // while(pos < MAX_BUFFER_SIZE && ( isOpen() && !(getIsClosing()) ) ) // Recebe bytes do indicador
        // {
        //     buffer[pos] = indicadorReceive();
        //     if (buffer[pos] != CHAR_VAZIO)
        //     {
        //         if (buffer[pos] == 0x02 || buffer[pos] == '\n') { break; }
        //         else { pos++; }
        //     }
        // } // SOLUCAO DO PROBLEMA DE TEMPO DE ENVIO DO INDICADOR

        // Retira os caracteres de inicio e de fim de transmissao
        deleteCharFromString(buffer, (char)0x01);
        deleteCharFromString(buffer, (char)0x02);
        deleteCharFromString(buffer, '\r');
        deleteCharFromString(buffer, '\n');
        //printf("%s\n", buffer);

        bufferLen = strlen(buffer);
        if (bufferLen <= 0) {continue;} // caso não tenha recebido nada

        // ------- Verificacoes de resposta a requisicoes -------
        // VERIFICACAO ZERO
        if ( strstr(buffer, "#ZERO" ) ) 
        {
            if ( strstr(buffer, "NOACK") ) { setZeroResponse(ZERO_NOACK_0); }
            else if ( strstr(buffer, "ACK") ) { setZeroResponse(ZERO_ACK_0); }
            continue;
        }
        // VERIFICACAO TARA
        if ( strstr(buffer, "#TARA" ) ) 
        {
            if ( strstr(buffer, "NOACK") ) { setTaraResponse((-1 * atoi(buffer+12)) + TARA_NOACK); }
            else if ( strstr(buffer, "ACK") ) { setTaraResponse(atoi(buffer+10) + TARA_ACK); }
            continue;
        }
        // VERIFICACAO IMPRESSAO
        if ( strstr(buffer, "#IMPRIME" ) ) 
        {
            if ( strstr(buffer, "NOACK") ) { setImprResponse(IMPRIME_NOACK_0); }
            else if ( strstr(buffer, "ACK") ) { setImprResponse(atoi(buffer+13) + IMPRIME_ACK); }
            continue;
        }
        // VERIFICACAO CONFIGURACAO
        if ( strstr(buffer, "#F000" ) ) 
        {
            setConfigResponse(OK);
            continue;
        }
        else if ( strstr(buffer, "#F" ) ) 
        {
            configStringLen = bufferLen + 1; // espaco extra para \0
            configString = (char*)malloc(sizeof(char) * configStringLen);
            if (configString != NULL) 
            {
                memset(configString, '\0', configStringLen);
                strcpy(configString, buffer);
                setConfigResponse(OK);
            } 
            else 
            {
                setConfigResponse(ERRO_MEMORY);
                freeStringResponse(&configString, &configStringLen);
            }
            continue;
        }
        // VERIFICACAO RELATORIO
        if ( strstr(buffer, "#SNDRL,ACK" ) || strstr(buffer, "#SNDDT,ACK" ) ||
                strstr(buffer, "#SNDRCP,ACK" ) || strstr(buffer, "#SNDRUSER,ACK" ) ||
                strstr(buffer, "#SNDDTCOD,ACK" ) || strstr(buffer, "#SNDDTUSR,ACK" ) ) // INICIO RELATORIO
        {
            relatorioStringLen += bufferLen + 1; // espaco extra para \0
            relatorioString = (char*)malloc(sizeof(char) * relatorioStringLen);
            if (relatorioString != NULL) 
            {
                memset(relatorioString, '\0', relatorioStringLen);
                strcpy(relatorioString, buffer);
                setRelatorioResponse(RESPONDING, 1);
            }
            else 
            { 
                setRelatorioResponse(ERRO_MEMORY, 1);
                freeStringResponse(&relatorioString, &relatorioStringLen);
            }
            continue;
        }
        else if (strstr(buffer, "#SPS" ) || strstr(buffer, "#SPC" ) || 
                strstr(buffer, "#STLP" ) || strstr(buffer, "#STLC" )) // MEIO RELATORIO 
        {
            relatorioStringLen += bufferLen + 1; // espaco extra para \n
            relatorioString = realloc(relatorioString, sizeof(char) * relatorioStringLen);
            if (relatorioString != NULL) 
            {
                strcat(relatorioString, "\n");
                strcat(relatorioString, buffer);
                setRelatorioResponse(RESPONDING, 1);
            } 
            else 
            {
                setRelatorioResponse(ERRO_MEMORY, 1);
                freeStringResponse(&relatorioString, &relatorioStringLen);
            }
            continue;
        } 
        else if ( strstr(buffer, "#SNDFIM" ) ) // FIM de relatorio 
        {
            setRelatorioResponse(OK, 1);
            continue;
        }         
        // VERIFICACAO PRODUTO
        if ( strstr(buffer, "#ACKRWCP" ) ) // INICIO PRODUTO
        { 
            produtoStringLen += bufferLen + 1; // espaco extra para \0
            produtoString = (char*)malloc(sizeof(char) * produtoStringLen);
            if (produtoString != NULL)
            {
                memset(produtoString, '\0', produtoStringLen);
                strcpy(produtoString, buffer);
                setProdutoResponse(RESPONDING, 1);
            }
            else 
            {
                setProdutoResponse(ERRO_MEMORY, 1);
                freeStringResponse(&produtoString, &produtoStringLen);
            }
            continue;
        }
        else if ( strstr(buffer, "#RWCP" ) ) // MEIO PRODUTO
        { 
            produtoStringLen += bufferLen + 1; // espaco extra para \n
            produtoString = realloc(produtoString, sizeof(char) * produtoStringLen);
            if (produtoString != NULL) 
            {
                strcat(produtoString, "\n");
                strcat(produtoString, buffer);
                setProdutoResponse(RESPONDING, 1);
            } 
            else
            {
                setProdutoResponse(ERRO_MEMORY, 1);
                freeStringResponse(&produtoString, &produtoStringLen);
            }
            continue;
        }
        else if ( strstr(buffer, "#ACKCP" ) || strstr(buffer, "#ACKEX" ) ||
            strstr(buffer, "#ACKCPED" ) || strstr(buffer, "#ACKDELC" ) ||
            strstr(buffer, "#ENDCP" ) || strstr(buffer, "#ENDCLRCP" ) || strstr(buffer, "#ACKEXEND" )) // FIM PRODUTO 
        {
            setProdutoResponse(OK, 1);
            continue;
        }
        // }
        // ------- FIM DAS VERIFICACOES DE RESPOSTA -------
        filterStringValues(buffer);    
        //Sleep(WAIT_READY);
    }
    _endthreadex( 0 );
    return 0;
}

static int checksum2Compl ( char* buffer, char separator )
{
    int i, j, lastSep;
    int bufferLen = strlen(buffer);

    for ( i = 0; i < bufferLen; i++ ) // Encontra o ultimo separador
    { 
        if (buffer[i] == separator) { lastSep = i; } 
    }

    char sumStr[MAX_BUFFER_SIZE];
    memset(sumStr, '\0', sizeof(sumStr));
    strncpy(sumStr, buffer + lastSep + 1, 2); // Adquire o valor de SUM

    int sum = strtol(sumStr, NULL, 16); // Converte o valor de SUM para decimal

    int check = 0;
    // Calcula o CHECK
    for ( i = 0; i < lastSep + 1; i++ ) { check += ((int)buffer[i]); }
    check &= 0xFF;
    check = (0x100 - check) & 0xFF;

    if (check == sum) { return TRUE; }
    else { return FALSE; }  
}

static int checksum ( char* buffer, char separator )
{
    int i, j, lastSep;
    int bufferLen = strlen(buffer);

    for ( i = 0; i < bufferLen; i++ ) // Encontra o ultimo separador
    { 
        if (buffer[i] == separator) { lastSep = i; } 
    }

    char sumStr[MAX_BUFFER_SIZE];
    memset(sumStr, '\0', sizeof(sumStr));
    strncpy(sumStr, buffer + lastSep + 1, 2); // Adquire o valor de SUM

    int sum = strtol(sumStr, NULL, 16); // Converte o valor de SUM para decimal

    int check = 0;
    // Calcula o CHECK
    for ( i = 0; i < lastSep + 1; i++ ) { check = check ^ ((int)buffer[i]); }

    if (check == sum) { return TRUE; }
    else { return FALSE; }    
}

static void deleteCharFromString(char* string, char chdelete)
{
    int i, j, str_len;
    str_len = strlen(string);
    for (i = 0; i < str_len; i++) 
    {
        while (string[i] == chdelete) 
        {
            for (j = i; j < str_len-1; j++) {string[j] = string[j + 1];}
            string[str_len-1] = '\0';
            str_len--;
        }
    }
}

static int  contaCasasDecimaisNumeroString( const char* numero )
{
    if (numero == NULL) { return 0; }

    int casasDecimais = 0;

    while ( *numero != '\0' && *numero != '.' ) { numero++; }

    if ( *numero == '.' )
    {
        numero++;
        while ( *numero != '\0' )
        {
            casasDecimais++;
            numero++;
        }
    }

    return casasDecimais;
}

static int  verificaData(const char* data) // Verifica se o formato da string esta dd/MM/yyyy
{
    if (data == NULL) {return FALSE;}
    if (strlen(data) >= DATA_LEN) {
        return ((data[0] >= '0' && data[0] <= '9') &&
                (data[1] >= '0' && data[1] <= '9') &&
                (data[2] == '/') &&
                (data[3] >= '0' && data[3] <= '9') &&
                (data[4] >= '0' && data[4] <= '9') &&
                (data[5] == '/') &&
                (data[6] >= '0' && data[6] <= '9') &&
                (data[7] >= '0' && data[7] <= '9') &&
                (data[8] >= '0' && data[8] <= '9') &&
                (data[9] >= '0' && data[9] <= '9') );
    } else 
        return FALSE;
}
static int  verificaHora(const char* hora) // Verifica se o formato da string esta HH:mm:ss 
{
    if (hora == NULL) {return FALSE;}
    if (strlen(hora) >= HORA_LEN) {
        return ((hora[0] >= '0' && hora[0] <= '9') &&
                (hora[1] >= '0' && hora[1] <= '9') &&
                (hora[2] == ':') &&
                (hora[3] >= '0' && hora[3] <= '9') &&
                (hora[4] >= '0' && hora[4] <= '9') &&
                (hora[5] == ':') &&
                (hora[6] >= '0' && hora[6] <= '9') &&
                (hora[7] >= '0' && hora[7] <= '9') );
    } else 
        return FALSE;
}
static int  verificaCodigoProduto(const char* codigoProduto) // Verifica se a string tem no maximo 14 caracteres numericos
{
    if (codigoProduto == NULL) {return FALSE;}
    int len = 0;

    // Percorre a String ate
    while(*codigoProduto != '\0' &&  // encontrar o fim da string
    (*codigoProduto >= '0' && *codigoProduto <= '9') && // encontrar um caracter nao desejado
    ++len < CODIGO_PRODUTO_LEN+1) // ultrapassar o tamanho maximo de um codigo de produto
    {
        codigoProduto++;
    }

    if (*codigoProduto != '\0' || len == 0) {return FALSE;}
    else {return TRUE;}
}

static void filterStringValues( char* buffer )
{
    if (buffer == NULL || strlen(buffer) == 0) { return; }

    char *token, *replace;
    char values[MAX_QTD_VALUES][MAX_BUFFER_SIZE], terminator[6];
    char separator[] = " ";
    int indiceStatus, i;
    dadosIndicador valuesIndicador;
    clear(&valuesIndicador);

    for (i = 0 ; i < MAX_QTD_VALUES; i++)
    {
        memset(values[i], '\0', MAX_BUFFER_SIZE);
    }

    if ( strchr(buffer, ' ') != NULL ) // Identifica o tipo de delimitador
    { 
        strcpy(separator, " ");
        valuesIndicador.protocolo = PROTOCOLO_LIDER_2;
    }
    else if ( strchr(buffer, ',') != NULL )
    {
        strcpy(separator, ",");
        valuesIndicador.protocolo = PROTOCOLO_LIDER_4;
    }
    else { return; }  
    
    char bufferCopy[MAX_BUFFER_SIZE];
    int size = 0; // Quantidade de valores apos o split
    strcpy(bufferCopy, buffer);

    token = strtok(bufferCopy, separator); // Split

    for ( i = 0; token != NULL && i < MAX_QTD_VALUES; i++ )
    {
        strcpy(values[i], token);
        size++;
        token = strtok(NULL, separator);
    }
    
    if (size < 2) { return; } // Verifica se obteve a quantidade minima de valores, 2 (Peso, Status)

    // Busca o indice do valor de StatusBalanca
    if (strpbrk(values[1], "EIPMARG"))
    {
        valuesIndicador.protocolo--;
        indiceStatus = 1;
    }
    else if (size >= 4 && strpbrk(values[3], "EIPMARG"))
    {
        indiceStatus = 3;
    } 
    else { return; }
    // Caso o indice do statusBalanca nao seja o 1 e nem o 3, protocolo desconhecido ou dados corrompidos
    // Descarta dados

    // Tratar cada um dos valores para suas respectivas variaveis
    if ( (valuesIndicador.protocolo == PROTOCOLO_LIDER_3 || valuesIndicador.protocolo == PROTOCOLO_LIDER_4) 
        && size > (indiceStatus + 1) ) // TRATAMENTO DE DADOS ADICIONAIS DOS PROTOCOLO LIDER 3 E LIDER 4
    { 
        int byteCampoAdicional;
        byteCampoAdicional = strtol(values[indiceStatus + 1], NULL, 16);
            int bit7 = (byteCampoAdicional & (1 << 7)) >> 7; 
            int bit6 = (byteCampoAdicional & (1 << 6)) >> 6;  
            int bit5 = (byteCampoAdicional & (1 << 5)) >> 5; 
            int bit4 = (byteCampoAdicional & (1 << 4)) >> 4; 
            int bit3 = (byteCampoAdicional & (1 << 3)) >> 3; 
            int bit2 = (byteCampoAdicional & (1 << 2)) >> 2; 
            int bit1 = (byteCampoAdicional & (1 << 1)) >> 1; 
            int bit0 = (byteCampoAdicional & 1);             

        if ( (2*bit2 + 2*bit3 + 2*bit4 + bit5) == (size - indiceStatus - 2) ) // Verifica se ha campos correspondentes
        {
            if (bit5) // Checksum
            {
                if(!checksum(buffer, separator[0])) { return; }  
            }
            if (bit0 || bit1)
            {
                //if( (byteCampoAdicional & 3) > 0 ) { // obtem somente o valor de bit 0 e bit 1
                valuesIndicador.unidadeMedida = (byteCampoAdicional & 3) ^ 3;
                // 01 - kg (id = 2)
                // 10 - lb (id = 1)
                // 11 - kN (id = 0)
            }

            i = indiceStatus + 2;
            if (bit2) // Verifica a bateria do Indicador
            { 
                switch(values[i++][0])
                {
                    case CHAR_STATUS_BATERIA_CARREGANDO:
                        valuesIndicador.statusBateriaIndicador = STATUS_BATERIA_CARREGANDO;
                        break;
                    case CHAR_STATUS_BATERIA_EM_BATERIA:
                        valuesIndicador.statusBateriaIndicador = STATUS_BATERIA_EM_BATERIA;
                        break;
                    case CHAR_STATUS_BATERIA_CARGA_COMPLETA:
                        valuesIndicador.statusBateriaIndicador = STATUS_BATERIA_CARGA_COMPLETA;
                        break;    
                    default:
                        valuesIndicador.statusBateriaIndicador = STATUS_BATERIA_DESCONHECIDO;
                }
                valuesIndicador.nivelBateriaIndicador = atoi(values[i++]);
            }

            if (bit3) // Verifica a bateria do TransmissorTx1
            {
                i++; // Status da Bateria do TransmissorTx1 nao implementado
                valuesIndicador.nivelBateriaTransmissorTx1 = atoi(values[i++]);
            }

            if (bit4) // Verifica a bateria do TransmissorTx2
            {
                i++; // Status da Bateria do TransmissorTx2 nao implementado
                valuesIndicador.nivelBateriaTransmissorTx2 = atoi(values[i++]);
            }
        }      
    } // FIM TRATAMENTO DE DADOS ADICIONAIS

    if ((valuesIndicador.protocolo == PROTOCOLO_LIDER_2) && size >= (indiceStatus + 1) ) // VERIFICA PROTOCOLO LIDER 10
    {
        if ( ((indiceStatus+1 < size) && verificaData(values[indiceStatus + 1])) &&
             ((indiceStatus+2 < size) && verificaHora(values[indiceStatus + 2])) ) // CAMPO DE DATA E HORA OBRIGATORIOS
        {
            strncpy(valuesIndicador.data, values[indiceStatus + 1], DATA_LEN+1);
            valuesIndicador.data[DATA_LEN] = '\0';
            strncpy(valuesIndicador.hora, values[indiceStatus + 2], HORA_LEN+1);
            valuesIndicador.hora[HORA_LEN] = '\0';

            valuesIndicador.protocolo = PROTOCOLO_LIDER_10;
            if (strstr(buffer, "               ")) // Verifica se o espaco para o codigo esta vazio
            { // Se sim
                if (indiceStatus+3 < size)// Verifica se ha checksum
                { 
                    if (!checksum2Compl(buffer, separator[0])) {return;}
                }
            } else // Se nao
            {
                if ((indiceStatus+3 < size) && verificaCodigoProduto(values[indiceStatus + 3])) // CAMPO DE CODIGO
                {
                    strncpy(valuesIndicador.codigoProduto, values[indiceStatus + 3], CODIGO_PRODUTO_LEN+1);
                    valuesIndicador.codigoProduto[CODIGO_PRODUTO_LEN] = '\0';
                }

                if (indiceStatus+4 < size) // CAMPO CHECKSUM
                {
                    if (!checksum2Compl(buffer, separator[0])) {return;}
                }
            }
        }
    }

    switch(values[indiceStatus][0])
    {
        case CHAR_STATUS_BALANCA_ESTAVEL:
            valuesIndicador.statusBalanca = STATUS_BALANCA_ESTAVEL;
            break;
        case CHAR_STATUS_BALANCA_INSTAVEL:
            valuesIndicador.statusBalanca = STATUS_BALANCA_INSTAVEL;
            break;
        case CHAR_STATUS_BALANCA_VALOR_PICO:
            valuesIndicador.statusBalanca = STATUS_BALANCA_VALOR_PICO;
            break;
        case CHAR_STATUS_BALANCA_PESO_MEDIO:
            valuesIndicador.statusBalanca = STATUS_BALANCA_PESO_MEDIO;
            break;
        case CHAR_STATUS_BALANCA_AGUARDANDO:
            valuesIndicador.statusBalanca = STATUS_BALANCA_AGUARDANDO;
            break;
        case CHAR_STATUS_BALANCA_PERDA_COMUNICACAO:
            valuesIndicador.statusBalanca = STATUS_BALANCA_PERDA_COMUNICACAO;
            break;
        case CHAR_STATUS_BALANCA_IMPRIME:
            valuesIndicador.statusBalanca = STATUS_BALANCA_IMPRIME;
            break;
        default:
            valuesIndicador.statusBalanca = STATUS_BALANCA_DESCONHECIDO;
    }

    // Verifica se ha excesso de peso na balanca
    if (!strncmp(values[indiceStatus - 1], "SSSSSS", 6))
    {
        valuesIndicador.statusBalanca = STATUS_BALANCA_PESO_EXCESSO_POSITIVO;
    }
    else if (!strncmp(values[indiceStatus - 1], "NNNNNN", 6))
    {
        valuesIndicador.statusBalanca = STATUS_BALANCA_PESO_EXCESSO_NEGATIVO;
    }
    else // peso sem excesso
    {
        if ( strchr(values[indiceStatus-1], '.') ) // Verifica se ha ponto decimal no valor de peso
        {
            valuesIndicador.isModoContadoraPecas = FALSE; // Se tiver ponto decimal, eh valor de peso
        }   
        else
        {
            valuesIndicador.isModoContadoraPecas = TRUE; //  Se nao, eh valor unitario (contagem)
        }  
        
        //if (indiceStatus == 1) // Protocolo SEM valor de Tara e PesoBruto
        if (indiceStatus == 3) // Protocolo COM valor de Tara e PesoBruto
        {
            valuesIndicador.precisaoDecimalPeso = contaCasasDecimaisNumeroString(values[indiceStatus-3]); // Adquire casas decimais do peso bruto

            valuesIndicador.pesoBruto = atof(values[indiceStatus-3]);
            valuesIndicador.tara = atof(values[indiceStatus-2]);
        } else { // Se nao ha peso bruto
            valuesIndicador.precisaoDecimalPeso = contaCasasDecimaisNumeroString(values[indiceStatus-1]); // Adquire casas decimais do peso liquido
        }
        valuesIndicador.pesoLiquido = atof(values[indiceStatus-1]);
    }
    setDadosIndicador(valuesIndicador);
    return;
}

static void inicializarCriticalSection()
{
    InitializeCriticalSection(&dadosIndicadorCriticalSection);
    InitializeCriticalSection(&zeroResponseCriticalSection);
    InitializeCriticalSection(&taraResponseCriticalSection);
    InitializeCriticalSection(&imprResponseCriticalSection);
    InitializeCriticalSection(&relatorioResponseCriticalSection);
    InitializeCriticalSection(&configResponseCriticalSection);
    InitializeCriticalSection(&produtoResponseCriticalSection);
    InitializeCriticalSection(&isClosingCriticalSection);
    indicadorCriticalSectionInicialized = TRUE;
}
static void deleteCriticalSection()
{
    DeleteCriticalSection(&dadosIndicadorCriticalSection);
    DeleteCriticalSection(&zeroResponseCriticalSection);
    DeleteCriticalSection(&taraResponseCriticalSection);
    DeleteCriticalSection(&imprResponseCriticalSection);
    DeleteCriticalSection(&relatorioResponseCriticalSection);
    DeleteCriticalSection(&configResponseCriticalSection);
    DeleteCriticalSection(&produtoResponseCriticalSection);
    DeleteCriticalSection(&isClosingCriticalSection);
    indicadorCriticalSectionInicialized = FALSE;
}



// --------------- GET and SET ---------------

INDICADOR_API double getPesoLiquido (  )
{
    double value;
    if (indicadorCriticalSectionInicialized)
        EnterCriticalSection(&dadosIndicadorCriticalSection);
    value = indicador.pesoLiquido;
    if (indicadorCriticalSectionInicialized)
        LeaveCriticalSection(&dadosIndicadorCriticalSection);
    return value;
}

INDICADOR_API double getPesoBruto (  )
{
    double value;
    if (indicadorCriticalSectionInicialized)
        EnterCriticalSection(&dadosIndicadorCriticalSection);
    value = indicador.pesoBruto;
    if (indicadorCriticalSectionInicialized)
        LeaveCriticalSection(&dadosIndicadorCriticalSection);
    return value;
}

INDICADOR_API double getTara (  )
{
    double value;
    if (indicadorCriticalSectionInicialized)
        EnterCriticalSection(&dadosIndicadorCriticalSection);
    value = indicador.tara;
    if (indicadorCriticalSectionInicialized)
        LeaveCriticalSection(&dadosIndicadorCriticalSection);
    return value;
}

INDICADOR_API int    getPrecisaoDecimalPeso (  )
{
    int value;
    if (indicadorCriticalSectionInicialized)
        EnterCriticalSection(&dadosIndicadorCriticalSection);
    value = indicador.precisaoDecimalPeso;
    if (indicadorCriticalSectionInicialized)
        LeaveCriticalSection(&dadosIndicadorCriticalSection);
    return value;
} 

INDICADOR_API int   getStatusBalanca (  )
{
    int value;
    if (indicadorCriticalSectionInicialized)
        EnterCriticalSection(&dadosIndicadorCriticalSection);
    value = indicador.statusBalanca;
    if (indicadorCriticalSectionInicialized)
        LeaveCriticalSection(&dadosIndicadorCriticalSection);
    return value;
}

INDICADOR_API int    getUnidadeMedida (  )
{
    int value;
    if (indicadorCriticalSectionInicialized)
        EnterCriticalSection(&dadosIndicadorCriticalSection);
    value = indicador.unidadeMedida;
    if (indicadorCriticalSectionInicialized)
        LeaveCriticalSection(&dadosIndicadorCriticalSection);
    return value;
}

INDICADOR_API int    isModoContadoraDePecas (  )
{
    int value;
    if (indicadorCriticalSectionInicialized)
        EnterCriticalSection(&dadosIndicadorCriticalSection);
    value = indicador.isModoContadoraPecas;
    if (indicadorCriticalSectionInicialized)
        LeaveCriticalSection(&dadosIndicadorCriticalSection);
    return value;
}

INDICADOR_API int    getProtocolo (  )
{
    int value;
    if (indicadorCriticalSectionInicialized)
        EnterCriticalSection(&dadosIndicadorCriticalSection);
    value = indicador.protocolo;
    if (indicadorCriticalSectionInicialized)
        LeaveCriticalSection(&dadosIndicadorCriticalSection);
    return value;
}

INDICADOR_API int   getStatusBateriaIndicador (  )
{
    int value;
    if (indicadorCriticalSectionInicialized)
        EnterCriticalSection(&dadosIndicadorCriticalSection);
    value = indicador.statusBateriaIndicador;
    if (indicadorCriticalSectionInicialized)
        LeaveCriticalSection(&dadosIndicadorCriticalSection);
    return value;
} 

INDICADOR_API int    getNivelBateriaIndicador (  )
{
    int value;
    if (indicadorCriticalSectionInicialized)
        EnterCriticalSection(&dadosIndicadorCriticalSection);
    value = indicador.nivelBateriaIndicador;
    if (indicadorCriticalSectionInicialized)
        LeaveCriticalSection(&dadosIndicadorCriticalSection);
    return value;
}

INDICADOR_API int    getNivelBateriaTransmissorTx1 (  )
{
    int value;
    if (indicadorCriticalSectionInicialized)
        EnterCriticalSection(&dadosIndicadorCriticalSection);
    value = indicador.nivelBateriaTransmissorTx1;
    if (indicadorCriticalSectionInicialized)
        LeaveCriticalSection(&dadosIndicadorCriticalSection);
    return value;
}

INDICADOR_API int    getNivelBateriaTransmissorTx2 (  )
{
    int value;
    if (indicadorCriticalSectionInicialized)
        EnterCriticalSection(&dadosIndicadorCriticalSection);
    value = indicador.nivelBateriaTransmissorTx2;
    if (indicadorCriticalSectionInicialized)
        LeaveCriticalSection(&dadosIndicadorCriticalSection);
    return value;
}

INDICADOR_API const char*  getCodigoProduto (  )
{
    static char value[CODIGO_PRODUTO_LEN+1];
    memset(value, '\0', CODIGO_PRODUTO_LEN+1);
    if (indicadorCriticalSectionInicialized)
        EnterCriticalSection(&dadosIndicadorCriticalSection);
    strncpy(value, indicador.codigoProduto, CODIGO_PRODUTO_LEN+1);
    value[CODIGO_PRODUTO_LEN] = '\0';
    if (indicadorCriticalSectionInicialized)
        LeaveCriticalSection(&dadosIndicadorCriticalSection);
    return value;
}

INDICADOR_API const char*  getData (  )
{
    static char value[DATA_LEN+1];
    memset(value, '\0', DATA_LEN+1);
    if (indicadorCriticalSectionInicialized)
        EnterCriticalSection(&dadosIndicadorCriticalSection);
    strncpy(value, indicador.data, DATA_LEN+1);
    value[DATA_LEN] = '\0';
    if (indicadorCriticalSectionInicialized)
        LeaveCriticalSection(&dadosIndicadorCriticalSection);
    return value;
}

INDICADOR_API const char*  getHora (  )
{
    static char value[HORA_LEN+1];
    memset(value, '\0', HORA_LEN+1);
    if (indicadorCriticalSectionInicialized)
        EnterCriticalSection(&dadosIndicadorCriticalSection);
    strncpy(value, indicador.hora, HORA_LEN+1);
    value[HORA_LEN] = '\0';
    if (indicadorCriticalSectionInicialized)
        LeaveCriticalSection(&dadosIndicadorCriticalSection);
    return value;
}

void setDadosIndicador ( dadosIndicador value ) 
{
    if (indicadorCriticalSectionInicialized)
        EnterCriticalSection(&dadosIndicadorCriticalSection);
    
    indicador = value;

    if (indicadorCriticalSectionInicialized)
        LeaveCriticalSection(&dadosIndicadorCriticalSection);
}

INDICADOR_API dadosIndicador getTodosDadosIndicador (  )
{
    dadosIndicador value;
    if (indicadorCriticalSectionInicialized)
        EnterCriticalSection(&dadosIndicadorCriticalSection);
    
    value = indicador;

    if (indicadorCriticalSectionInicialized)
        LeaveCriticalSection(&dadosIndicadorCriticalSection);

    return value;
}

// --------------------- RESPONSE ---------------------

static int  getZeroResponse (  )
{
    int value;
    if (indicadorCriticalSectionInicialized)
        EnterCriticalSection(&zeroResponseCriticalSection);
    value = zeroResponse;
    if (indicadorCriticalSectionInicialized)
        LeaveCriticalSection(&zeroResponseCriticalSection);
    return value;
}
static void setZeroResponse ( int value )
{
    if (indicadorCriticalSectionInicialized)
        EnterCriticalSection(&zeroResponseCriticalSection);
    zeroResponse = value;
    if (indicadorCriticalSectionInicialized)
        LeaveCriticalSection(&zeroResponseCriticalSection);
}

static int  getTaraResponse (  )
{
    int value;
    if (indicadorCriticalSectionInicialized)
        EnterCriticalSection(&taraResponseCriticalSection);
    value = taraResponse;
    if (indicadorCriticalSectionInicialized)
        LeaveCriticalSection(&taraResponseCriticalSection);
    return value;
}
static void setTaraResponse ( int value )
{
    if (indicadorCriticalSectionInicialized)
        EnterCriticalSection(&taraResponseCriticalSection);
    taraResponse = value;
    if (indicadorCriticalSectionInicialized)
        LeaveCriticalSection(&taraResponseCriticalSection);
}

static int  getImprResponse (  )
{
    int value;
    if (indicadorCriticalSectionInicialized)
        EnterCriticalSection(&imprResponseCriticalSection);
    value = imprResponse;
    if (indicadorCriticalSectionInicialized)
        LeaveCriticalSection(&imprResponseCriticalSection);
    return value;
}
static void setImprResponse ( int value )
{
    if (indicadorCriticalSectionInicialized)
        EnterCriticalSection(&imprResponseCriticalSection);
    imprResponse = value;
    if (indicadorCriticalSectionInicialized)
        LeaveCriticalSection(&imprResponseCriticalSection);
}

static int  getRelatorioResponse (  )
{
    int value;
    if (indicadorCriticalSectionInicialized)
        EnterCriticalSection(&relatorioResponseCriticalSection);
    value = relatorioResponse;
    if (indicadorCriticalSectionInicialized)
        LeaveCriticalSection(&relatorioResponseCriticalSection);
    return value;
}
static void setRelatorioResponse ( int value, int prioridade )
{
    if (indicadorCriticalSectionInicialized)
        EnterCriticalSection(&relatorioResponseCriticalSection);

    if (prioridade == 0 && (relatorioResponse == RESPONDING || relatorioResponse == WAITING_RESPONSE))
        relatorioResponse = value;
    else if (prioridade == 1)
        relatorioResponse = value;
    
    if (indicadorCriticalSectionInicialized)
        LeaveCriticalSection(&relatorioResponseCriticalSection);
}

static int  getConfigResponse (  )
{
    int value;
    if (indicadorCriticalSectionInicialized)
        EnterCriticalSection(&configResponseCriticalSection);
    value = configResponse;
    if (indicadorCriticalSectionInicialized)
        LeaveCriticalSection(&configResponseCriticalSection);
    return value;
}
static void setConfigResponse ( int value )
{
    if (indicadorCriticalSectionInicialized)
        EnterCriticalSection(&configResponseCriticalSection);
    configResponse = value;
    if (indicadorCriticalSectionInicialized)
        LeaveCriticalSection(&configResponseCriticalSection);
}

static int  getProdutoResponse (  )
{
    int value;
    if (indicadorCriticalSectionInicialized)
        EnterCriticalSection(&produtoResponseCriticalSection);
    value = produtoResponse;
    if (indicadorCriticalSectionInicialized)
        LeaveCriticalSection(&produtoResponseCriticalSection);
    return value;
}
static void setProdutoResponse ( int value, int prioridade )
{
    if (indicadorCriticalSectionInicialized)
        EnterCriticalSection(&produtoResponseCriticalSection);

    if (prioridade == 0 && (produtoResponse == RESPONDING || produtoResponse == WAITING_RESPONSE))
        produtoResponse = value;
    else if (prioridade == 1)
        produtoResponse = value;

    if (indicadorCriticalSectionInicialized)
        LeaveCriticalSection(&produtoResponseCriticalSection);
}

static int  getIsClosing (  )
{
    int value;
    if (indicadorCriticalSectionInicialized)
        EnterCriticalSection(&isClosingCriticalSection);
    value = isClosing;
    if (indicadorCriticalSectionInicialized)
        LeaveCriticalSection(&isClosingCriticalSection);
    return value;
}
static void setIsClosing ( int value )
{
    if (indicadorCriticalSectionInicialized)
        EnterCriticalSection(&isClosingCriticalSection);
    isClosing = value;
    if (indicadorCriticalSectionInicialized)
        LeaveCriticalSection(&isClosingCriticalSection);
}