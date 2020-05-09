
#include "utils_broker.h"

void recibir_mensaje_broker(t_config* config){
	log_info(logger,"probando log 1 2 3");

	//recibe un mensaje, tiene que deserializarlo y llamar a recibir_mensaje_queue
	//generar el id unico (Ver el tipo, quiza tenga que ser un uuid)
	//pasandole el tipo t_mensaje* y config*
};

void recibir_mensaje_queue(t_mensaje* mensaje,t_config* config){
	char *procesos_validos = config_get_string_value(config,PROCESOS_VALIDOS);
	char *queues_validas = config_get_string_value(config,QUEUES_VALIDAS);

	if(proceso_valido(procesos_validos,mensaje -> proceso))
		printf("%s","El proceso no esta autorizado para suscribirse o mandar mensajes");

	if(queue_valida(queues_validas,mensaje -> queue))
		printf("%s","La queue no existe");

	//suscribir_mensaje_queue
}

int proceso_valido(char*procesos_validos,char* proceso){

	char* s = strstr(procesos_validos,proceso);

	if(s != NULL) return 1;
	return 0;
}

int queue_valida(char*queues_validas,char* queue){

	char* s = strstr(queues_validas, queue);

	if(s != NULL) return 1;
	return 0;
}

void suscribir_mensaje_queue(t_mensaje* mensaje){
	log_suscribir_mensaje_queue(mensaje -> proceso,mensaje -> queue);

	//pushear mensaje a la cola correspondiente
}

void log_suscribir_mensaje_queue(char* proceso,char* queue){
	char* mensaje_log = "Proceso: ";
	string_append_with_format(&mensaje_log, "%s", proceso);
	string_append_with_format(&mensaje_log, "%s", 'Se suscribio a la cola: ');
	string_append_with_format(&mensaje_log, "%s", queue);
	log_info(logger,mensaje_log);
	free(mensaje_log);
}

void crear_queues(void){
	NEW_POKEMON_QUEUE = list_create();
	APPEARED_POKEMON = list_create();
	CATCH_POKEMON = list_create();
	CAUGHT_POKEMON = list_create();
	GET_POKEMON = list_create();
}

void terminar_queues(void){
	list_destroy(NEW_POKEMON_QUEUE);
	list_destroy(APPEARED_POKEMON);
	list_destroy(CATCH_POKEMON);
	list_destroy(CAUGHT_POKEMON);
	list_destroy(GET_POKEMON);
}
