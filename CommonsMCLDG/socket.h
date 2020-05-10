#ifndef socket_h
#define socket_h

#include <stdlib.h>
#include <stdio.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include "utils.h"
#include <arpa/inet.h>//inet_addr
#include <pthread.h>

typedef enum
{
	MENSAJE=1,
	NEW_POKEMON=2,
	APPEARED_POKEMON=3,
	CATCH_POKEMON=4,
	CAUGHT_POKEMON=5,
	GET_POKEMON=6,
	LOCALIZED_POKEMON=7,

}queue_code;

typedef struct
{
	int size;
	void* stream;
} t_buffer;

typedef struct
{
	queue_code codigo_operacion;
	t_buffer* buffer;
} t_paquete;

pthread_t thread;

void* recibir_buffer(int*, int);
int crear_conexion(char *ip, char* puerto);
void iniciar_servidor(char* ip, char* puerto);
void esperar_cliente(int);
void* recibir_mensaje(int socket_cliente, int* size);
int recibir_operacion(int);
void process_request(int cod_queue, int cliente_fd);
void serve_client(int *socket);
//void* serializar_paquete(t_paquete* paquete, int *bytes);
void devolver_mensaje(void* payload, int size, int socket_cliente);


#endif /* socket_h */
