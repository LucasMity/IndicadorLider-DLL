#include <stdio.h>
#include <windows.h>

#define CODIGO_PRODUTO_LEN 15
#define DATA_LEN 11
#define HORA_LEN 9

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
    char     codigoProduto[CODIGO_PRODUTO_LEN];
    char     data[DATA_LEN];
    char     hora[HORA_LEN];
} dadosIndicador;

// STATUS BALANCA
enum ind_status_balanca {
    STATUS_BALANCA_PESO_EXCESSO_POSITIVO    =   -3,
    STATUS_BALANCA_PESO_EXCESSO_NEGATIVO    =   -2,
    STATUS_BALANCA_NULO                     =   -1,
    STATUS_BALANCA_ESTAVEL                  =    0,
    STATUS_BALANCA_INSTAVEL                 =    1,
    STATUS_BALANCA_VALOR_PICO               =    2,
    STATUS_BALANCA_PESO_MEDIO               =    3,
    STATUS_BALANCA_AGUARDANDO               =    4,
    STATUS_BALANCA_PERDA_COMUNICACAO        =    5,
    STATUS_BALANCA_IMPRIME                  =    6,
};

typedef int (*openSerialFunction)( const char* port, int baudRate, int dataBits, int stopBits, int parity );
typedef int (*openTcpFunction)( const char* ip, int port );
typedef int (*isOpenFunction)(  );
typedef int (*closeFunction)(  );

typedef double (*getPesoLiquidoFunction)(  );
typedef double (*getPesoBrutoFunction)(  );
typedef double (*getTaraFunction)(  );
typedef int (*getPrecisaoDecimalPesoFunction)(  );
typedef int (*getStatusBalancaFunction)(  );
typedef int (*getUnidadeMedidaFunction)(  );
typedef int (*isModoContadoraDePecasFunction)(  );
typedef int (*getProtocoloFunction)(  );
typedef dadosIndicador (*getTodosDadosIndicadorFunction)(  );

typedef int (*zerarFunction)(  );
typedef int (*tararFunction)(  );
typedef int (*tararManualFunction)( int tara );
typedef char* (*getConfiguracaoFunction)( int funcao );
typedef char* (*setConfiguracaoFunction)( int funcao, char* input );
typedef int (*saveConfiguracaoFunction)(  );
typedef char* (*getTabelaProdutosFunction)(  );
typedef int (*ERFprodutoFunction)(  );
typedef int (*cadastrarProdutoFunction)( char* codigo, char* descricao );
typedef int (*deletarProdutoFunction)( char* codigo );
typedef char* (*gerarRelatorioFunction)( int tipo, char* input );
typedef int (*deletarTabelaProdutosFunction)(  );
typedef int (*perguntarPesoFunction)(  );
typedef int (*imprimirMediaFunction)(  );
typedef char* (*getDataFunction)(  );
typedef char* (*getHoraFunction)(  );

int check(int response) {
    if (response > 0) {
        printf("SUCESSO\n");
        return TRUE;
    } else {
        printf("ERRO\n");
        return FALSE;
    }
}

