
#include "utils.h"
#define IP_BROKER "IP_BROKER"
#define PUERTO_BROKER "PUERTO_BROKER"

//Procesos
const char *BROKER = "BROKER";
const char *TEAM = "TEAM";
const char *GAMECARD = "GAMECARD";
const char *GAMEBOY = "GAMEBOY";
//
//mensajes errores
const char *argumentos_invalidos = "Por favor ingrese un formato valido. Para obtener ayuda ingrese el comando help";
const char *procesos_invalidos = "Por favor ingrese un proceso valido. Para obtener ayuda ingrese el comando help";
const char *argumento_invalido= "Argumento invalido\n";

//Comandos
const char *comando_help = "help";
const char *comando_exit = "exit";
//

//Mensajes help
const char *help_procesos = "Los procesos validos son BROKER || TEAM || GAMECARD";
const char *help_formato_argumentos = "El unico formato valido para ingresar es: [PROCESO] [TIPO_MENSAJE] [ARGUMENTOS]*";
const char *help_argumentos = "Help admite los siguientes argumentos: \n 1- FORMATO\n 2- PROCESOS ";

void iniciar_gameboy(void){

	t_config* config = leer_config();
	t_log* logger = iniciar_logger(config);

	char *ip = config_get_string_value(config,IP_BROKER);
	char *puerto = config_get_string_value(config,PUERTO_BROKER);

  	log_info(logger,puerto);
  	log_info(logger,ip);

  	iniciar_consola(logger);
  	terminar_gameboy(1,logger,config);
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
		char * mensaje = linea_split[1];

		if(string_equals_ignore_case(BROKER,proceso))
			ejecutar_broker(mensaje,linea_split);
		else if(string_equals_ignore_case(TEAM,proceso))
			ejecutar_team(mensaje,linea_split);
		else if(string_equals_ignore_case(GAMECARD,proceso))
			ejecutar_gamecard(mensaje,linea_split);
		else
			{
				printf("%s\n", procesos_invalidos);
				free(linea_split);
				free(proceso);
				free(mensaje);
				iniciar_consola(logger);
			}

		free(linea);
	}

}

t_log* iniciar_logger(t_config* config)
{
	config = leer_config();

	char* nombre_archivo = config_get_string_value(config,"ARCHIVO_LOG");
	char* nombre_aplicacion = config_get_string_value(config,"NOMBRE_APLICACION");
	int log_consola = config_get_int_value(config,"LOG_CONSOLA");
	t_log* logger = log_create(nombre_archivo,nombre_aplicacion,log_consola,LOG_LEVEL_INFO);
	return logger;
}

t_config* leer_config(void)
{
//	puts(HELLO2);
	t_config* config = config_create("GameBoy.config");
	return config;
}

void help(char* mensaje){

	if(mensaje == NULL){
		puts(help_argumentos);
		return;
	}

	if(string_equals_ignore_case(mensaje,"PROCESOS"))
		puts(help_procesos);
	else if(string_equals_ignore_case(mensaje,"FORMATO"))
		puts(help_formato_argumentos);
	else
		{
			puts(argumento_invalido);
			puts(help_argumentos);
		}
}
void terminar_gameboy(int conexion, t_log* logger, t_config* config)
{
	config_destroy(config);
	//liberar_conexion(conexion);
	log_info(logger,"-----------LOG END--------");
	log_destroy(logger);
}

void ejecutar_broker(char* mensaje,...){
	puts(mensaje);
}

void ejecutar_team(char* mensaje,...){
	puts(mensaje);
}

void ejecutar_gamecard(char* mensaje,...){
	puts(mensaje);
}


