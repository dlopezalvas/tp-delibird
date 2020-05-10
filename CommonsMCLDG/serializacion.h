#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/socket.h>
#include "utils.h"
#include "socket.h"


typedef struct{
  char *proceso;
  char **parametros; //ejemplo: ["PARAM1","PARAM2","PARAM3"]
  char *queue;
  uint32_t *id;
}  t_mensaje;


void* serializar_paquete(t_paquete* paquete, int *bytes);

void* enviar_mensaje(char* mensaje, int socket_cliente);

void* recibir_mensaje(int socket_cliente,  int* size);

char* deserializar_mensaje(t_buffer* buffer);


