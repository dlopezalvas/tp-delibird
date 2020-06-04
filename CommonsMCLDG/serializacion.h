#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/socket.h>
#include "utils.h"
#include "socket.h"

typedef struct{
  op_code tipo_mensaje;
  char **parametros; //ejemplo: ["PARAM1","PARAM2","PARAM3"]
  char *queue;
  uint32_t id;
}t_mensaje;


typedef struct{
	uint32_t size;
	void* stream;
} t_buffer;

typedef struct{
	op_code codigo_operacion;
	t_buffer* buffer;
} t_paquete;

typedef struct{
	uint32_t pos_x;
	uint32_t pos_y;
}coordenadas_pokemon;

typedef struct{
	uint32_t largo_nombre;
	char* nombre;
}nombre_pokemon;

typedef struct{
	coordenadas_pokemon coordenadas;
	nombre_pokemon nombre;
	uint32_t cantidad;
}t_new_pokemon;

typedef struct{
	coordenadas_pokemon coordenadas;
	nombre_pokemon nombre;
}t_position_and_name;

typedef struct{
	nombre_pokemon nombre;
}t_get_pokemon;

typedef struct{
	uint32_t caught;
}t_caught_pokemon;

void enviar_mensaje(t_mensaje* mensaje, int socket);
void* serializar_paquete(t_paquete* paquete, int *bytes, uint32_t id);

t_buffer* cargar_buffer(t_mensaje* mensaje);
t_buffer* buffer_caught_pokemon(char** parametros);
t_buffer* buffer_position_and_name(char** parametros);
t_buffer* buffer_get_pokemon(char** parametros);
t_buffer* buffer_new_pokemon(char** parametros);


