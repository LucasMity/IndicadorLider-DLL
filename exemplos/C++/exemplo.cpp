#include <iostream>
#include <windows.h>
#include <string.h>
#include <iomanip>
using namespace std;

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
typedef int (*isOpenFunction)();
typedef int (*closeFunction)();
typedef double (*getPesoLiquidoFunction)();
typedef double (*getPesoBrutoFunction)();
typedef double (*getTaraFunction)();
typedef int (*getPrecisaoDecimalPesoFunction)();
typedef int (*getStatusBalancaFunction)();
typedef int (*isModoContadoraPecasFunction)();
typedef int (*getStatusBateriaIndicadorFunction)();
typedef int (*getNivelBateriaIndicadorFunction)();
typedef int (*getProtocoloFunction)();
typedef int (*getNivelBateriaTransmissorTx1Function)();
typedef int (*getNivelBateriaTransmissorTx2Function)();
typedef int (*getUnidadeMedidaFunction)();
typedef int (*zerarFunction)(  );
typedef int (*tararFunction)(  );
typedef int (*tararManualFunction)( int tara );
typedef char* (*getTabelaProdutoFunction)(  );

int check(int response) {
    if (response > 0) {
        cout << "SUCESSO" << endl;
        return true;
    } else {
        cout << "ERRO" << endl;
        return false;
    }
}

