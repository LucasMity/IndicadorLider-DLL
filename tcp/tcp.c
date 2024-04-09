#include "tcp.h"

int tcp_open ( const char* ipHost, int port )
{
    char portString[MAX_BUFFER_SIZE];
    sprintf(portString, "%d", port);
    int iResult;
    WSADATA wsaDATA;
    // Initialize Winsock
    iResult = WSAStartup(MAKEWORD(2,2), &wsaDATA);
    if (iResult != 0)
    {
        printf("WSAStartup failed: %d\n", iResult);
        return ERRO;
    }

    struct addrinfo *result = NULL,
                    *ptr = NULL,
                    hints;
    ZeroMemory( &hints, sizeof(hints) );
    hints.ai_family     = AF_INET;
    hints.ai_socktype   = SOCK_STREAM;
    hints.ai_protocol   = IPPROTO_TCP;

    // Resolve the server address and port
    iResult = getaddrinfo(ipHost, portString, &hints, &result);
    if (iResult != 0)
    {
        printf("getaddrinfo failed: %d\n", iResult);
        WSACleanup();
        return ERRO_INVALID_ARG;
    }

    // Attempt to connect to the first address returned by
    // the call to getaddrinfo
    ptr=result;

    // Create a SOCKET for connecting to server
    ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype, 
        ptr->ai_protocol);

    if ( ConnectSocket == INVALID_SOCKET )
    {
        printf("Error at socket(): %ld\n", WSAGetLastError());
        freeaddrinfo(result);
        WSACleanup();
        return ERRO;
    }

    // Connect to server.
    iResult = connect( ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
    if (iResult == SOCKET_ERROR)
    {
        closesocket(ConnectSocket);
        ConnectSocket = INVALID_SOCKET;
    }
    
    // Should really try the next address returned by getaddrinfo
    // if the connect call failed
    // But for this simple example we just free the resources
    // returned by getaddrinfo and print an error message

    freeaddrinfo(result);

    if (ConnectSocket == INVALID_SOCKET)
    {
        printf("Unable to connect to server!\n");
        WSACleanup();
        return ERRO;
    }
   
    DWORD timeout = TCP_TIMEOUT_MSEC;
    int timeoutLen = sizeof(DWORD);

    if (setsockopt(ConnectSocket, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout, timeoutLen) != 0)
    {
        printf("setsockopt SO_RCVTIMEOUT failed\n");
        return ERRO;
    }
 
    if (setsockopt(ConnectSocket, SOL_SOCKET, SO_SNDTIMEO, (char*)&timeout, timeoutLen) != 0)
    {
        printf("setsockopt SO_SNDTIMEO failed\n");
        return ERRO;
    }
   
    return OK;
}

int tcp_isOpen (  )
{
    int resp;
        
    if( ConnectSocket == INVALID_SOCKET )
        resp = ERRO;
    else
        resp = OK;
        
    return resp;
}

int tcp_close   (  )
{
    int iResult;

    iResult = shutdown(ConnectSocket, SD_SEND);
    if (iResult == SOCKET_ERROR)
        printf("shutdown SEND failed: %d\n", WSAGetLastError());
    
    iResult = shutdown(ConnectSocket, SD_RECEIVE);
    if (iResult == SOCKET_ERROR)
        printf("shutdown RECEIVE failed: %d\n", WSAGetLastError());

    iResult = closesocket(ConnectSocket);
    if (iResult == SOCKET_ERROR)
    {
        printf("Close Socket failed: %d\n", WSAGetLastError());
        return ERRO;
    }
    else
    {
        ConnectSocket = INVALID_SOCKET;
        // cleanup
        WSACleanup();
        return OK;
    }
}

char tcp_receive (  )
{
    char c;

    if( recv(ConnectSocket, &c, 1, 0 ) <= 0 ) { c = CHAR_VAZIO; }

    return c;
}

int tcp_send    ( const char* data )
{
    int resp;
        
    if ( send(ConnectSocket, data, (int) strlen(data), 0 ) == SOCKET_ERROR )
    {
        printf("Send failed: %d\n", WSAGetLastError());
        resp = ERRO;
    }
    else
        resp = OK;

    return resp;
}