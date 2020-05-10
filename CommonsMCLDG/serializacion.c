#include "serializacion.h"
#include "socket.h"


void* serializar_paquete(t_paquete* paquete, int *bytes){

	int desplazamiento = 0;
	int size_serializado = sizeof(paquete->codigo_operacion) + sizeof(paquete->buffer->size) + *bytes;

	void* buffer = malloc(size_serializado);

	memcpy(buffer + desplazamiento,&(paquete->codigo_operacion),sizeof(paquete->codigo_operacion));
	desplazamiento += sizeof(paquete->codigo_operacion);

	memcpy(buffer + desplazamiento,&(paquete->buffer->size),sizeof(paquete->buffer->size));
	desplazamiento += sizeof(paquete->buffer->size);

	memcpy(buffer + desplazamiento,&(paquete->buffer->stream),paquete->buffer->size);
	desplazamiento += paquete->buffer->size;

	*bytes = size_serializado;

	return buffer;
}

void* enviar_mensaje(char* mensaje, int socket_cliente){

	t_paquete *paquete = malloc(sizeof(t_paquete));
	//aca esta hardcodeado con MENSAJE, la idea seria una forma de enviar c/u de las queue_cod
	paquete->codigo_operacion = MENSAJE;
	paquete->buffer = malloc(sizeof(t_buffer));
	paquete->buffer->size = mensaje;
	paquete->buffer->size = strlen(mensaje) +1;
	int size_serializado;
	void *serializado = serializar_paquete(paquete, &size_serializado);

	if(send(socket_cliente, serializado, size_serializado,0)== -1){
		perror("No se pudo enviar el mensaje");
	}else{
		;
	}

}

void* recibir_mensaje(int socket_cliente, int* size)
{
	void * buffer;

	recv(socket_cliente, size, sizeof(int), MSG_WAITALL);
	buffer = malloc(*size);
	recv(socket_cliente, buffer, *size, MSG_WAITALL);

	return buffer;
}



