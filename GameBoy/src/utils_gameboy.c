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

	free(linea);

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

	return false;
}

bool validar_mensaje(char* proceso, char* mensaje){

	if(string_equals_ignore_case(TEAM,proceso)){
		const bool team_is_valid_mensaje = string_equals_ignore_case(MENSAJE_APPEARED_POKEMON,mensaje);

		return team_is_valid_mensaje;
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
	}else{
		perror("No se ha podido ejecutar el proceso");
	}
}


void ejecutar_broker(char** linea_split, t_config* config, tipo_id flag_id){

	char* ip = config_get_string_value(config,IP_BROKER);
	char* puerto = config_get_string_value(config,PUERTO_BROKER);

	op_code codigo_operacion = codigo_mensaje(linea_split[1]);

	t_mensaje* mensaje = malloc(sizeof(t_mensaje));

	mensaje -> tipo_mensaje = codigo_operacion;
	mensaje -> parametros = argumentos(linea_split, flag_id);
	mensaje -> id = calcular_id(flag_id, linea_split);

	int socket_broker = iniciar_cliente(ip, puerto);

	log_info(logger,"Se ha establecido una conexion con el proceso Broker");

	enviar_mensaje(mensaje, socket_broker);

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
	mensaje -> id = 0;

	int socket_team = iniciar_cliente(ip, puerto);

	log_info(logger,"Se ha establecido una conexion con el proceso Team");

	enviar_mensaje(mensaje, socket_team);

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
	mensaje -> id = calcular_id(flag_id, linea_split);

	int socket_gamecard = iniciar_cliente(ip, puerto);

	if(socket_gamecard != 1){
		log_info(logger,"Se ha establecido una conexion con el proceso GameCard");

	}

	enviar_mensaje(mensaje, socket_gamecard);

	liberar_conexion(socket_gamecard);

	free(mensaje -> parametros);
	free(mensaje);
}

/////////////////CALCULOS ARGUMENTOS/ID/CODIGO////////////////////////

uint32_t calcular_id(tipo_id flag_id, char** linea_split){
	uint32_t id;

	char** aux = argumentos(linea_split, 0);

	int cantidad = cantidad_argumentos(linea_split) - 1; //posicion del ultimo argumento

	switch(flag_id){
		case NO_TIENE_ID:
			id = 0;
			break;
		case ID_AL_PRINCIPIO:
			id = atoi(aux[0]);
			break;
		case ID_AL_FINAL:
			id = atoi(aux[cantidad]);
			break;
	}

	free(aux);

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

	int j_lista_argumentos = 0; //para iterar en la lista

	switch(flag_id){
		case NO_TIENE_ID:
			break;
		case ID_AL_FINAL:	//no cargo el ultimo argumento
			cantidad--;
			break;
		case ID_AL_PRINCIPIO:	//empiezo desde el primer argumento, sin tener en cuenta el id (posicion 3)
			i_linea_split ++;
			break;
	}

	char** lista_argumentos = malloc(sizeof(char**));

	while(cantidad!= 0){
		lista_argumentos[j_lista_argumentos] = linea_split[i_linea_split];
		i_linea_split++;
		j_lista_argumentos++;
		cantidad --;
	}

	return lista_argumentos;
}

op_code codigo_mensaje(char* tipo_mensaje){

	if(string_equals_ignore_case(MENSAJE_NEW_POKEMON, tipo_mensaje)){
		return NEW_POKEMON;
	}else if(string_equals_ignore_case(MENSAJE_APPEARED_POKEMON, tipo_mensaje)){
		return APPEARED_POKEMON;
	}else if(string_equals_ignore_case(MENSAJE_CATCH_POKEMON, tipo_mensaje)){
		return CATCH_POKEMON;
	}else if(string_equals_ignore_case(MENSAJE_CAUGHT_POKEMON, tipo_mensaje)){
			return CAUGHT_POKEMON;
	}else if(string_equals_ignore_case(MENSAJE_GET_POKEMON, tipo_mensaje)){
		return GET_POKEMON;
	}else if(string_equals_ignore_case(MENSAJE_LOCALIZED_POKEMON, tipo_mensaje)){
		return LOCALIZED_POKEMON;
	}else{
		return 0;
	}
}


