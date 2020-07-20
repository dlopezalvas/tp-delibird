
#include "utils_broker.h"

extern t_list* multihilos;


int proceso_valido(char*procesos_validos,char* proceso){

	char* s = strstr(procesos_validos,proceso);

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
	NEW_POKEMON_QUEUE_SUSCRIPT = list_create();
	APPEARED_POKEMON_QUEUE_SUSCRIPT = list_create();
	CATCH_POKEMON_QUEUE_SUSCRIPT = list_create();
	CAUGHT_POKEMON_QUEUE_SUSCRIPT = list_create();
	GET_POKEMON_QUEUE_SUSCRIPT = list_create();
}

void terminar_queues(void){
	list_destroy(NEW_POKEMON_QUEUE);
	list_destroy(APPEARED_POKEMON_QUEUE);
	list_destroy(CATCH_POKEMON_QUEUE);
	list_destroy(CAUGHT_POKEMON_QUEUE);
	list_destroy(GET_POKEMON_QUEUE);
	list_destroy(NEW_POKEMON_QUEUE_SUSCRIPT);
	list_destroy(APPEARED_POKEMON_QUEUE_SUSCRIPT);
	list_destroy(CATCH_POKEMON_QUEUE_SUSCRIPT);
	list_destroy(CAUGHT_POKEMON_QUEUE_SUSCRIPT);
	list_destroy(GET_POKEMON_QUEUE_SUSCRIPT);
}

void esperar_cliente(int servidor){

	struct sockaddr_in direccion_cliente;

	unsigned int tam_direccion = sizeof(struct sockaddr_in);

	int cliente = accept (servidor, (void*) &direccion_cliente, &tam_direccion);

	pthread_t hilo;

	list_add(multihilos, &hilo);

	puts(string_itoa(multihilos->elements_count));

	pthread_create(&hilo,NULL,(void*)serve_client,&cliente);
	pthread_detach(hilo);
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

	uint32_t id;
	id = suscribir_mensaje(cod_op,buffer,cliente_fd);
	op_code codigo_operacion = APPEARED_POKEMON;
	t_mensaje* mensaje = malloc(sizeof(t_mensaje));

	char* linea_split = "PIKACHU,2,2";
	mensaje -> tipo_mensaje = codigo_operacion;
	mensaje -> parametros = string_split(linea_split,",");
	puts(mensaje->parametros[0]);

	enviar_mensaje(mensaje,cliente_fd);
	puts("envia mensaje");
	//reenviar id
}

