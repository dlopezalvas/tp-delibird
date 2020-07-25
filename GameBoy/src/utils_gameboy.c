#include "utils_gameboy.h"

extern t_config* config;
extern t_log* logger;


void iniciar_gameboy(void){

	config = leer_config(PATH);
	logger = iniciar_logger(config);

	iniciar_consola(config);
  	terminar_proceso(1,logger,config);
}

//////////////CONSOLA//////////////////////////
void iniciar_consola(t_config* config){

	char * linea;

	tipo_id flag_id = NO_TIENE_ID;

	linea = readline(">");

	if(linea){
		add_history(linea);
	}

	char** linea_split = string_split(linea," ");

	//free(linea);

	if(string_equals_ignore_case(linea_split[0],comando_help)){

		help(linea_split[1]);
		liberar_vector(linea_split);
		iniciar_consola(config);

	}else if(verificar_mensaje(linea_split, config, &flag_id)){

		ejecutar_proceso(linea_split, config, flag_id);
		liberar_vector(linea_split);

		//liberar_consola(proceso, tipo_mensaje, linea_split);
		iniciar_consola(config);
	}else if(string_equals_ignore_case(linea_split[0],comando_exit)){

		printf("%s\n", terminar_consola);
		liberar_vector(linea_split);

	}else{

		printf("%s\n", mensaje_invalido);
		liberar_vector(linea_split);
		iniciar_consola(config);
	}

}

void help(char* mensaje){

	if(mensaje == NULL){
		puts(help_argumentos);
		return;
	}

	if(string_equals_ignore_case(mensaje,PROCESOS))
		puts(help_procesos);
	else if(string_equals_ignore_case(mensaje,FORMATO))
		puts(help_formato_argumentos);
	else if(string_equals_ignore_case(mensaje,MENSAJES))
			puts(help_mensajes);
	else
		{
			puts(argumento_invalido);
			puts(help_argumentos);
		}
}

void liberar_consola(char* proceso, char* mensaje, char** linea_split){
	free(proceso);
	free(mensaje);
	liberar_vector(linea_split);
}

//////////////VERIFICACION DEL MENSAJE////////////////////////
bool verificar_mensaje(char** linea_split, t_config* config, tipo_id* flag_id ){

		char * proceso = linea_split[0];
		char * tipo_mensaje = linea_split[1];

		const bool tiene_argumentos_suficientes = !(linea_split[1] == NULL || linea_split[2] == NULL); //tiene que tener minimo 3 "palabras"
		const bool mensaje_valido = validar_mensaje(proceso,tipo_mensaje) && validar_argumentos(tipo_mensaje,linea_split,proceso, flag_id);

		return tiene_argumentos_suficientes && mensaje_valido;
}

bool validar_argumentos(char* tipo_mensaje, char** linea_split, char* proceso, tipo_id *flag_id){

	int cantidad_total = cantidad_argumentos(linea_split);

	if(string_equals_ignore_case(proceso, MENSAJE_MODO_SUSCRIPTOR)){
		return cantidad_total == 1;
	}else{
		switch(codigo_mensaje(tipo_mensaje)){
		case APPEARED_POKEMON:
			if(string_equals_ignore_case(proceso,BROKER)){
				*flag_id = ID_AL_FINAL;
				return cantidad_total == ARGUMENTOS_APPEARED_POKEMON + 1; //MAS EL ID
			}else{
				return cantidad_total == ARGUMENTOS_APPEARED_POKEMON;
			}
		case NEW_POKEMON:
			if(string_equals_ignore_case(proceso,GAMECARD)){
				*flag_id = ID_AL_FINAL;
				return cantidad_total == ARGUMENTOS_NEW_POKEMON + 1; //MAS EL ID
			}else{
				return cantidad_total == ARGUMENTOS_NEW_POKEMON;
			}
		case CATCH_POKEMON:
			if(string_equals_ignore_case(proceso,GAMECARD)){
				*flag_id = ID_AL_FINAL;
				return cantidad_total == ARGUMENTOS_CATCH_POKEMON + 1; //MAS EL ID
			}else{
				return cantidad_total == ARGUMENTOS_CATCH_POKEMON;
			}
		case CAUGHT_POKEMON:
			if(string_equals_ignore_case(proceso,BROKER)){
				*flag_id = ID_AL_PRINCIPIO;
				return cantidad_total == ARGUMENTOS_CAUGHT_POKEMON + 1; //MAS EL ID
			}else{
				return cantidad_total == ARGUMENTOS_CAUGHT_POKEMON;
			}
		case GET_POKEMON:
			if(string_equals_ignore_case(proceso,GAMECARD)){
				*flag_id = ID_AL_FINAL;
				return cantidad_total == ARGUMENTOS_GET_POKEMON + 1; //MAS EL ID
			}else{
				return cantidad_total == ARGUMENTOS_GET_POKEMON;
			}
		}
	}

	return false;
}

