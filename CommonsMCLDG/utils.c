#include "utils.h"

void iniciar_proceso(char* proceso, t_config* config){

	t_log* logger = iniciar_logger(config);

	char *ip = config_get_string_value(config,IP_BROKER);
	char *puerto = config_get_string_value(config,PUERTO_BROKER);

  	log_info(logger,puerto);
  	log_info(logger,ip);

}


t_log* iniciar_logger(t_config* config)
{
	char* nombre_archivo = config_get_string_value(config,"LOG_FILE");
	char* nombre_aplicacion = config_get_string_value(config,"NOMBRE_APLICACION");
	int log_consola = config_get_int_value(config,"LOG_CONSOLA");

	config = leer_config(nombre_aplicacion);

	t_log* logger = log_create(nombre_archivo,nombre_aplicacion,log_consola,LOG_LEVEL_INFO);
	return logger;
}

t_config* leer_config(char* proceso)
{
	char *config_file = string_new();
//	string_append(&config_file, "../" );
	string_append(&config_file, proceso);
	string_append(&config_file, ".config");
	t_config* config = config_create(config_file);
	return config;


}

void terminar_proceso(int conexion, t_log* logger, t_config* config)
{
	config_destroy(config);
	//liberar_conexion(conexion);
	log_info(logger,"-----------LOG END--------");
	log_destroy(logger);
}


