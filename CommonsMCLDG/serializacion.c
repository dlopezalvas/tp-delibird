#include "serializacion.h"
#include "socket.h"



void enviar_mensaje(t_mensaje* mensaje, int socket)
{
	t_paquete* paquete = malloc(sizeof(t_paquete));

	t_buffer* buffer_cargado = cargar_buffer(mensaje);
	paquete -> buffer = buffer_cargado;

	paquete -> codigo_operacion = mensaje -> tipo_mensaje;

	int bytes = 0;

	void* a_enviar = serializar_paquete(paquete, &bytes);

	send(socket,a_enviar,bytes,0);

	free(a_enviar);
	free(buffer_cargado -> stream);
	free(buffer_cargado);
	free(paquete -> buffer -> stream);
	free(paquete -> buffer);
	free(paquete);

}

void* serializar_paquete(t_paquete* paquete, int *bytes)
{

	void* a_enviar = malloc (sizeof(uint32_t) + paquete->buffer->size + sizeof(paquete->codigo_operacion));

	memcpy(a_enviar + *bytes, &paquete-> codigo_operacion, sizeof(paquete->codigo_operacion));
	*bytes += sizeof(paquete->codigo_operacion);
	memcpy(a_enviar  + *bytes, &(paquete -> buffer -> size),sizeof(int));
	*bytes += sizeof(int);
	memcpy(a_enviar  + *bytes, paquete -> buffer -> stream, paquete -> buffer -> size);
	*bytes += paquete->buffer->size;

	return a_enviar;
}


t_buffer* cargar_buffer(t_mensaje* mensaje){

	uint32_t proceso = mensaje -> tipo_mensaje;
	char** parametros = mensaje -> parametros;

	switch(proceso){
		case(NEW_POKEMON): return buffer_new_pokemon(parametros);
		case(GET_POKEMON): return buffer_get_pokemon(parametros);
		case(APPEARED_POKEMON): return buffer_position_and_name(parametros);
		case(CATCH_POKEMON): return buffer_position_and_name(parametros);
		case(CAUGHT_POKEMON): return buffer_caught_pokemon(parametros);
		//case(LOCALIZED_POKEMON: return buffer_localized_pokemon(linea_split);
	}
	return 0;
}

t_buffer* buffer_caught_pokemon(char** parametros){

	t_buffer* buffer = malloc(sizeof(t_buffer));
	uint32_t caught = atoi(parametros[0]);

	t_caught_pokemon caught_pokemon;
	caught_pokemon.caught = caught;

	buffer -> size = sizeof(uint32_t);

	void* stream = malloc(buffer -> size);

	memcpy(stream, &caught_pokemon.caught, sizeof(uint32_t));

	buffer -> stream = stream;

	free(stream);

	return buffer;
}

t_buffer* buffer_position_and_name(char** parametros){ //para mensajes APPEARED_POKEMON y CATCH_POKEMON

	t_buffer* buffer = malloc(sizeof(t_buffer));

	char* nombre = parametros[0];
	uint32_t pos_x = atoi(parametros[1]);
	uint32_t pos_y = atoi(parametros[2]);

	t_position_and_name position_and_name;

	position_and_name.nombre.nombre = malloc(sizeof(char*));

	strcpy(position_and_name.nombre.nombre, nombre);
	position_and_name.nombre.largo_nombre = strlen(position_and_name.nombre.nombre);
	position_and_name.coordenadas.pos_x = pos_x;
	position_and_name.coordenadas.pos_y = pos_y;

	free(nombre);

	buffer -> size = sizeof(uint32_t)*3 + strlen(position_and_name.nombre.nombre) + 1;

	void* stream = malloc(buffer -> size);
	int offset = 0;

	memcpy(stream + offset, &position_and_name.coordenadas.pos_y, sizeof(uint32_t));
	offset += sizeof(uint32_t);
	memcpy(stream + offset, &position_and_name.coordenadas.pos_x, sizeof(uint32_t));
	offset += sizeof(uint32_t);
	memcpy(stream + offset, &position_and_name.nombre.largo_nombre, sizeof(uint32_t));
	offset += sizeof(uint32_t);
	memcpy(stream + offset, position_and_name.nombre.nombre, strlen(position_and_name.nombre.nombre) + 1);

	free(position_and_name.nombre.nombre);

	buffer -> stream = stream;

	free(stream);

	return buffer;

}

t_buffer* buffer_get_pokemon(char** parametros){

	t_buffer* buffer = malloc(sizeof(t_buffer));

	char* nombre = parametros[0];

	t_get_pokemon get_pokemon;

	get_pokemon.nombre.nombre = malloc(sizeof(char*));
	strcpy(get_pokemon.nombre.nombre, nombre);
	get_pokemon.nombre.largo_nombre = strlen(get_pokemon.nombre.nombre);

	free(nombre);

	buffer -> size = sizeof(uint32_t) + strlen(get_pokemon.nombre.nombre) + 1;

	void* stream = malloc(buffer -> size);
	int offset = 0;

	memcpy(stream + offset, &get_pokemon.nombre.largo_nombre, sizeof(uint32_t));
	offset += sizeof(uint32_t);
	memcpy(stream + offset, get_pokemon.nombre.nombre, strlen(get_pokemon.nombre.nombre) + 1);

	free(get_pokemon.nombre.nombre);

	buffer -> stream = stream;

	//free(stream);

	return buffer;
}

t_buffer* buffer_new_pokemon(char** parametros){

	t_buffer* buffer = malloc(sizeof(t_buffer));
	char* nombre = parametros[0];
	uint32_t pos_x = atoi(parametros[1]);
	uint32_t pos_y = atoi(parametros[2]);
	uint32_t cantidad = atoi(parametros[3]);


	t_new_pokemon new_pokemon;
	new_pokemon.nombre.nombre = malloc(sizeof(char*));
	strcpy(new_pokemon.nombre.nombre, nombre);
	new_pokemon.nombre.largo_nombre = strlen(new_pokemon.nombre.nombre);
	new_pokemon.coordenadas.pos_x = pos_x;
	new_pokemon.coordenadas.pos_y = pos_y;
	new_pokemon.cantidad = cantidad;

	free(nombre);

	buffer -> size = sizeof(uint32_t)*4 + strlen(new_pokemon.nombre.nombre) + 1;

	void* stream = malloc(buffer -> size);
	int offset = 0;

	memcpy(stream + offset, &new_pokemon.coordenadas.pos_y, sizeof(uint32_t));
	offset += sizeof(uint32_t);
	memcpy(stream + offset, &new_pokemon.coordenadas.pos_x, sizeof(uint32_t));
	offset += sizeof(uint32_t);
	memcpy(stream + offset, &new_pokemon.cantidad, sizeof(uint32_t));
	offset += sizeof(uint32_t);
	memcpy(stream + offset, &new_pokemon.nombre.largo_nombre, sizeof(uint32_t));
	offset += sizeof(uint32_t);
	memcpy(stream + offset, new_pokemon.nombre.nombre, strlen(new_pokemon.nombre.nombre) + 1);

	free(new_pokemon.nombre.nombre);

	buffer -> stream = stream;

	free(stream);

	return buffer;
}

void* recibir_mensaje(int socket_cliente, int* size)
{
	void * buffer;

	recv(socket_cliente, size, sizeof(int), MSG_WAITALL);
	buffer = malloc(*size);
	recv(socket_cliente, buffer, *size, MSG_WAITALL);

	return buffer;
}