int suscribir_mensaje(int cod_op,void* buffer,int cliente_fd){

//	t_new_pokemon* new_pokemon;
//	t_position_and_name* appeared_pokemon;
//	t_position_and_name* catch_pokemon;
//	t_caught_pokemon* caught_pokemon;
//	t_get_pokemon* get_pokemon;

	t_mensaje_broker* mensaje = malloc(sizeof(t_mensaje_broker));
	mensaje->buffer = buffer;
	mensaje->tipo_mensaje = cod_op;
	mensaje->id = unique_message_id++;
	mensaje->suscriptor = cliente_fd;

	switch (cod_op) {
	case NEW_POKEMON:
		ejecutar_new_pokemon(mensaje);
		break;
	case APPEARED_POKEMON:
		ejecutar_appeared_pokemon(mensaje);
		break;
	case CATCH_POKEMON:
		ejecutar_catch_pokemon(mensaje);
		break;
	case CAUGHT_POKEMON:
		ejecutar_caught_pokemon(mensaje);
		break;
	case GET_POKEMON:
		ejecutar_get_pokemon(mensaje);
		break;
	case LOCALIZED_POKEMON:
		ejecutar_localized_pokemon(mensaje);
		break;
	case SUSCRIPCION:
		ejecutar_suscripcion(mensaje);
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

void enviar_mensaje_broker(int cliente_a_enviar,t_mensaje* mensaje_enviar,char* mensaje_log){
	enviar_mensaje(mensaje_enviar,cliente_a_enviar);
	log_info(logger,mensaje_log);
}

void ejecutar_new_pokemon(t_mensaje_broker* mensaje){
	t_new_pokemon* new_pokemon;
	new_pokemon = deserializar_new_pokemon(mensaje->buffer);
	uint32_t mensaje_id;
	mensaje_id = mensaje->id;
	new_pokemon->id = mensaje_id;
	list_add(NEW_POKEMON_QUEUE,new_pokemon);
	int cliente_a_enviar;
//	cliente_a_enviar = mensaje->suscriptor;
	op_code codigo_operacion = APPEARED_POKEMON;
	t_mensaje* mensaje_enviar = malloc(sizeof(t_mensaje));

	//arma mensaje para enviar
	char* linea_split = "";
	char* nombre;

	nombre = new_pokemon->nombre.nombre;
	int coordenadas_x = new_pokemon->coordenadas.pos_x;
	int coordenadas_y = new_pokemon->coordenadas.pos_y;

	sprintf(linea_split, "%s,%d,%d,%d", nombre ,coordenadas_x,coordenadas_y, mensaje_id);
	mensaje_enviar -> tipo_mensaje = codigo_operacion;
	mensaje_enviar -> parametros = string_split(linea_split,",");
	//

	//envia y loguea mensaje
	char* log_envio_new_pokemon;
	sprintf(log_envio_new_pokemon,"Se envio el mensaje NEW_POKEMON con id: %d, al socket, %d",mensaje_id,cliente_a_enviar);

	void _enviar_mensaje_broker(void* pokemon){
		return enviar_mensaje_broker(cliente_a_enviar, mensaje_enviar,log_envio_new_pokemon);
	}
	list_iterate(NEW_POKEMON_QUEUE_SUSCRIPT, (void*)_enviar_mensaje_broker);
}

void ejecutar_appeared_pokemon(t_mensaje_broker* mensaje){
	t_position_and_name* appeared_pokemon;
	appeared_pokemon = deserializar_position_and_name(mensaje->buffer);
	list_add(APPEARED_POKEMON_QUEUE,appeared_pokemon);
}

//	t_position_and_name* catch_pokemon;
//	t_caught_pokemon* caught_pokemon;
//	t_get_pokemon* get_pokemon;
void ejecutar_catch_pokemon(t_mensaje_broker* mensaje){
	t_position_and_name* catch_pokemon;
	catch_pokemon = deserializar_position_and_name(mensaje->buffer);
	list_add(CATCH_POKEMON_QUEUE,catch_pokemon);

}

void ejecutar_caught_pokemon(t_mensaje_broker* mensaje){
	t_caught_pokemon* caught_pokemon;
	caught_pokemon = deserializar_caught_pokemon(mensaje->buffer);
	list_add(CAUGHT_POKEMON_QUEUE,caught_pokemon);

}

void ejecutar_get_pokemon(t_mensaje_broker* mensaje){
	t_get_pokemon* get_pokemon;
	get_pokemon = deserializar_get_pokemon(mensaje->buffer);
	list_add(GET_POKEMON_QUEUE,get_pokemon);

}

void ejecutar_localized_pokemon(t_mensaje_broker* mensaje){
	t_localized_pokemon* localized_pokemon;
	localized_pokemon = deserializar_localized_pokemon(mensaje->buffer);
	list_add(LOCALIZED_POKEMON_QUEUE,localized_pokemon);
}

void ejecutar_suscripcion(t_mensaje_broker* mensaje){
	int cola;
	void* buffer = mensaje->buffer;

	t_suscripcion* mensaje_suscripcion;
	mensaje_suscripcion = deserializar_suscripcion(buffer);
	cola = mensaje_suscripcion->cola;
	int suscriptor = mensaje->suscriptor;
	switch (cola) {
		case NEW_POKEMON:
			ejecutar_new_pokemon_suscripcion(suscriptor);
			break;
		case APPEARED_POKEMON:
			ejecutar_appeared_pokemon_suscripcion(suscriptor);
			break;
		case CATCH_POKEMON:
			ejecutar_catch_pokemon_suscripcion(suscriptor);
			break;
		case CAUGHT_POKEMON:
			ejecutar_caught_pokemon_suscripcion(suscriptor);
			break;
		case GET_POKEMON:
			ejecutar_get_pokemon_suscripcion(suscriptor);
			break;
		case LOCALIZED_POKEMON:
			ejecutar_localized_pokemon_suscripcion(suscriptor);
			break;
		case 0:
			pthread_exit(NULL);
		case -1:
			pthread_exit(NULL);
		}
}

void ejecutar_new_pokemon_suscripcion(int suscriptor){
	char* log_new_pokemon_suscriptor = "";
	sprintf(log_new_pokemon_suscriptor,"Se suscribio el proceso, %d ,a la cola NEW_POKEMON_QUEUE_SUSCRIPT",suscriptor);
	list_add(NEW_POKEMON_QUEUE_SUSCRIPT,&suscriptor);
	log_info(logger,log_new_pokemon_suscriptor);
}

void ejecutar_appeared_pokemon_suscripcion(int suscriptor){
	char* log_appeared_pokemon_suscriptor= "";
	sprintf(log_appeared_pokemon_suscriptor,"Se suscribio el proceso, %d ,a la cola APPEAREAD_POKEMON",suscriptor);
	list_add(APPEARED_POKEMON_QUEUE_SUSCRIPT,&suscriptor);
	log_info(logger,log_appeared_pokemon_suscriptor);
}

void ejecutar_catch_pokemon_suscripcion(int suscriptor){
	char* log_catch_pokemon_suscriptor= "";
	sprintf(log_catch_pokemon_suscriptor,"Se suscribio el proceso, %d ,a la cola CATCH_POKEMON",suscriptor);
	list_add(CATCH_POKEMON_QUEUE_SUSCRIPT,&suscriptor);
	log_info(logger,log_catch_pokemon_suscriptor);
}

void ejecutar_caught_pokemon_suscripcion(int suscriptor){
	char* log_caught_pokemon_suscriptor= "";
	sprintf(log_caught_pokemon_suscriptor,"Se suscribio el proceso, %d ,a la cola CATCH_POKEMON",suscriptor);
	list_add(CATCH_POKEMON_QUEUE_SUSCRIPT,&suscriptor);
	log_info(logger,log_caught_pokemon_suscriptor);
}

void ejecutar_get_pokemon_suscripcion(int suscriptor){
	char* log_get_pokemon_suscriptor= "";
	sprintf(log_get_pokemon_suscriptor,"Se suscribio el proceso, %d ,a la cola GET_POKEMON",suscriptor);
	list_add(GET_POKEMON_QUEUE_SUSCRIPT,&suscriptor);
	log_info(logger,log_get_pokemon_suscriptor);
}

void ejecutar_localized_pokemon_suscripcion(int suscriptor){
	char* log_localized_pokemon_suscriptor= "";
	sprintf(log_localized_pokemon_suscriptor,"Se suscribio el proceso, %d ,a la cola LOCALIZED_POKEMON",suscriptor);
	list_add(LOCALIZED_POKEMON_QUEUE_SUSCRIPT,&suscriptor);
	log_info(logger,log_localized_pokemon_suscriptor);
}

