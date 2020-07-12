
#include "utils_broker.h"

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

void log_suscribir_mensaje_queue(char* proceso,char* queue){
	char* mensaje_log = "Proceso: ";
	string_append_with_format(&mensaje_log, "%s", proceso);
	string_append_with_format(&mensaje_log, "%s", "Se suscribio a la cola: ");
	string_append_with_format(&mensaje_log, "%s", queue);
	log_info(logger,mensaje_log);
	free(mensaje_log);
}

void crear_queues(void){
	unique_message_id = 0;
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

	int id = suscribir_mensaje(cod_op,buffer);

	//reenviar id
}

int suscribir_mensaje(int cod_op,void* buffer){
	//TODO: Ver donde meter esa declaracion
	t_new_pokemon* new_pokemon = malloc(sizeof(t_new_pokemon));
	t_position_and_name* appeared_pokemon = malloc(sizeof(t_position_and_name));
	t_position_and_name* catch_pokemon = malloc(sizeof(t_position_and_name));
	t_caught_pokemon* caught_pokemon = malloc(sizeof(t_caught_pokemon));
	t_get_pokemon* get_pokemon = malloc(sizeof(t_get_pokemon));

	t_mensaje_broker* mensaje = malloc(sizeof(t_mensaje_broker));
	mensaje->buffer = buffer;
	mensaje->tipo_mensaje = cod_op;
	mensaje->id = unique_message_id++;

	switch (cod_op) {
	case NEW_POKEMON:
		new_pokemon = deserializar_new_pokemon(buffer);
		list_add(NEW_POKEMON_QUEUE,new_pokemon);
		break;
	case APPEARED_POKEMON:
		appeared_pokemon = deserializar_position_and_name(buffer);
		list_add(APPEARED_POKEMON_QUEUE,appeared_pokemon);
		puts(appeared_pokemon->nombre.nombre);
		break;
	case CATCH_POKEMON:
		catch_pokemon = deserializar_position_and_name(buffer);
		list_add(CATCH_POKEMON_QUEUE,catch_pokemon);
		break;
	case CAUGHT_POKEMON:
		caught_pokemon = deserializar_caught_pokemon(buffer);
		list_add(CAUGHT_POKEMON_QUEUE,caught_pokemon);
		break;
	case GET_POKEMON:
		get_pokemon = deserializar_get_pokemon(buffer);
		puts(get_pokemon->nombre.nombre);
		list_add(GET_POKEMON_QUEUE,get_pokemon);
		break;
	case 0:
		pthread_exit(NULL);
	case -1:
		pthread_exit(NULL);
	}

	return mensaje->id;
}

void socketEscucha(char*ip, char* puerto){
	int servidor = iniciar_servidor(ip,puerto);
	printf("Se creo el socket servidor en el puerto ( %s )", puerto);
	while(1){
		esperar_cliente(servidor);
	}
}
