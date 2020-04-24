#include "utils.h"

void iniciarTeam(void){

	t_config* config = leer_config();
	t_log* logger = iniciar_logger(config);

	char *ip = config_get_string_value(config,IP_BROKER);
	char *puerto = config_get_string_value(config,PUERTO_BROKER);

  	log_info(logger,puerto);
  	log_info(logger,ip);

  	terminarTeam(1,logger,config);
}



t_log* iniciar_logger(t_config* config)
{
	config = leer_config();

	char* nombre_archivo = config_get_string_value(config,"LOG_FILE");
	char* nombre_aplicacion = config_get_string_value(config,"NOMBRE_APLICACION");
	t_log* logger = log_create(nombre_archivo,nombre_aplicacion,0,LOG_LEVEL_INFO);
	return logger;
}

t_config* leer_config(void)
{
//	puts(HELLO2);
	t_config* config = config_create("Team.config");
	return config;
}


void terminarTeam(int conexion, t_log* logger, t_config* config)
{
	config_destroy(config);
	//liberar_conexion(conexion);
	log_info(logger,"-----------LOG END--------");
	log_destroy(logger);
}