int main () {
    int opc = 1;
    int taraValor, precisao;
    char* tabela;

    HINSTANCE hDll = LoadLibrary("../../lib/x86/indicadorLider.dll"); // Carrega a DLL
    if (hDll != NULL) { // Verifica se a DLL foi carregada corretamente
        // Carrega cada uma das funcoes
        openSerialFunction openSerial = reinterpret_cast<openSerialFunction>(GetProcAddress(hDll, "openSerial"));
        openTcpFunction openTcp = reinterpret_cast<openTcpFunction>(GetProcAddress(hDll, "openTcp"));
        isOpenFunction isOpen = reinterpret_cast<isOpenFunction>(GetProcAddress(hDll, "isOpen"));
        closeFunction close = reinterpret_cast<closeFunction>(GetProcAddress(hDll, "close"));
        getPesoLiquidoFunction getPesoLiquido = reinterpret_cast<getPesoLiquidoFunction>(GetProcAddress(hDll, "getPesoLiquido"));
        getPesoBrutoFunction getPesoBruto = reinterpret_cast<getPesoBrutoFunction>(GetProcAddress(hDll, "getPesoBruto"));
        getTaraFunction getTara = reinterpret_cast<getTaraFunction>(GetProcAddress(hDll, "getTara"));
        getPrecisaoDecimalPesoFunction getPrecisaoDecimalPeso = reinterpret_cast<getPrecisaoDecimalPesoFunction>(GetProcAddress(hDll, "getPrecisaoDecimalPeso"));
        getStatusBalancaFunction getStatusBalanca = reinterpret_cast<getStatusBalancaFunction>(GetProcAddress(hDll, "getStatusBalanca"));
        isModoContadoraPecasFunction isModoContadoraPecas = reinterpret_cast<isModoContadoraPecasFunction>(GetProcAddress(hDll, "isModoContadoraDePecas"));
        getUnidadeMedidaFunction getUnidadeMedida = reinterpret_cast<getUnidadeMedidaFunction>(GetProcAddress(hDll, "getUnidadeMedida"));
        zerarFunction zerar = reinterpret_cast<zerarFunction>(GetProcAddress(hDll, "zerar"));
        tararFunction tarar = reinterpret_cast<tararFunction>(GetProcAddress(hDll, "tarar"));
        tararManualFunction tararManual = reinterpret_cast<tararManualFunction>(GetProcAddress(hDll, "tararManual"));
        getTabelaProdutoFunction getTabelaProduto = reinterpret_cast<getTabelaProdutoFunction>(GetProcAddress(hDll, "getTabelaProduto"));
        getProtocoloFunction getProtocolo = reinterpret_cast<getProtocoloFunction>(GetProcAddress(hDll, "getProtocolo"));

        if (openSerial && openTcp && isOpen && close && getPesoLiquido && getPesoBruto && getTara && getPrecisaoDecimalPeso && getStatusBalanca && isModoContadoraPecas &&getUnidadeMedida && zerar && tarar && tararManual && getTabelaProduto && getProtocolo) { // Verifica se todas as funcoes foram carregadas corretamente

            // openSerial("COM4", 9600, 8, 1, 0); // Abre comunicacao Serial
            openTcp("10.20.0.200", 502); // Abre comunicacao TCP
            
            while(opc != 0) {
                cout << "1 - ZERAR\n2 - MOSTRAR VALORES\n3 - TARAR\n4 - TARAR MANUAL\n5 - MOSTRAR TABELA PRODUTOS\n0 - SAIR\nOPC: ";
                cin >> opc;
                switch(opc)
                {
                    case 1:
                        check(zerar());
                        break;
                    case 2:
                        precisao = getPrecisaoDecimalPeso();
                        if (getStatusBalanca() >= 0) {
                            if (isModoContadoraPecas() == 1) {
                                cout << fixed << setprecision(0) << "Pecas       : " << getPesoLiquido() << endl;
                            } else {
                                cout << fixed << setprecision(precisao) << "Peso Liquido: " << getPesoLiquido() << endl;
                            }
                            cout << fixed << setprecision(precisao) << "Peso Bruto  : " << getPesoBruto() << endl;
                            cout << fixed << setprecision(precisao) << "Tara        : " << getTara() << endl;
                        }
                        cout << "Status      : ";
                        switch(getStatusBalanca())
                        {
                            case STATUS_BALANCA_PESO_EXCESSO_NEGATIVO:
                                cout << "Excesso de Peso Negativo" << endl;
                                break;
                            case STATUS_BALANCA_PESO_EXCESSO_POSITIVO:
                                cout << "Excesso de Peso Positivo" << endl;
                                break;
                            case STATUS_BALANCA_ESTAVEL:
                                cout << "Peso Estavel." << endl;
                                break;
                            case STATUS_BALANCA_INSTAVEL:
                                cout << "Peso Instavel." << endl;
                                break;
                            case STATUS_BALANCA_VALOR_PICO:
                                cout << "Valor de Pico." << endl;
                                break;
                            case STATUS_BALANCA_PESO_MEDIO:
                                cout << "Peso Medio." << endl;
                                break;
                            case STATUS_BALANCA_AGUARDANDO:
                                cout << "Aguardando." << endl;
                                break;
                            case STATUS_BALANCA_PERDA_COMUNICACAO:
                                cout << "Perda de Comunicacao." << endl;
                                break;
                            case STATUS_BALANCA_IMPRIME:
                                cout << "Imprime." << endl;
                                break;
                            default:
                                cout << "Sem informacoes." << endl;
                        }

                        cout << "PROTOCOLO LIDER " << getProtocolo() + 1 << endl;
                        cout << "UNIDADE DE MEDIDA: ";
                        switch (getUnidadeMedida()) {
                            case 2:
                                cout << "KG" << endl;
                                break;
                            case 1:
                                cout << "LB" << endl;
                                break;
                            case 0:
                                cout << "KN" << endl;
                                break;
                            default:
                                cout << "Desconhecido" << endl;
                        }
                        break;
                    case 3:
                        check(tarar());
                        break;
                    case 4:
                        cout << "Tara: ";
                        cin >> taraValor;
                        check(tararManual(taraValor));
                        break;
                    case 5:
                        tabela = getTabelaProduto();
                        cout << "Tabela de Produto:" << endl;
                        cout << tabela << endl;
                        break;
                }
            }
            close(); // Fecha comunicacao
        } else {
            cout << "Algumas funcoes nao foram encontradas na DLL." << endl;
        }   
        FreeLibrary(hDll); // Libera DLL
    } else {
        cout << "Nao foi possivel carregar a DLL." << endl;
    }     
    return 0;
}