bool validar_mensaje(char* proceso, char* mensaje){

	if(string_equals_ignore_case(TEAM,proceso)){
		const bool team_is_valid_mensaje = string_equals_ignore_case(MENSAJE_APPEARED_POKEMON,mensaje);

		return team_is_valid_mensaje;
	}

	if(string_equals_ignore_case(MENSAJE_MODO_SUSCRIPTOR, proceso)){
		const bool suscriptor_is_valid_mensaje =
			string_equals_ignore_case(MENSAJE_NEW_POKEMON,mensaje) ||
			string_equals_ignore_case(MENSAJE_CATCH_POKEMON,mensaje) ||
			string_equals_ignore_case(MENSAJE_GET_POKEMON,mensaje) ||
			string_equals_ignore_case(MENSAJE_APPEARED_POKEMON,mensaje) ||
			string_equals_ignore_case(MENSAJE_LOCALIZED_POKEMON,mensaje) ||
			string_equals_ignore_case(MENSAJE_CAUGHT_POKEMON,mensaje);

		return suscriptor_is_valid_mensaje;
	}

	if(string_equals_ignore_case(GAMECARD,proceso)){
		const bool gamecard_is_valid_mensaje =
			string_equals_ignore_case(MENSAJE_NEW_POKEMON,mensaje) ||
			string_equals_ignore_case(MENSAJE_CATCH_POKEMON,mensaje) ||
			string_equals_ignore_case(MENSAJE_GET_POKEMON,mensaje);

		return gamecard_is_valid_mensaje;
	}

	if(string_equals_ignore_case(BROKER,proceso)){
		const bool broker_is_valid_mensaje =
				string_equals_ignore_case(MENSAJE_NEW_POKEMON,mensaje) ||
				string_equals_ignore_case(MENSAJE_APPEARED_POKEMON,mensaje) ||
				string_equals_ignore_case(MENSAJE_CATCH_POKEMON,mensaje) ||
				string_equals_ignore_case(MENSAJE_CAUGHT_POKEMON,mensaje) ||
				string_equals_ignore_case(MENSAJE_GET_POKEMON,mensaje);

		return broker_is_valid_mensaje;
	}

	return false;
}

//////////////EJECUCION DE PROCESOS/////////////////////////
void ejecutar_proceso(char** linea_split, t_config* config, tipo_id flag_id){
	if(string_equals_ignore_case(BROKER,linea_split[0])){
		ejecutar_broker(linea_split, config, flag_id);
	}else if(string_equals_ignore_case(TEAM,linea_split[0])){
		ejecutar_team(linea_split, config);
	}else if(string_equals_ignore_case(GAMECARD,linea_split[0])){
		ejecutar_gamecard(linea_split, config, flag_id);
	}else if(string_equals_ignore_case(MENSAJE_MODO_SUSCRIPTOR, linea_split[0])){
		ejecutar_modo_suscriptor(linea_split, config);
	}else{
		perror("No se ha podido ejecutar el proceso");
	}
}

void ejecutar_modo_suscriptor(char** linea_split, t_config* config){
	char* ip = config_get_string_value(config,IP_BROKER);
	char* puerto = config_get_string_value(config,PUERTO_BROKER);

	t_mensaje* mensaje = malloc(sizeof(t_mensaje));

	mensaje->tipo_mensaje = SUSCRIPCION;
	char* cola[1] = {linea_split[1]};
	mensaje -> parametros = cola;

	int socket_broker = iniciar_cliente(ip, puerto);

	log_info(logger,"Se ha suscripto a la cola %s", linea_split[1]);

	enviar_mensaje(mensaje, socket_broker);

	puts("enviar mensaje");

	int tiempo_de_espera = atoi(linea_split[2]);

	pthread_t modo_suscriptor;

	pthread_create(&modo_suscriptor, NULL, (void*) recibir_mensajes_de_cola, &socket_broker);

	pthread_detach(modo_suscriptor);

	sleep(tiempo_de_espera);

	pthread_cancel(modo_suscriptor);

	liberar_conexion(socket_broker);
}

void recibir_mensajes_de_cola(int* socket){

	while(1){
		int cod_op;
		if(recv(*socket, &cod_op, sizeof(int), MSG_WAITALL) == -1)
			cod_op = -1;
		process_request(cod_op, *socket);
	}
}

