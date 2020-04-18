
#include "utils.h"


t_log* iniciar_logger(void)
{
	t_config* config;
	config = leer_config();


	t_log* logger = log_create(config_get_string_value(config,"ARCHIVO_LOG"),config_get_string_value(config,"NOMBRE_APLICACION"),true,LOG_LEVEL_INFO);
	return logger;
}

t_config* leer_config(void)
{
	t_config* config = config_create("GameBoy.config");
	return config;
}

void terminar_programa(int conexion, t_log* logger, t_config* config)
{
	config_destroy(config);
	log_info(logger,"-----------LOG END--------");
	//liberar_conexion(conexion);
	log_destroy(logger);
}





