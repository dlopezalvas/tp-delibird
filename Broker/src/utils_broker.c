
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

//	if(proceso_valido(procesos_validos,mensaje -> proceso))
//		printf("%s","El proceso no esta autorizado para suscribirse o mandar mensajes");

	if(queue_valida(queues_validas,mensaje -> queue)) // @suppress("Field cannot be resolved")
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
	char* mensaje_queue = mensaje -> queue; // @suppress("Field cannot be resolved")
	//log_suscribir_mensaje_queue(mensaje -> process,mensaje_queue);

	if(string_equals_ignore_case(mensaje_queue,NEW_POKEMON_QUEUE_NAME))
		{
			list_add(NEW_POKEMON_QUEUE,mensaje);

		}
	else if(string_equals_ignore_case(mensaje_queue,APPEARED_POKEMON_QUEUE_NAME))
		list_add(APPEARED_POKEMON_QUEUE,mensaje);
	else if(string_equals_ignore_case(mensaje_queue,CATCH_POKEMON_QUEUE_NAME))
		list_add(CATCH_POKEMON_QUEUE,mensaje);
	else if(string_equals_ignore_case(mensaje_queue,CAUGHT_POKEMON_QUEUE_NAME))
		list_add(CAUGHT_POKEMON_QUEUE,mensaje);
	else if(string_equals_ignore_case(mensaje_queue,GET_POKEMON_QUEUE_NAME))
		list_add(GET_POKEMON_QUEUE,mensaje);

	free(mensaje_queue);
}

void log_suscribir_mensaje_queue(char* proceso,char* queue){
	char* mensaje_log = "Proceso: ";
	string_append_with_format(&mensaje_log, "%s", proceso);
	string_append_with_format(&mensaje_log, "%s", "Se suscribio a la cola: ");
	string_append_with_format(&mensaje_log, "%s", queue);
	log_info(logger,mensaje_log);
	free(mensaje_log);
}

void crear_queues(void){
	NEW_POKEMON_QUEUE = list_create();
	APPEARED_POKEMON_QUEUE = list_create();
	CATCH_POKEMON_QUEUE = list_create();
	CAUGHT_POKEMON_QUEUE = list_create();
	GET_POKEMON_QUEUE = list_create();
}

void terminar_queues(void){
	list_destroy(NEW_POKEMON_QUEUE);
	list_destroy(APPEARED_POKEMON_QUEUE);
	list_destroy(CATCH_POKEMON_QUEUE);
	list_destroy(CAUGHT_POKEMON_QUEUE);
	list_destroy(GET_POKEMON_QUEUE);
}

void esperar_cliente(int servidor){
	pthread_t thread;
	struct sockaddr_in direccion_cliente;

	unsigned int tam_direccion = sizeof(struct sockaddr_in);

	int cliente = accept (servidor, (void*) &direccion_cliente, &tam_direccion);

	pthread_create(&thread,NULL,(void*)serve_client,&cliente);
	pthread_detach(thread);
}

void serve_client(int* socket)
{
	int cod_op;
	if(recv(*socket, &cod_op, sizeof(int), MSG_WAITALL) == -1)
		cod_op = -1;
	process_request(cod_op, *socket);
}

void process_request(int cod_op, int cliente_fd) {
	int size = 0;
	void* buffer = recibir_mensaje(cliente_fd, &size);
	//int id = recv(cliente_fd, &id,sizeof(int),0);

	switch (cod_op) {
	case NEW_POKEMON:
		t_new_pokemon* new_pokemon = malloc(sizeof(t_new_pokemon));
		new_pokemon = deserializar_new_pokemon(buffer);
		puts(new_pokemon->nombre.nombre);
		break;
	case APPEARED_POKEMON:
		t_position_and_name* appeared_pokemon = malloc(sizeof(t_position_and_name));
		appeared_pokemon  = deserializar_position_and_name(buffer);
		puts(appeared_pokemon ->nombre.nombre);
		break;
	case CATCH_POKEMON:
		t_position_and_name* catch_pokemon = malloc(sizeof(t_position_and_name));
		catch_pokemon = deserializar_catch_pokemon(buffer);
		puts(catch_pokemon ->nombre.nombre);

		break;
	case CAUGHT_POKEMON:
		t_caught_pokemon* caught_pokemon = malloc(sizeof(t_caught_pokemon));
		caught_pokemon = deserializar_position_and_name(buffer);
		puts(caught_pokemon ->caught);
		break;
	case GET_POKEMON:
		t_get_pokemon* get_pokemon = malloc(sizeof(t_get_pokemon));
		get_pokemon* = deserializar_position_and_name(buffer);
		puts(get_pokemon->nombre.nombre);
		break;
	case 0:
		pthread_exit(NULL);
	case -1:
		pthread_exit(NULL);
	}
}

void socketEscucha(char*ip, char* puerto){
	int servidor = iniciar_servidor(ip,puerto);
	printf("Se creo el socket servidor en el puerto ( %s )", puerto);
	while(1){
		esperar_cliente(servidor);
	}
}