void process_request(int cod_op, int cliente_fd){
	int size = 0;
	void* buffer = recibir_mensaje(cliente_fd, &size);
	puts("recibi un mensaje");

	t_new_pokemon* new_pokemon;
	t_position_and_name* appeared_pokemon;
	t_position_and_name* catch_pokemon;
	t_caught_pokemon* caught_pokemon;
	t_get_pokemon* get_pokemon;
	t_localized_pokemon* localized_pokemon;

	switch (cod_op) {
	case NEW_POKEMON:
		new_pokemon = deserializar_new_pokemon(buffer);
		//Envio ack
		send(cliente_fd,&new_pokemon->id,sizeof(uint32_t),0);
		//
		log_info(logger, "Llego el mensaje New_pokemon del pokemon: %s en las coordenadas: %d %d con la cantidad de: %d con ID: %d", new_pokemon->nombre, new_pokemon->coordenadas.pos_x, new_pokemon->coordenadas.pos_y, new_pokemon->cantidad, new_pokemon->id);
		break;
	case APPEARED_POKEMON:
		appeared_pokemon = deserializar_position_and_name(buffer);
		send(cliente_fd,&appeared_pokemon->id,sizeof(uint32_t),0);
		log_info(logger, "Llego el mensaje Appeared_pokemon del pokemon: %s en las coordenadas: %d %d con ID: %d", appeared_pokemon->nombre.nombre, appeared_pokemon->coordenadas.pos_x, appeared_pokemon->coordenadas.pos_y, appeared_pokemon->id);
		break;
	case CATCH_POKEMON:
		catch_pokemon = deserializar_position_and_name(buffer);
		send(cliente_fd,&catch_pokemon->id,sizeof(uint32_t),0);
		log_info(logger, "Llego el mensaje Catch_pokemon del pokemon: %s en las coordenadas: %d %d con ID: %d", catch_pokemon->nombre.nombre, catch_pokemon->coordenadas.pos_x, catch_pokemon->coordenadas.pos_y, catch_pokemon->id);
		break;
	case CAUGHT_POKEMON:
		caught_pokemon = deserializar_caught_pokemon(buffer);
		send(cliente_fd,&caught_pokemon->id,sizeof(uint32_t),0);
		log_info(logger, "Llego el mensaje Caught_pokemon con el bit de captura en: %d con ID: %d con correlation id: %d", caught_pokemon->caught, caught_pokemon->id, caught_pokemon->correlation_id );
		break;
	case GET_POKEMON:
		get_pokemon = deserializar_get_pokemon(buffer);
//		send(cliente_fd,&(get_pokemon->id),sizeof(uint32_t),0);
		log_info(logger, "Llego el mensaje Get_pokemon del pokemon: %s con ID: %d", get_pokemon->nombre.nombre , get_pokemon->id);
		break;
	case LOCALIZED_POKEMON:
		localized_pokemon = deserializar_localized_pokemon(buffer);
		send(cliente_fd,&localized_pokemon->id,sizeof(uint32_t),0);
		char* mensaje_localized_pokemon = string_new();
		string_append_with_format(&mensaje_localized_pokemon, "Llego el mensaje localized_pokemon del pokemon: %s con la cantidad de: %d con ID: %d ", localized_pokemon->nombre.nombre, localized_pokemon->cantidad, localized_pokemon->id);
		coordenadas_pokemon* coord;
		for(int i = 0; i<localized_pokemon->cantidad; i++){
			coord = list_get(localized_pokemon->listaCoordenadas, i);
			string_append_with_format(&mensaje_localized_pokemon, "en las coordenadas: %d %d", coord->pos_x, coord->pos_y);
		}
		string_append_with_format(&mensaje_localized_pokemon, "con correlation id: %d", localized_pokemon->correlation_id);
		log_info(logger, mensaje_localized_pokemon);
	}

}


void ejecutar_broker(char** linea_split, t_config* config, tipo_id flag_id){

	char* ip = config_get_string_value(config,IP_BROKER);
	char* puerto = config_get_string_value(config,PUERTO_BROKER);

	op_code codigo_operacion = codigo_mensaje(linea_split[1]);

	t_mensaje* mensaje = malloc(sizeof(t_mensaje));

	mensaje -> tipo_mensaje = codigo_operacion;
	mensaje -> parametros = argumentos(linea_split, flag_id);

	int socket_broker = iniciar_cliente(ip, puerto);

	log_info(logger,"Se ha establecido una conexion con el proceso Broker");

	enviar_mensaje(mensaje, socket_broker);
	uint32_t id;
	int _recv;
	_recv = recv(socket_broker, &id, sizeof(uint32_t), 0);
	if(_recv == 0 || _recv == -1){
		log_info(logger,"DEBUG: recibio el ack %d",id);
	}
	liberar_conexion(socket_broker);

	free(mensaje -> parametros);
	free(mensaje);
}


