#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment(lib, "ws2_32.lib")

int generarBoleanoAleatorio() {
    return rand() % 2;
}

char generadorNumeroLetraAleatorio(int opcion) {
    // Declarar los arrays de números y letras
    char numeros[] = {'1','2','3','4','5','6','7','8','9','0'};
    char letras[] = {'a','b', 'c', 'd','e','f', 'g', 'h','i', 'j', 'k', 'l', 'm', 'n','o', 'p', 'q', 'r', 's', 't','u', 'v', 'w', 'x', 'y', 'z'};
    char *conjunto;
    int largo;

    if(opcion == 1) {
        conjunto = numeros;
        largo = sizeof(numeros) / sizeof(numeros[0]); // Obtener la longitud del array de números
    } else {
        conjunto = letras;
        largo = sizeof(letras) / sizeof(letras[0]); // Obtener la longitud del array de letras
    }
    // Generar un índice aleatorio
    int randomIndex = rand() % largo;
    // Retornar el carácter aleatorio
    return conjunto[randomIndex];
}

char* generadorUsuario(int longitud){
    if(longitud>15||longitud<5){
        return "la longitud es invalida, debe estar entre 5 y 15 caracteres";
    }
    char *mensaje = (char *)malloc((longitud + 1) * sizeof(char));
    char consonantes[] = {'b', 'c', 'd', 'f', 'g', 'h', 'j', 'k', 'l', 'm', 'n', 'p', 'q', 'r', 's', 't', 'v', 'w', 'x', 'y', 'z'};
    char vocales[] = {'a', 'e', 'i', 'o', 'u'};
    int random = generarBoleanoAleatorio();
    int vocalIndex = 0;
    mensaje[0] = '\0';
    if(random == 0){
        for(int i = 0;i<=longitud;i++){
            if(i%2==0){
                mensaje[i] = vocales[vocalIndex];
                vocalIndex++;
                if(vocalIndex>4){
                    vocalIndex = 0;
                }
            }else{
                mensaje[i] = consonantes[i];
            }
        }
    }else{
        for(int i = 0;i<=longitud;i++){
            if(i%2==0){
                mensaje[i] = consonantes[i];
            }else{
                mensaje[i] = vocales[vocalIndex];
                vocalIndex++;
                if(vocalIndex>4){
                    vocalIndex = 0;
                }
            }
        }
    }
    mensaje[longitud] = '\0';  // Termina la cadena
    return mensaje;
}

char* generadorPassword(int longitud){
    if(longitud<8 || longitud>50){
        return "la longitud es incorecta , debe estar entre 8 y 50 caracteres";
    }
    char *password = (char *)malloc((longitud + 1) * sizeof(char));
    password[0] = '\0';
    for(int i = 0 ; i<= longitud; i++){
        int alfa = generarBoleanoAleatorio();// para saber si sera letre o numero
        if(alfa == 0){ // si es letra
            char letra = generadorNumeroLetraAleatorio(0); //obtnemos una letra random
            int minuscula = generarBoleanoAleatorio(); // con esto decidimos si sera mayuscula o minuscula
            if(minuscula == 0){//si es minuscula
                password[i] = letra;
            }else{//si es mayuscula
                password[i] = toupper((unsigned char) letra);
            }
        }else{//si es un numero
            password[i] = generadorNumeroLetraAleatorio(1);
        }
    }
    password[longitud] = '\0';  // Termina la cadena
    return password;
}

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
    srand(time(NULL));
    iniciar_sockets();

    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == INVALID_SOCKET) {
        printf("Error al crear el socket. Código de error: %d\n", WSAGetLastError());
        cerrar_sockets();
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(8080);

    if (bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
        printf("Error al enlazar el socket. Código de error: %d\n", WSAGetLastError());
        cerrar_sockets();
        exit(EXIT_FAILURE);
    }

    if (listen(server_socket, 5) == SOCKET_ERROR) {
        printf("Error al escuchar. Código de error: %d\n", WSAGetLastError());
        cerrar_sockets();
        exit(EXIT_FAILURE);
    }

    printf("Servidor escuchando en el puerto 8080...\n");

    int client_socket;
    struct sockaddr_in client_addr;
    int client_len = sizeof(client_addr);
    int salir = 0;
    do{
        client_socket = accept(server_socket, (struct sockaddr*)&client_addr, &client_len);
        if (client_socket == INVALID_SOCKET) {
            printf("Error al aceptar la conexión. Código de error: %d\n", WSAGetLastError());
            cerrar_sockets();
            exit(EXIT_FAILURE);
        }
        char *menu = "Elige entre las siguientes opciones:\n 1. Crear usuario.\n 2. Crear password.\n 0. Salir\n";
        send(client_socket, menu, strlen(menu), 0);

        int opcion;
        recv(client_socket, &opcion, sizeof(opcion), 0);
        opcion = ntohl(opcion);

        if (opcion == 0) {
            char *mensaje = "Adios";
            send(client_socket, mensaje, strlen(mensaje), 0);
        } else if(opcion > 2){
            char *mensaje = "Opcion incorrecta";
            send(client_socket, mensaje, strlen(mensaje), 0);
        } else {
            char *mensaje = "dime el largo que te gustaria que tenga";
            send(client_socket, mensaje, strlen(mensaje), 0);

            int largo;
            recv(client_socket, &largo, sizeof(largo), 0);
            largo = ntohl(largo);
            char *resultado;
            if(opcion == 1){
                resultado = generadorUsuario(largo);
            }else if(opcion == 2){
                resultado = generadorPassword(largo);
            }

            send(client_socket, resultado, strlen(resultado), 0);
        }
        closesocket(client_socket);
    }while(!salir);
        closesocket(server_socket);
        cerrar_sockets();
        return 0;

}
