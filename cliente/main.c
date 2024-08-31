#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment(lib, "ws2_32.lib")

void iniciar_sockets() {
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        printf("Error en la inicialización de Winsock. Código de error: %d\n", WSAGetLastError());
        exit(EXIT_FAILURE);
    }
}

void cerrar_sockets() {
    WSACleanup();
}

int main()
{
     iniciar_sockets();
     int client_socket;

     int salir = 0;

    do{
        client_socket = socket(AF_INET, SOCK_STREAM, 0);
        if (client_socket == INVALID_SOCKET) {
        printf("Error al crear el socket. Código de error: %d\n", WSAGetLastError());
        cerrar_sockets();
        exit(EXIT_FAILURE);
        }

        struct sockaddr_in server_addr;
        server_addr.sin_family = AF_INET;
        server_addr.sin_port = htons(8080);

        if (inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr) <= 0) {
            printf("Error en la dirección IP.\n");
            cerrar_sockets();
            exit(EXIT_FAILURE);
        }

        if (connect(client_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
            printf("Error al conectar. Código de error: %d\n", WSAGetLastError());
            cerrar_sockets();
            exit(EXIT_FAILURE);
        }

        //obtencion del menu
        char buffer[1024] = {0};
        recv(client_socket, buffer, sizeof(buffer), 0);
        printf("%s\n", buffer);

         //Envío de la opción
        int opcion;
        scanf("%d", &opcion);
        opcion = htonl(opcion); // Convertir a big-endian
        send(client_socket, &opcion, sizeof(opcion), 0);

        // Recepción del mensaje ("adios", "Opcion incorrecta" o "dime el largo que te gustaria que tenga")
        char mensaje[1024] = {0};
        recv(client_socket, mensaje, sizeof(mensaje), 0); // Usar buffer en lugar de un puntero no inicializado
        printf("%s\n", mensaje);

        if (strcmp(mensaje, "Adios") == 0) {
            salir = 1;
        } else if (strcmp(mensaje, "Opcion incorrecta") == 0){
            printf("intentelo nuevamente\n");
        } else {
            // Envío del largo
            int largo;
            scanf("%d", &largo);
            largo = htonl(largo); // Convertir a big-endian
            send(client_socket, &largo, sizeof(largo), 0); // Pasar la dirección de largo

            char resultado[1024] = {0};
            // Recepción de la información
            recv(client_socket, resultado, sizeof(resultado), 0);
            printf("%s\n", resultado);
        }

        closesocket(client_socket);
    }while(!salir);

    cerrar_sockets();
    return 0;
}
