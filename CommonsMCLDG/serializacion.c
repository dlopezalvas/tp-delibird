#include "serializacion.h"

void enviar_mensaje(t_mensaje* mensaje, int socket){

	t_paquete* paquete = malloc(sizeof(t_paquete));

	t_buffer* buffer_cargado = cargar_buffer(mensaje);

	paquete -> buffer = buffer_cargado;

	paquete -> codigo_operacion = mensaje -> tipo_mensaje;

	int bytes = 0;
;
	void* a_enviar = serializar_paquete(paquete, &bytes, mensaje -> id);

	send(socket,a_enviar,bytes,0);

	free(a_enviar);
//	free(buffer_cargado->stream);
//	free(buffer_cargado);
//	free(paquete -> buffer -> stream);
	free(paquete -> buffer);
	free(paquete);

}

void* serializar_paquete(t_paquete* paquete, int *bytes, uint32_t id){ //id == 0 NO TIENE ID

	int size = sizeof(uint32_t) + paquete->buffer->size + sizeof(paquete->codigo_operacion);
	if(id != 0){
		size +=sizeof(uint32_t);
	}
	void* a_enviar = malloc (size);

	memcpy(a_enviar + *bytes, &paquete-> codigo_operacion, sizeof(paquete->codigo_operacion));
	*bytes += sizeof(paquete->codigo_operacion);
	memcpy(a_enviar  + *bytes, &(paquete -> buffer -> size),sizeof(int));
	*bytes += sizeof(int);
	memcpy(a_enviar  + *bytes, paquete -> buffer -> stream, paquete -> buffer -> size);
	*bytes += paquete->buffer->size;
	if(id != 0){
		memcpy(a_enviar + *bytes, &id, sizeof(uint32_t));
		*bytes +=sizeof(uint32_t);
	}


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
		case(LOCALIZED_POKEMON): return buffer_localized_pokemon(parametros);
	}
	return 0;
}

///////////////////////////SERIALIZAR/////////////////////////////////
t_buffer* buffer_localized_pokemon(char** parametros){
	t_buffer* buffer = malloc(sizeof(t_buffer));
	char* nombre = parametros[0];
	uint32_t cantidad = atoi(parametros[1]);
	uint32_t cantidadParametros = cantidad*2;
	t_localized_pokemon localized_pokemon;
	coordenadas_pokemon coord;

	localized_pokemon.nombre.nombre = malloc(sizeof(char*));

	strcpy(localized_pokemon.nombre.nombre, nombre);
	localized_pokemon.nombre.largo_nombre = strlen(localized_pokemon.nombre.nombre);
	localized_pokemon.cantidad = cantidad;

	buffer -> size = sizeof(uint32_t)*(cantidadParametros+2) + strlen(localized_pokemon.nombre.nombre) + 1;
	void* stream = malloc(buffer -> size);
	int offset = 0;


	memcpy(stream + offset, &localized_pokemon.cantidad, sizeof(uint32_t));
	offset += sizeof(uint32_t);

	for(int i = 2; cantidadParametros+2>i ; i++){
		coord.pos_x = atoi(parametros[i]);
		i++;
		coord.pos_y = atoi(parametros[i]);

		memcpy(stream + offset, &coord.pos_x, sizeof(uint32_t));
		offset += sizeof(uint32_t);
		memcpy(stream + offset, &coord.pos_y, sizeof(uint32_t));
		offset += sizeof(uint32_t);
	}

	memcpy(stream + offset, &localized_pokemon.nombre.largo_nombre, sizeof(uint32_t));
	offset += sizeof(uint32_t);
	memcpy(stream + offset, localized_pokemon.nombre.nombre, strlen(localized_pokemon.nombre.nombre) + 1);

	buffer -> stream = stream;
	return buffer;
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

	//free(stream);

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

//	free(nombre);

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

//	free(position_and_name.nombre.nombre);

	buffer -> stream = stream;

	//free(stream);

	return buffer;

}

t_buffer* buffer_get_pokemon(char** parametros){

	t_buffer* buffer = malloc(sizeof(t_buffer));

	char* nombre = parametros[0];

	t_get_pokemon get_pokemon;

	get_pokemon.nombre.nombre = malloc(sizeof(char*));
	strcpy(get_pokemon.nombre.nombre, nombre);
	get_pokemon.nombre.largo_nombre = strlen(get_pokemon.nombre.nombre);

	//free(nombre);

	buffer -> size = sizeof(uint32_t) + strlen(get_pokemon.nombre.nombre) + 1;

	void* stream = malloc(buffer -> size);
	int offset = 0;

	memcpy(stream + offset, &get_pokemon.nombre.largo_nombre, sizeof(uint32_t));
	offset += sizeof(uint32_t);
	memcpy(stream + offset, get_pokemon.nombre.nombre, strlen(get_pokemon.nombre.nombre) + 1);

//	free(get_pokemon.nombre.nombre);

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

	//free(nombre);

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

	//free(stream);

	return buffer;
}

