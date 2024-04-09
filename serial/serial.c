#include "serial.h"

int serial_open ( const char* portName, int baudRate, int dataBits, int stopBits, int parity )
{

    serialPort = CreateFile(portName,                           // address of name of the communications device
                            GENERIC_READ | GENERIC_WRITE,       // access mode
                            0,                                  // share mode
                            NULL,                               // address of security descriptor
                            OPEN_EXISTING,                      // how to create
                            FILE_FLAG_OVERLAPPED,               // file attributes
                            NULL );                             // handle of file with attributes to copy

    if (serialPort == INVALID_HANDLE_VALUE)
    {
        return ERRO_INVALID_ARG;
    }
    // Configurar parametros da porta serial
    DCB dcbSerialParams;
    SecureZeroMemory(&dcbSerialParams, sizeof(DCB));
    dcbSerialParams.DCBlength = sizeof(DCB);

    if (!GetCommState(serialPort, &dcbSerialParams))
    {
        CloseHandle(serialPort);
        serialPort = INVALID_HANDLE_VALUE;
        return ERRO;
    }

    dcbSerialParams.BaudRate = baudRate;
    dcbSerialParams.ByteSize = dataBits;
    if (stopBits == 1)
        dcbSerialParams.StopBits = ONESTOPBIT;
    else if (stopBits == 2)
        dcbSerialParams.StopBits = TWOSTOPBITS;
    else
    {
        CloseHandle(serialPort);
        serialPort = INVALID_HANDLE_VALUE;
        return ERRO_INVALID_ARG;
    }
    dcbSerialParams.Parity   = parity;

    if (!SetCommState(serialPort, &dcbSerialParams))
    {
        CloseHandle(serialPort);
        serialPort = INVALID_HANDLE_VALUE;
        return ERRO_INVALID_ARG;
    }

    // Configurar timeouts
    COMMTIMEOUTS timeouts;
    SecureZeroMemory(&timeouts, sizeof(COMMTIMEOUTS));
    timeouts.ReadIntervalTimeout = MAXWORD;
    timeouts.ReadTotalTimeoutConstant = SERIAL_TIMEOUT;
    timeouts.ReadTotalTimeoutMultiplier = 1;
    timeouts.WriteTotalTimeoutConstant = SERIAL_TIMEOUT;
    timeouts.WriteTotalTimeoutMultiplier = 1;

    if (!SetCommTimeouts(serialPort, &timeouts))
    {
        CloseHandle(serialPort);
        serialPort = INVALID_HANDLE_VALUE;
        return ERRO;
    }

    return OK;
}

int serial_setParametrosConfig( int baudRate, int dataBits, int stopBits, int parity )
{
    // Configurar parametros da porta serial
    DCB dcbSerialParams;
    SecureZeroMemory(&dcbSerialParams, sizeof(DCB));
    dcbSerialParams.DCBlength = sizeof(DCB);

    if (!GetCommState(serialPort, &dcbSerialParams))
    {
        return ERRO;
    }

    if (baudRate != -1)
        dcbSerialParams.BaudRate = baudRate;
    if (dataBits != -1)
        dcbSerialParams.ByteSize = dataBits;
    if (stopBits != -1)
    {
        if (stopBits == 1)
            dcbSerialParams.StopBits = ONESTOPBIT;
        else if (stopBits == 2)
            dcbSerialParams.StopBits = TWOSTOPBITS;
        else
        {
            return ERRO_INVALID_ARG;
        }
    }
    if (parity != -1)
        dcbSerialParams.Parity   = parity;

    if (!SetCommState(serialPort, &dcbSerialParams))
    {
        return ERRO_INVALID_ARG;
    }
    return OK;
}

int serial_isOpen (  )
{    
    if ( serialPort == INVALID_HANDLE_VALUE )
        return ERRO;
    else
        return OK;
}

int serial_close (  )
{
    if ( CloseHandle(serialPort) )
    {
        serialPort = INVALID_HANDLE_VALUE;
        return OK;
    }
    else { return ERRO; }
}