int main () {
    char input[1000], *output;
    char codigo[50], produto[50];
    int func;
    int imp, usuario;
    int opc = 1, precisao;
    dadosIndicador dados;

    // Verificando se cada um dos valores é diferente de NULL
    HMODULE hDll = LoadLibrary("../../lib/x86/indicadorLider.dll"); // Carrega DLL
    if (hDll != NULL) { // Verifica se a DLL foi carregada corretamente
        // Carrega as funcoes da DLL que serao utilizadas
        openSerialFunction openSerial = (openSerialFunction)GetProcAddress(hDll, "openSerial");
        openTcpFunction openTcp = (openTcpFunction)GetProcAddress(hDll, "openTcp");
        isOpenFunction isOpen = (isOpenFunction)GetProcAddress(hDll, "isOpen");
        closeFunction close = (closeFunction)GetProcAddress(hDll, "close");

        getPesoLiquidoFunction getPesoLiquido = (getPesoLiquidoFunction)GetProcAddress(hDll, "getPesoLiquido");
        getPesoBrutoFunction getPesoBruto = (getPesoBrutoFunction)GetProcAddress(hDll, "getPesoBruto");
        getTaraFunction getTara = (getTaraFunction)GetProcAddress(hDll, "getTara");
        getPrecisaoDecimalPesoFunction getPrecisaoDecimalPeso = (getPrecisaoDecimalPesoFunction)GetProcAddress(hDll, "getPrecisaoDecimalPeso"); 
        getStatusBalancaFunction getStatusBalanca = (getStatusBalancaFunction)GetProcAddress(hDll, "getStatusBalanca");
        getUnidadeMedidaFunction getUnidadeMedida = (getUnidadeMedidaFunction)GetProcAddress(hDll, "getUnidadeMedida");
        isModoContadoraDePecasFunction isModoContadoraDePecas = (isModoContadoraDePecasFunction)GetProcAddress(hDll, "isModoContadoraDePecas");
        getProtocoloFunction getProtocolo = (getProtocoloFunction)GetProcAddress(hDll, "getProtocolo");
        getTodosDadosIndicadorFunction getTodosDadosIndicador = (getTodosDadosIndicadorFunction)GetProcAddress(hDll, "getTodosDadosIndicador");

        zerarFunction zerar = (zerarFunction)GetProcAddress(hDll, "zerar");
        tararFunction tarar = (tararFunction)GetProcAddress(hDll, "tarar");
        tararManualFunction tararManual = (tararManualFunction)GetProcAddress(hDll, "tararManual");
        getConfiguracaoFunction getConfiguracao = (getConfiguracaoFunction)GetProcAddress(hDll, "getConfiguracaoIndicador");
        setConfiguracaoFunction setConfiguracao = (setConfiguracaoFunction)GetProcAddress(hDll, "setConfiguracaoIndicador");
        saveConfiguracaoFunction saveConfiguracao = (saveConfiguracaoFunction)GetProcAddress(hDll, "saveConfiguracaoIndicador");
        getTabelaProdutosFunction getTabelaProdutos = (getTabelaProdutosFunction)GetProcAddress(hDll, "getTabelaProduto");
        ERFprodutoFunction ERFproduto = (ERFprodutoFunction)GetProcAddress(hDll, "ERF_tabelaProduto");
        cadastrarProdutoFunction cadastrarProduto = (cadastrarProdutoFunction)GetProcAddress(hDll, "cadastrarProduto");
        deletarProdutoFunction deletarProduto = (deletarProdutoFunction)GetProcAddress(hDll, "deletarProduto");
        gerarRelatorioFunction gerarRelatorio = (gerarRelatorioFunction)GetProcAddress(hDll, "gerarRelatorio");
        deletarTabelaProdutosFunction deletarTabelaProdutos = (deletarTabelaProdutosFunction)GetProcAddress(hDll, "deletarTabelaProduto");
        perguntarPesoFunction perguntarPeso = (perguntarPesoFunction)GetProcAddress(hDll, "perguntarPeso");
        imprimirMediaFunction imprimirCalculaMedia = (imprimirMediaFunction)GetProcAddress(hDll, "imprimir_calcularMedia");
        getDataFunction getData = (getDataFunction)GetProcAddress(hDll, "getData");
        getHoraFunction getHora = (getHoraFunction)GetProcAddress(hDll, "getHora");

        if (openSerial && openTcp && isOpen && close && zerar && tarar && tararManual && perguntarPeso && imprimirCalculaMedia
        && getPesoLiquido && getPesoBruto && getTara && getPrecisaoDecimalPeso && getStatusBalanca && getUnidadeMedida && isModoContadoraDePecas && getProtocolo && getData && getHora && getTodosDadosIndicador
        && getConfiguracao && setConfiguracao && saveConfiguracao 
        && getTabelaProdutos && cadastrarProduto && ERFproduto && deletarProduto && gerarRelatorio && deletarTabelaProdutos)
        {   // Verifica se todas as funcoes foram carregadas corretamente
            // check(openSerial("COM4", 9600, 8, 1, 0)); // Abre comunicacao Serial
            check(openTcp("10.20.0.200", 502)); // Abre comunicacao TCP
            
            while(opc != 0) {
                printf("1 - MOSTRAR VALORES\n2 - ZERAR\n3 - TARAR\n4 - TARAR MANUAL\n5 - CONSULTAR CONFIG\n6 - ALTERAR CONFIG\n7 - CADASTRAR PRODUTO\n8 - LISTAR PRODUTOS\n9 - DELETAR PRODUTO\n10 - GERAR RELATORIO\n11 - DELETAR LISTA DE PRODUTOS\n12 - PERGUNTAR PESO\n13 - IMPRIMIR/CALCULAR MEDIA\n0 - SAIR\nOPC: ");
                scanf("%d", &opc);
                switch(opc) {
                    case 1:
                        precisao = getPrecisaoDecimalPeso();
                        if (getStatusBalanca() >= 0)
                        {
                            if (isModoContadoraDePecas())
                                printf("Pecas       : %.*lf\n", 0, getPesoLiquido());
                            else
                                printf("Peso Liquido: %.*lf\n", precisao, getPesoLiquido());
                            printf("Peso Bruto  : %.*lf\n", precisao, getPesoBruto());
                            printf("Tara        : %.*lf\n", precisao, getTara());
                        }
                        printf("Status      : ");
                        switch (getStatusBalanca())
                        {
                            case STATUS_BALANCA_PESO_EXCESSO_NEGATIVO:
                                printf("Excesso de Peso Negativo\n");
                                break;
                            case STATUS_BALANCA_PESO_EXCESSO_POSITIVO:
                                printf("Excesso de Peso Positivo\n");
                                break;
                            case STATUS_BALANCA_NULO:
                                printf("Sem informacoes.\n");
                                break;
                            case STATUS_BALANCA_ESTAVEL:
                                printf("Peso Estavel.\n");
                                break;
                            case STATUS_BALANCA_INSTAVEL:
                                printf("Peso Instavel.\n");
                                break;
                            case STATUS_BALANCA_VALOR_PICO:
                                printf("Valor de Pico.\n");
                                break;
                            case STATUS_BALANCA_PESO_MEDIO:
                                printf("Peso Medio.\n");
                                break;
                            case STATUS_BALANCA_AGUARDANDO:
                                printf("Aguardando.\n");
                                break;
                            case STATUS_BALANCA_PERDA_COMUNICACAO:
                                printf("Perda de Comunicacao.\n");
                                break;
                            case STATUS_BALANCA_IMPRIME:
                                printf("Imprime.\n");
                                break;
                        }
                        printf("PROTOCOLO LIDER %d\n", getProtocolo() + 1);
                        printf("UNIDADE DE MEDIDA: ");
                        switch (getUnidadeMedida()) {
                            case 2:
                                printf("KG\n");
                                break;
                            case 1:
                                printf("LB\n");
                                break;
                            case 0:
                                printf("KN\n");
                                break;
                            default:
                                printf("Desconhecido\n");
                        }
                        output = getData();
                        printf("Data: %s\n", output);
                        output = getHora();
                        printf("Hora %s\n", output);
                        break;
                    case 2:
                        check(zerar());
                        break;
                    case 3:
                        check(tarar());
                        break;
                    case 4:
                        int taraValor;
                        printf("TARA: ");
                        scanf("%d", &taraValor);
                        check(tararManual(taraValor));
                        break;
                    case 5:
                        printf("FUNCAO: ");
                        scanf("%d", &func);
                        output = getConfiguracao(func);
                        printf("F%d = %s\n", func, output);
                        break;
                    case 6:
                        printf("FUNCAO: ");
                        scanf("%d", &func);
                        printf("VALOR: ");
                        scanf("%s", &input);
                        output = setConfiguracao(func, input);
                        printf("F%d = %s\n", func, output);
                        check(saveConfiguracao());
                        break;
                    case 7:
                        printf("CODIGO: ");
                        fflush(stdin);
                        fgets(codigo, 14, stdin);
                        fflush(stdin);
                        printf("DESCRICAO: ");
                        fflush(stdin);
                        fgets(produto, 21, stdin);
                        fflush(stdin);
                        if (ERFproduto() > 0)
                            check(cadastrarProduto(codigo, produto));
                        else
                            check(0);
                        break;
                    case 8:
                        output = getTabelaProdutos();
                        printf("%s\n", output);
                        break;
                    case 9:
                        printf("CODIGO: ");
                        fflush(stdin);
                        fgets(codigo, 14, stdin);
                        fflush(stdin);
                        check(deletarProduto(codigo));
                        break;
                    case 10:
                        printf("NUMERO DE IMPRESSOES: ");
                        scanf("%d", &imp);
                        sprintf(input, "%d", imp);
                        output = gerarRelatorio(0, input);
                        printf("RELATORIO:\n%s\n", output);
                        break;
                    case 11:
                        check(deletarTabelaProdutos());
                        break;
                    case 12:
                        check(perguntarPeso());
                        break;
                    case 13:
                        check(imprimirCalculaMedia());
                        break;
                    case 0:
                        printf("Saindo...\n");
                        break;
                    default:
                        break;
                }
            }
            check(close()); // Fecha comunicacao
        } else {
            printf("Algumas funcoes nao foram encontradas na DLL.\n");
        }
        FreeLibrary(hDll); // Libera DLL
    } else {
        printf("Nao foi possivel carregar a DLL.\n");
    }
    return 0;
}