void ejecutar_team(char** linea_split, t_config* config){
	char* ip = config_get_string_value(config,IP_TEAM);
	char* puerto = config_get_string_value(config,PUERTO_TEAM);

	op_code codigo_operacion = codigo_mensaje(linea_split[1]);

	t_mensaje* mensaje = malloc(sizeof(t_mensaje));

	mensaje -> tipo_mensaje = codigo_operacion;
	mensaje -> parametros = argumentos(linea_split, 0); //no necesita id en ningun mensaje

	int socket_team = iniciar_cliente(ip, puerto);

	log_info(logger,"Se ha establecido una conexion con el proceso Team");

	enviar_mensaje(mensaje, socket_team);
//	uint32_t id;
//	int _recv;
//	_recv = recv(socket_team, &id, sizeof(uint32_t), 0);
//	if(_recv == 0 || _recv == -1){
//		log_info(logger,"DEBUG: recibio el ack %d",id);
//	}
	liberar_conexion(socket_team);

	free(mensaje -> parametros);
	free(mensaje);
}


void ejecutar_gamecard(char** linea_split, t_config* config, tipo_id flag_id){
	char* ip = config_get_string_value(config,IP_GAMECARD);
	char* puerto = config_get_string_value(config,PUERTO_GAMECARD);

	op_code codigo_operacion = codigo_mensaje(linea_split[1]);

	t_mensaje* mensaje = malloc(sizeof(t_mensaje));

	mensaje -> tipo_mensaje = codigo_operacion;
	mensaje -> parametros = argumentos(linea_split, flag_id);

	int socket_gamecard = iniciar_cliente(ip, puerto);

	if(socket_gamecard != 1){
		log_info(logger,"Se ha establecido una conexion con el proceso GameCard");

	}

	enviar_mensaje(mensaje, socket_gamecard);

//	uint32_t id;
//	int _recv;
//	_recv = recv(socket_gamecard, &id, sizeof(uint32_t), 0);
//	if(_recv == 0 || _recv == -1){
//		log_info(logger,"DEBUG: recibio el ack %d",id);
//	}
	liberar_conexion(socket_gamecard);

	free(mensaje -> parametros);
	free(mensaje);
}

/////////////////CALCULOS ARGUMENTOS/ID/CODIGO////////////////////////

char* calcular_id(tipo_id flag_id, char** linea_split){
	char* id;

//	char** aux = argumentos(linea_split, 0);

	int cantidad = cantidad_argumentos(linea_split) - 1; //posicion del ultimo argumento

	switch(flag_id){
		case NO_TIENE_ID:
			id = "0";
			break;
		case ID_AL_PRINCIPIO:
			if(string_equals_ignore_case(linea_split[1], "CAUGHT_POKEMON")) id = "0";
			else id = linea_split[2];
			break;
		case ID_AL_FINAL:
			if(string_equals_ignore_case(linea_split[1], "APPEARED_POKEMON")) id="0";
			else	id = linea_split[cantidad+2];
			break;
	}

	//free(aux);

	return id;
}

char* calcular_correlation_id(tipo_id flag_id, char** linea_split){
	char* id;

//	char** aux = argumentos(linea_split, 0);

	int cantidad = cantidad_argumentos(linea_split) - 1; //posicion del ultimo argumento

	switch(flag_id){
		case NO_TIENE_ID:
			id = "0";
			break;
		case ID_AL_FINAL:
			if(string_equals_ignore_case(linea_split[1], "APPEARED_POKEMON")) id = linea_split[cantidad+2];
			else id = "0";
			break;
		case ID_AL_PRINCIPIO:
			if(string_equals_ignore_case(linea_split[1], "CAUGHT_POKEMON")) id = linea_split[2];
			else id = "0";
			break;
	}

	//free(aux);

	return id;
}
int cantidad_argumentos (char** linea_split){
	int cantidad = 0;

	while(linea_split[cantidad]!=NULL){
		cantidad++;
	}
	return cantidad - 2; //resto el proceso y el tipo de mensaje, quedan solo los argumentos
}

char** argumentos(char** linea_split, tipo_id flag_id){

	int cantidad = cantidad_argumentos(linea_split);

	int i_linea_split = 2; //comienza a cargar la lista a partir del primer argumento, sin contar el proceso y tipo de mensaje

	switch(flag_id){
		case NO_TIENE_ID:
			break;
		case ID_AL_FINAL:	//no cargo el ultimo argumento
			cantidad--;
			break;
		case ID_AL_PRINCIPIO:	//empiezo desde el primer argumento, sin tener en cuenta el id (posicion 3)
			cantidad--;
			i_linea_split ++;
			break;
	}
	char* id = calcular_id(flag_id, linea_split);
	char* correlation_id = calcular_correlation_id(flag_id, linea_split);

	char** lista_argumentos = malloc(sizeof(char*)*(cantidad+2));
	int k;

	for(k = 0; k < cantidad; k++){
		lista_argumentos[k] = linea_split[i_linea_split];
		i_linea_split++;
	}

	lista_argumentos[k] = id;
	lista_argumentos[k+1] = correlation_id;


	return lista_argumentos;
}