char serial_receive ( )
{
    char c = CHAR_VAZIO;
    // ----------- LEITURA ASSINCRONA --------------- FULL DUPLEX
    DWORD dwRead, dwRes;
    BOOL fWaitingOnRead = FALSE;

    // Create the overlapped event. Must be closed before exiting
    // to avoid a handle leak.
    osReader.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

    if (osReader.hEvent == NULL) // Error creating overlapped event; abort
        return c;

    while(1) {
        if (!fWaitingOnRead) // Se nao ha espera para leitura
        { 
            if (ReadFile(serialPort, &c, 1, &dwRead, &osReader)) // Realiza leitura
            {
                break; // Leitura foi imediata
            } else { // Caso tenha dado erro
                if (GetLastError() == ERROR_IO_PENDING) // leitura pendente
                    fWaitingOnRead = TRUE; // Coloca em modo de espera
                else 
                    break; // Houve um erro na comunicacao
            }
        }
        else // Se ha espera
        { 
            //dwRes = WaitForSingleObject(osReader.hEvent, SERIAL_TIMEOUT); // Aguarda
            dwRes = WaitForSingleObject(osReader.hEvent, INFINITE); // Aguarda
            // Fim da espera
            if (dwRes == WAIT_OBJECT_0) // Caso a espera tenha dado resultado
            {
                if(GetOverlappedResult(serialPort, &osReader, &dwRead, FALSE))// Verifica se a leitura foi um sucesso
                    break;
                else // Erro na comunicacao
                {
                    c = CHAR_VAZIO;
                    break;
                }
            }
            else // Outro ERRO
            {
                // Error in the WaitForSingleObject; abort.
                // This indicates a problem with the OVERLAPPED structure's
                // event handle.
                break;
            }
        }
    }
    CloseHandle(osReader.hEvent);
    return c;
    // ----------- FIM LEITURA ASSINCRONA ---------------

    // ----------- LEITURA SINCRONA --------------- HALF DUPLEX
    // DWORD bytesRead = 0;
    // char c = CHAR_VAZIO;

    // if (ReadFile(serialPort, &c, 1, &bytesRead, NULL)) 
    // { 
    //     return c;
    // } 
    // else { return CHAR_VAZIO; }   
    // ----------- FIM LEITURA SINCRONA ---------------
}

int serial_send ( const char* data )
{
    // ----------- ESCRITA ASSINCRONA --------------- FULL DUPLEX
    DWORD dwWritten;
    DWORD dataSize = (DWORD)strlen(data);
    int fRes;

    // Create this writes OVERLAPPED structure hEvent.
    osWrite.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

    if (osWrite.hEvent == NULL) // Error creating overlapped evend handle
        return ERRO; // abort

    // Realiza a escrita
    if (WriteFile(serialPort, data, dataSize, &dwWritten, &osWrite))
    { // Escrita feita imediatamente
        fRes = OK;
    } 
    else
    { // Escrita pendente
        if (GetLastError() != ERROR_IO_PENDING)
        {// WriteFile failed, but it isn't delayed. Report error and abort.
            fRes = ERRO;
        } 
        else
        {// Write is pending.
            if (GetOverlappedResult(serialPort, &osWrite, &dwWritten, TRUE)) // Espera ate a operacao terminar
                fRes = OK; // SUCESSO
            else
                fRes = ERRO; // FALHA
        }
    }

    CloseHandle(osWrite.hEvent);
    return fRes;
    // ----------- FIM ESCRITA ASSINCRONA ------------

    // ----------- ESCRITA SINCRONA --------------- HALF DUPLEX
    // DWORD bytesWritten = 0;
    // DWORD size = (DWORD)strlen(data);

    // if(WriteFile(serialPort, data, size, &bytesWritten, NULL))
    // {
    //     return (bytesWritten == size) ? OK : ERRO;
    // }
    // else { return ERRO; }   
    // ----------- FIM ESCRITA SINCRONA ------------
}