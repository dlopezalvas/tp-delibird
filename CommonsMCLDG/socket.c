#include "socket.h"

int crear_conexion(char *ip, char* puerto)
{
	struct addrinfo hints;
	struct addrinfo *server_info;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	getaddrinfo(ip, puerto, &hints, &server_info);

	int socket_cliente = socket(server_info->ai_family, server_info->ai_socktype, server_info->ai_protocol);

	if(connect(socket_cliente, server_info->ai_addr, server_info->ai_addrlen) == -1)
		printf("error");

	freeaddrinfo(server_info);

	return socket_cliente;
}

void serve_client(int* socket)
{
	int cod_queue;
	if(recv(*socket, &cod_queue, sizeof(int), MSG_WAITALL) == -1)
		cod_queue = -1;
	process_request(cod_queue, *socket);
}

void process_request(int cod_queue, int cliente_fd) {
	int size;
	void* msg;
		switch (cod_queue) {
		case MENSAJE:
			msg = recibir_mensaje(cliente_fd, &size);
			devolver_mensaje(msg, size, cliente_fd);
			free(msg);
			break;
		case 0:
			pthread_exit(NULL);
		case -1:
			pthread_exit(NULL);
		}
}

void* liberar_conexion(int socket_cliente){

	close(socket_cliente);
}

void iniciar_servidor(char* ip, char* puerto)
{
	int socket_servidor;

    struct addrinfo hints, *servinfo, *p;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    getaddrinfo(ip, puerto, &hints, &servinfo);

    for (p=servinfo; p != NULL; p = p->ai_next)
    {
        if ((socket_servidor = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1)
            continue;

        if (bind(socket_servidor, p->ai_addr, p->ai_addrlen) == -1) {
            close(socket_servidor);
            perror("No se pudo asignar la direccion al socket");
            continue;
        }
        break;
    }


    listen(socket_servidor, SOMAXCONN);


    freeaddrinfo(servinfo);

    while(1)
    	esperar_cliente(socket_servidor);
}

void esperar_cliente(int socket_servidor)
{
	struct sockaddr_in dir_cliente;

	int tam_direccion = sizeof(struct sockaddr_in);

	int socket_cliente = accept(socket_servidor, (void*) &dir_cliente, &tam_direccion);

	pthread_create(&thread,NULL,(void*)serve_client,&socket_cliente);
	pthread_detach(thread);

}

//void devolver_mensaje(void* payload, int size, int socket_cliente)
//{
//	t_paquete* paquete = malloc(sizeof(t_paquete));
//
//	paquete->codigo_operacion = MENSAJE;
//	paquete->buffer = malloc(sizeof(t_buffer));
//	paquete->buffer->size = size;
//	paquete->buffer->stream = malloc(paquete->buffer->size);
//	memcpy(paquete->buffer->stream, payload, paquete->buffer->size);
//
//	int bytes = paquete->buffer->size + 2*sizeof(int);
//
//	void* a_enviar = serializar_paquete(paquete, bytes);
//
//	send(socket_cliente, a_enviar, bytes, 0);
//
//	free(a_enviar);
//	free(paquete->buffer->stream);
//	free(paquete->buffer);
//	free(paquete);
//}