///////////////////////////DESERIALIZAR/////////////////////////////////

t_get_pokemon* deserializar_get_pokemon(void* buffer){

	t_get_pokemon* get_pokemon = malloc(sizeof(t_get_pokemon));

	memcpy(&get_pokemon->nombre.largo_nombre, buffer, sizeof(uint32_t));
	buffer += sizeof(uint32_t);
	get_pokemon->nombre.nombre = malloc(get_pokemon->nombre.largo_nombre + 1);
	memcpy(get_pokemon->nombre.nombre, buffer, get_pokemon->nombre.largo_nombre + 1);

	return get_pokemon;
}

t_localized_pokemon* deserializar_localized_pokemon(void*buffer){

	t_localized_pokemon* localized_pokemon = malloc(sizeof(t_localized_pokemon));


	localized_pokemon->listaCoordenadas = list_create();


	memcpy(&localized_pokemon->cantidad,buffer,  sizeof(uint32_t));
	buffer += sizeof(uint32_t);

	int cantidadParametros = localized_pokemon->cantidad * 2;
	coordenadas_pokemon coord;
	coordenadas_pokemon* coordenadas;
	for(int i = 0; cantidadParametros>i ; i+=2){
		coordenadas = malloc(sizeof(coordenadas_pokemon*));
		memcpy(&coord.pos_x,buffer, sizeof(uint32_t));
		buffer += sizeof(uint32_t);
		memcpy(&coord.pos_y,buffer, sizeof(uint32_t));
		buffer += sizeof(uint32_t);
		coordenadas->pos_x = coord.pos_x;
		coordenadas->pos_y = coord.pos_y;
		list_add(localized_pokemon->listaCoordenadas, coordenadas);
	}

	memcpy(&localized_pokemon->nombre.largo_nombre, buffer, sizeof(uint32_t));
	buffer += sizeof(uint32_t);
	localized_pokemon->nombre.nombre = malloc(localized_pokemon->nombre.largo_nombre + 1);
	memcpy(localized_pokemon->nombre.nombre,buffer, localized_pokemon->nombre.largo_nombre + 1);
	buffer += localized_pokemon->nombre.largo_nombre + 1;

	return localized_pokemon;

}

t_new_pokemon* deserializar_new_pokemon(void* buffer){

	t_new_pokemon* new_pokemon = malloc(sizeof(t_new_pokemon));

	memcpy( &new_pokemon->coordenadas.pos_y, buffer, sizeof(uint32_t));
	buffer += sizeof(uint32_t);
	memcpy(&new_pokemon->coordenadas.pos_x, buffer, sizeof(uint32_t));
	buffer += sizeof(uint32_t);
	memcpy(&new_pokemon->cantidad, buffer, sizeof(uint32_t));
	buffer += sizeof(uint32_t);
	memcpy(&new_pokemon->nombre.largo_nombre, buffer, sizeof(uint32_t));
	buffer += sizeof(uint32_t);
	new_pokemon->nombre.nombre = malloc(new_pokemon->nombre.largo_nombre + 1);
	memcpy(new_pokemon->nombre.nombre, buffer, new_pokemon->nombre.largo_nombre + 1);

	return new_pokemon;
}

t_position_and_name* deserializar_position_and_name(void* buffer){

	t_position_and_name* position_and_name = malloc(sizeof(t_position_and_name));

	memcpy( &position_and_name->coordenadas.pos_y, buffer, sizeof(uint32_t));
	buffer += sizeof(uint32_t);
	memcpy(&position_and_name->coordenadas.pos_x, buffer, sizeof(uint32_t));
	buffer += sizeof(uint32_t);
	memcpy(&position_and_name->nombre.largo_nombre, buffer, sizeof(uint32_t));
	buffer += sizeof(uint32_t);
	position_and_name->nombre.nombre = malloc(position_and_name->nombre.largo_nombre + 1);
	memcpy(position_and_name->nombre.nombre, buffer, position_and_name->nombre.largo_nombre + 1);

	return position_and_name;
}

t_caught_pokemon* deserializar_caught_pokemon(void* buffer){

	t_caught_pokemon* caught_pokemon = malloc(sizeof(t_caught_pokemon));

	memcpy(&caught_pokemon->caught, buffer, sizeof(uint32_t));

	return caught_pokemon;
}







