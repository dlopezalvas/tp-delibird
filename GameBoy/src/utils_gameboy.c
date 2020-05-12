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

		if(string_equals_ignore_case(linea_split[0],comando_help))
		{
			free(linea);
			help(linea_split[1]);
			iniciar_consola(logger);
		}

		if(linea_split[2] == NULL){
			printf("%s\n", argumentos_invalidos);
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

		char * proceso = linea_split[0];
		char * tipo_mensaje = linea_split[1];

		if(!validar_mensaje(proceso,tipo_mensaje)){
			printf("%s\n", mensaje_invalido);
			free(linea_split);
			free(proceso);
			free(tipo_mensaje);
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
				free(linea_split);
				free(proceso);
				free(tipo_mensaje);
				iniciar_consola(logger);
			}

		free(linea);
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

void ejecutar_broker(char* tipo_mensaje, char** linea_split){
//	t_config* config = leer_config(PATH);
//
//	char *ip = config_get_string_value(config,IP_BROKER);
//	char *puerto = config_get_string_value(config,PUERTO_BROKER);
//
//	op_code codigo_operacion = codigo_mensaje(tipo_mensaje);
//
//	int socket_broker = crear_conexion(ip, puerto);
//	enviar_mensaje(mensaje, socket_broker);
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
		printf("%s\n", argumentos_invalidos);
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

