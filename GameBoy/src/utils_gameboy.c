#include "utils_gameboy.h"


void iniciar_gameboy(void){

	t_config* config = leer_config(PATH);
	t_log* logger = iniciar_logger(config);

	char *ip = config_get_string_value(config,IP_BROKER);
	char *puerto = config_get_string_value(config,PUERTO_BROKER);

  	log_info(logger,puerto);
  	log_info(logger,ip);

  	iniciar_consola(logger);
  	terminar_proceso(1,logger,config);
}

void iniciar_consola(t_log* logger){

	char * linea;

	while(1) {
		linea = readline(">");

		log_info(logger,linea);
		if(linea)
		  add_history(linea);

		if(string_equals_ignore_case(linea,comando_exit)) {
		   free(linea);
		   break;
		}

		char** linea_split = string_split(linea," ");

		free(linea);

		if(linea_split[2] == NULL){
				printf("%s\n", mensaje_invalido);
				free(linea_split);
	//			TODO: Definir
	//			1- Podemos poner un break aca y se finaliza el programa
	//				Consecuencia: Tienen que reiniciar el programa
	//				A favor: No hace falta mas validacion
	//			2- Hacemos un gran if con length >= 3 y encerramos toda la demas funciones así
	//				Consecuencia: "Desprolijo"
	//			3- Usamos recursividad
	//				Consecuencia: ??

				iniciar_consola(logger);
			}

		if(string_equals_ignore_case(linea_split[0],comando_help))
		{
			free(linea);
			help(linea_split[1]);
			iniciar_consola(logger);
		}

		char * proceso = linea_split[0];
		char * tipo_mensaje = linea_split[1];

		if(!validar_mensaje(proceso,tipo_mensaje)){
			printf("%s\n", procesos_invalidos);
			liberar_consola(proceso, tipo_mensaje, linea_split);
			iniciar_consola(logger);
		}

		if(!validar_argumentos(tipo_mensaje,linea_split)){

			printf("%s\n", argumento_invalido);
			liberar_consola(proceso, tipo_mensaje, linea_split);
			iniciar_consola(logger);
		}


		if(string_equals_ignore_case(BROKER,proceso))
			ejecutar_broker(tipo_mensaje,linea_split);
		else if(string_equals_ignore_case(TEAM,proceso))
			ejecutar_team(tipo_mensaje,linea_split);
		else if(string_equals_ignore_case(GAMECARD,proceso))
			ejecutar_gamecard(tipo_mensaje,linea_split);
		else
			{
				printf("%s\n", procesos_invalidos);
				liberar_consola(proceso, tipo_mensaje, linea_split);
				iniciar_consola(logger);
			}

	}

}

void liberar_consola(char* proceso, char* mensaje, char** linea_split){
	free(proceso);
	free(mensaje);

	int i = 0;
	while(linea_split[i]!=NULL){
		free(linea_split[i]);
		i++;
	}
	free(linea_split);
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

void ejecutar_broker(char* tipo_mensaje, char** linea_split){
	t_config* config = leer_config(PATH);

	char* ip = config_get_string_value(config,IP_BROKER);
	char* puerto = config_get_string_value(config,PUERTO_BROKER);

	op_code codigo_operacion = codigo_mensaje(tipo_mensaje);

	t_mensaje* mensaje = malloc(sizeof(t_mensaje));

	mensaje -> tipo_mensaje = codigo_operacion;
	mensaje -> parametros = argumentos(linea_split);

	int socket_broker = crear_conexion(ip, puerto);
	enviar_mensaje(mensaje, socket_broker);
}

bool validar_argumentos(char* tipo_mensaje, char** linea_split){

	int cantidad_total = cantidad_argumentos(linea_split);

	if(codigo_mensaje(tipo_mensaje) == APPEARED_POKEMON){

		return cantidad_total == ARGUMENTOS_APPEARED_POKEMON;

	}else if(codigo_mensaje(tipo_mensaje) == NEW_POKEMON){

		return cantidad_total == ARGUMENTOS_NEW_POKEMON;

	}else if(codigo_mensaje(tipo_mensaje) == CATCH_POKEMON){

		return cantidad_total == ARGUMENTOS_CATCH_POKEMON;

	}else if(codigo_mensaje(tipo_mensaje) == CAUGHT_POKEMON){

		return cantidad_total == ARGUMENTOS_CAUGHT_POKEMON;

	}else if(codigo_mensaje(tipo_mensaje) == GET_POKEMON){

		return cantidad_total == ARGUMENTOS_GET_POKEMON;
	}else{
		return false;
	}
}

int cantidad_argumentos (char** linea_split){
	int cantidad = 0;

	while(linea_split[cantidad]!=NULL){
		cantidad++;
	}
	return cantidad - 2; //resto el proceso y el tipo de mensaje, quedan solo los argumentos
}

char** argumentos(char** linea_split){

	int cantidad = cantidad_argumentos(linea_split);

	int i_linea_split = 2; //comienza a cargar la lista a partir del primer argumento, sin contar el proceso y tipo de mensaje

	int j_lista_argumentos = 0; //para iterar en la lista

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

bool validar_mensaje(char* proceso, char*mensaje){

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

void ejecutar_team(char* mensaje,...){
	puts(mensaje);
}

void ejecutar_gamecard(char* mensaje,...){
	puts(mensaje);
}

