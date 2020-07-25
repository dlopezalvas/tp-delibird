
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
	pthread_mutex_lock(&logger_mutex);
	log_info(logger,mensaje_log);
	pthread_mutex_unlock(&logger_mutex);
	free(mensaje_log);
}

void crear_queues(void){
	unique_message_id = 0;
	NEW_POKEMON_QUEUE = list_create();
	APPEARED_POKEMON_QUEUE = list_create();
	CATCH_POKEMON_QUEUE = list_create();
	CAUGHT_POKEMON_QUEUE = list_create();
	GET_POKEMON_QUEUE = list_create();
	LOCALIZED_POKEMON_QUEUE = list_create();
	NEW_POKEMON_QUEUE_SUSCRIPT = list_create();
	APPEARED_POKEMON_QUEUE_SUSCRIPT = list_create();
	CATCH_POKEMON_QUEUE_SUSCRIPT = list_create();
	CAUGHT_POKEMON_QUEUE_SUSCRIPT = list_create();
	GET_POKEMON_QUEUE_SUSCRIPT = list_create();
	LOCALIZED_POKEMON_QUEUE_SUSCRIPT = list_create();
	NEW_POKEMON_COLA = queue_create();
	APPEARED_POKEMON_COLA = queue_create();
	CATCH_POKEMON_COLA = queue_create();
	CAUGHT_POKEMON_COLA = queue_create();
	GET_POKEMON_COLA = queue_create();
	LOCALIZED_POKEMON_COLA = queue_create();
	SUSCRIPCION_COLA = queue_create();
	pthread_mutex_init(&new_pokemon_mutex,NULL);
	pthread_mutex_init(&appeared_pokemon_mutex,NULL);
	pthread_mutex_init(&catch_pokemon_mutex,NULL);
	pthread_mutex_init(&caught_pokemon_mutex,NULL);
	pthread_mutex_init(&localized_pokemon_mutex,NULL);
	pthread_mutex_init(&get_pokemon_mutex,NULL);
	pthread_mutex_init(&suscripcion_mutex,NULL);
	pthread_mutex_init(&new_pokemon_queue_mutex,NULL);
	pthread_mutex_init(&appeared_pokemon_queue_mutex,NULL);
	pthread_mutex_init(&catch_pokemon_queue_mutex,NULL);
	pthread_mutex_init(&caught_pokemon_queue_mutex,NULL);
	pthread_mutex_init(&localized_pokemon_queue_mutex,NULL);
	pthread_mutex_init(&get_pokemon_queue_mutex,NULL);
	pthread_mutex_init(&suscripcion_new_queue_mutex,NULL);
	pthread_mutex_init(&suscripcion_get_queue_mutex,NULL);
	pthread_mutex_init(&suscripcion_caught_queue_mutex,NULL);
	pthread_mutex_init(&suscripcion_localized_queue_mutex,NULL);
	pthread_mutex_init(&suscripcion_catch_queue_mutex,NULL);
	pthread_mutex_init(&suscripcion_appeared_queue_mutex,NULL);
	pthread_mutex_init(&multhilos_mutex,NULL);
	pthread_mutex_init(&logger_mutex,NULL);
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

	pthread_mutex_lock(&multhilos_mutex);
	list_add(multihilos, &hilo);
	pthread_mutex_unlock(&multhilos_mutex);

//	puts(string_itoa(multihilos->elements_count));

	pthread_create(&hilo,NULL,(void*)serve_client,cliente);
	pthread_detach(hilo);

}

void serve_client(int socket)
{	int rec;
	int cod_op;
	while(1){
		rec = recv(socket, &cod_op, sizeof(op_code), MSG_WAITALL);
		if(rec == -1 || rec == 0 ){
			cod_op = -1;
			pthread_mutex_lock(&logger_mutex);
			log_info(logger,"Se desconecto el proceso con id: %d",socket);
			pthread_mutex_unlock(&logger_mutex);
			pthread_exit(NULL);
		}
		process_request(cod_op, socket);
	}
}

void process_request(int cod_op, int cliente_fd) {
	int size = 0;
	void* buffer = recibir_mensaje(cliente_fd, &size);
	char* log_conexion_proceso = string_new();
	string_append_with_format(&log_conexion_proceso ,"Se conecto el proceso con id: %d",cliente_fd);
	pthread_mutex_lock(&logger_mutex);
	log_info(logger,log_conexion_proceso);
	pthread_mutex_unlock(&logger_mutex);
	suscribir_mensaje(cod_op,buffer,cliente_fd);
//	op_code codigo_operacion = APPEARED_POKEMON;
//	t_mensaje* mensaje = malloc(sizeof(t_mensaje));
//
//	char* linea_split = "PIKACHU,2,2";
//	mensaje -> tipo_mensaje = codigo_operacion;
//	mensaje -> parametros = string_split(linea_split,",");
//	puts(mensaje->parametros[0]);
//
//	enviar_mensaje(mensaje,cliente_fd);

	//reenviar id
}

int suscribir_mensaje(int cod_op,void* buffer,int cliente_fd){

//	t_new_pokemon* new_pokemon;
//	t_position_and_name* appeared_pokemon;
//	t_position_and_name* catch_pokemon;
//	t_caught_pokemon* caught_pokemon;
//	t_get_pokemon* get_pokemon;

	t_mensaje_broker* mensaje = malloc(sizeof(t_mensaje_broker));
	mensaje->buffer = malloc(sizeof(t_buffer));
	mensaje->buffer = buffer;
	mensaje->tipo_mensaje = cod_op;
	pthread_mutex_lock(&unique_id_mutex);
	mensaje->id = unique_message_id++;
	pthread_mutex_unlock(&unique_id_mutex);

	mensaje->suscriptor = cliente_fd;
//	puts("Despues del t_mensaje_broker");
	switch (cod_op) {
	case NEW_POKEMON:
		//ejecutar_new_pokemon(mensaje);
		pthread_mutex_lock(&new_pokemon_mutex);
		queue_push(NEW_POKEMON_COLA,mensaje);
		pthread_mutex_unlock(&new_pokemon_mutex);
		sem_post(&new_pokemon_sem);
		break;
	case APPEARED_POKEMON:

		//ejecutar_appeared_pokemon(mensaje);
		pthread_mutex_lock(&appeared_pokemon_mutex);
		queue_push(APPEARED_POKEMON_COLA,mensaje);
		pthread_mutex_unlock(&appeared_pokemon_mutex);
		sem_post(&appeared_pokemon_sem);
		break;
	case CATCH_POKEMON:

		//ejecutar_catch_pokemon(mensaje);
		pthread_mutex_lock(&catch_pokemon_mutex);
		queue_push(CATCH_POKEMON_COLA,mensaje);
		pthread_mutex_unlock(&catch_pokemon_mutex);
		sem_post(&catch_pokemon_sem);
		break;
	case CAUGHT_POKEMON:

		//ejecutar_caught_pokemon(mensaje);
		pthread_mutex_lock(&caught_pokemon_mutex);
		queue_push(CAUGHT_POKEMON_COLA,mensaje);
		pthread_mutex_unlock(&caught_pokemon_mutex);
		sem_post(&caught_pokemon_sem);
		break;
	case GET_POKEMON:

		//ejecutar_get_pokemon(mensaje);
		pthread_mutex_lock(&get_pokemon_mutex);
		queue_push(GET_POKEMON_COLA,mensaje);
		pthread_mutex_unlock(&get_pokemon_mutex);
		sem_post(&get_pokemon_sem);
		break;
	case LOCALIZED_POKEMON:
		//ejecutar_localized_pokemon(mensaje);
		pthread_mutex_lock(&localized_pokemon_mutex);
		queue_push(LOCALIZED_POKEMON_COLA,mensaje);
		pthread_mutex_unlock(&localized_pokemon_mutex);
		sem_post(&localized_pokemon_sem);
		break;
	case SUSCRIPCION:
		//ejecutar_suscripcion(mensaje);
		pthread_mutex_lock(&suscripcion_mutex);
		queue_push(SUSCRIPCION_COLA,mensaje);
		pthread_mutex_unlock(&suscripcion_mutex);
		sem_post(&suscripcion_sem);
		break;
	case 0:
		puts("mata al hilo");
		pthread_exit(NULL);
	case -1:
		puts("mata al hilo por -1");
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

void enviar_mensaje_broker(int cliente_a_enviar,t_mensaje* mensaje_enviar,uint32_t mensaje_id,char* mensaje_log){
//	puts(string_itoa(cliente_a_enviar));
	enviar_mensaje(mensaje_enviar,cliente_a_enviar);
//	uint32_t id;
//	int _recv;
//	_recv = recv(cliente_a_enviar, &id, sizeof(uint32_t), MSG_WAITALL);
//	if(_recv == 0 || _recv == -1){
//		pthread_mutex_lock(&logger_mutex);
//		log_info(logger,"Fallo al recibir el ack para el mensaje con id %d",mensaje_id);
//		pthread_mutex_unlock(&logger_mutex);
//	}else{
//		string_append_with_format(&mensaje_log ," y recibio ack: %d",id);
//		pthread_mutex_lock(&logger_mutex);
//		log_info(logger,mensaje_log);
//		pthread_mutex_unlock(&logger_mutex);
//	}
	pthread_mutex_lock(&logger_mutex);
	log_info(logger,mensaje_log);
	pthread_mutex_unlock(&logger_mutex);
}



void ejecutar_new_pokemon(){

	while(1){

		sem_wait(&new_pokemon_sem);
		pthread_mutex_lock(&new_pokemon_mutex);
		t_mensaje_broker* mensaje = queue_pop(NEW_POKEMON_COLA);
		pthread_mutex_unlock(&new_pokemon_mutex);

		t_new_pokemon* new_pokemon;
		new_pokemon = deserializar_new_pokemon(mensaje->buffer);
		uint32_t mensaje_id;
		mensaje_id = mensaje->id;
		new_pokemon->id = mensaje_id;

		char* llegada_new_pokemon_log = string_new();
		string_append_with_format(&llegada_new_pokemon_log ,"Llego un mensaje: NEW_POKEMON %s %d %d %d del cliente: %d",new_pokemon->nombre.nombre,new_pokemon->coordenadas.pos_x,new_pokemon->coordenadas.pos_y,new_pokemon->cantidad,new_pokemon->id,mensaje->suscriptor);
		pthread_mutex_lock(&logger_mutex);
		log_info(logger,llegada_new_pokemon_log);
		pthread_mutex_unlock(&logger_mutex);

		pthread_mutex_lock(&new_pokemon_queue_mutex);
		list_add(NEW_POKEMON_QUEUE,new_pokemon);
		pthread_mutex_unlock(&new_pokemon_queue_mutex);
		//Envio ack
		send(mensaje->suscriptor,&mensaje_id,sizeof(uint32_t),0);
		//

		op_code codigo_operacion = NEW_POKEMON;
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
		char* log_envio_new_pokemon = string_new();
		string_append_with_format(&log_envio_new_pokemon,"Se envio el mensaje NEW_POKEMON con id: %d, al socket, %d",mensaje_id,mensaje->suscriptor);

		void _enviar_mensaje_broker(int cliente_a_enviar){
			return enviar_mensaje_broker(cliente_a_enviar, mensaje_enviar,mensaje_id,log_envio_new_pokemon);
		}
		list_iterate(NEW_POKEMON_QUEUE_SUSCRIPT, (void*)_enviar_mensaje_broker);
		free(mensaje->buffer);
		free(mensaje);
	}
}

void ejecutar_appeared_pokemon(){

	while(1){

		sem_wait(&appeared_pokemon_sem);
		pthread_mutex_lock(&appeared_pokemon_mutex);
		t_mensaje_broker* mensaje = queue_pop(APPEARED_POKEMON_COLA);
		pthread_mutex_unlock(&appeared_pokemon_mutex);

		t_position_and_name* appeared_pokemon;
		appeared_pokemon = deserializar_position_and_name(mensaje->buffer);
		uint32_t mensaje_id;
		mensaje_id = mensaje->id;
		appeared_pokemon->id = mensaje_id;

		char* llegada_appeared_pokemon_log = string_new();
		string_append_with_format(&llegada_appeared_pokemon_log ,"Llego un mensaje: APPEARED_POKEMON %s %d %d %d %d del cliente: %d",appeared_pokemon->nombre.nombre,appeared_pokemon->coordenadas.pos_x,appeared_pokemon->coordenadas.pos_y,appeared_pokemon->id,appeared_pokemon->correlation_id,mensaje->suscriptor);
		pthread_mutex_lock(&logger_mutex);
		log_info(logger,llegada_appeared_pokemon_log);
		pthread_mutex_unlock(&logger_mutex);


		pthread_mutex_lock(&appeared_pokemon_queue_mutex);
		list_add(APPEARED_POKEMON_QUEUE,appeared_pokemon);
		pthread_mutex_unlock(&appeared_pokemon_queue_mutex);

		send(mensaje->suscriptor,&mensaje_id,sizeof(uint32_t),0);
		t_mensaje* mensaje_enviar = malloc(sizeof(t_mensaje));
		char* linea_split = string_new();

		string_append_with_format(&linea_split, "%s,%d,%d,%d,%d", appeared_pokemon->nombre.nombre,appeared_pokemon->coordenadas.pos_x,appeared_pokemon->coordenadas.pos_y, appeared_pokemon->id,appeared_pokemon->correlation_id);
		mensaje_enviar -> tipo_mensaje = APPEARED_POKEMON;
		mensaje_enviar -> parametros = string_split(linea_split, ",");

		//envia y loguea mensaje
		char* log_envio_appeared_pokemon = string_new();
		string_append_with_format(&log_envio_appeared_pokemon ,"Se envio el mensaje APPEARED_POKEMON con id: %d, al socket, %d",mensaje_id,mensaje->suscriptor);

		void _enviar_mensaje_broker(int cliente_a_enviar){
			return enviar_mensaje_broker(cliente_a_enviar, mensaje_enviar,mensaje_id,log_envio_appeared_pokemon);
		}
		list_iterate(CATCH_POKEMON_QUEUE_SUSCRIPT, (void*)_enviar_mensaje_broker);send(mensaje->suscriptor,&mensaje_id,sizeof(uint32_t),0);
		free(mensaje->buffer);
		free(mensaje);
	}
}


void ejecutar_catch_pokemon(){

	while(1){

		sem_wait(&catch_pokemon_sem);
		pthread_mutex_lock(&catch_pokemon_mutex);
		t_mensaje_broker* mensaje = queue_pop(CATCH_POKEMON_COLA);
		pthread_mutex_unlock(&catch_pokemon_mutex);

		t_position_and_name* catch_pokemon;
		catch_pokemon = deserializar_position_and_name(mensaje->buffer);
		uint32_t mensaje_id;
		mensaje_id = mensaje->id;
		catch_pokemon->id = mensaje_id;

		char* llegada_catch_pokemon_log = string_new();
		string_append_with_format(&llegada_catch_pokemon_log ,"Llego un mensaje: CATCH_POKEMON %s %d %d %d del cliente: %d",catch_pokemon->nombre.nombre,catch_pokemon->coordenadas.pos_x,catch_pokemon->coordenadas.pos_y,catch_pokemon->id,catch_pokemon->correlation_id,mensaje->suscriptor);
		pthread_mutex_lock(&logger_mutex);
		log_info(logger,llegada_catch_pokemon_log);
		pthread_mutex_unlock(&logger_mutex);

		pthread_mutex_lock(&catch_pokemon_queue_mutex);
		list_add(CATCH_POKEMON_QUEUE,catch_pokemon);
		pthread_mutex_unlock(&catch_pokemon_queue_mutex);

		send(mensaje->suscriptor,&mensaje_id,sizeof(uint32_t),0);
		t_mensaje* mensaje_enviar = malloc(sizeof(t_mensaje));
		char* linea_split = string_new();

		string_append_with_format(&linea_split, "%s,%d,%d,%d,%d", catch_pokemon->nombre.nombre,catch_pokemon->coordenadas.pos_x,catch_pokemon->coordenadas.pos_y, catch_pokemon->id,catch_pokemon->correlation_id);
		mensaje_enviar -> tipo_mensaje = CATCH_POKEMON;
		mensaje_enviar -> parametros = string_split(linea_split, ",");


		char* log_envio_catch_pokemon = string_new();
		string_append_with_format(&log_envio_catch_pokemon ,"Se envio el mensaje CATCH_POKEMON con id: %d, al socket, %d",mensaje_id,mensaje->suscriptor);
		void _enviar_mensaje_broker(int cliente_a_enviar){
			return enviar_mensaje_broker(cliente_a_enviar, mensaje_enviar,mensaje_id,log_envio_catch_pokemon);
		}
		list_iterate(APPEARED_POKEMON_QUEUE_SUSCRIPT, (void*)_enviar_mensaje_broker);
		free(mensaje->buffer);
		free(mensaje);
	}
}

void ejecutar_caught_pokemon(){

	while(1){

		sem_wait(&caught_pokemon_sem);
		pthread_mutex_lock(&caught_pokemon_mutex);
		t_mensaje_broker* mensaje = queue_pop(CAUGHT_POKEMON_COLA);
		pthread_mutex_unlock(&caught_pokemon_mutex);

		t_caught_pokemon* caught_pokemon;
		caught_pokemon = deserializar_caught_pokemon(mensaje->buffer);

		uint32_t mensaje_id;
		mensaje_id = mensaje->id;
		caught_pokemon->id = mensaje_id;

		char* llegada_caught_pokemon_log = string_new();
		string_append_with_format(&llegada_caught_pokemon_log ,"Llego un mensaje: CAUGHT_POKEMON %d %d %d del cliente: %d",caught_pokemon->caught,caught_pokemon->id,caught_pokemon->correlation_id,mensaje->suscriptor);
		pthread_mutex_lock(&logger_mutex);
		log_info(logger,llegada_caught_pokemon_log);
		pthread_mutex_unlock(&logger_mutex);

		pthread_mutex_lock(&caught_pokemon_queue_mutex);
		list_add(CAUGHT_POKEMON_QUEUE,caught_pokemon);
		pthread_mutex_unlock(&caught_pokemon_queue_mutex);
		//Envio ack
		send(mensaje->suscriptor,&mensaje_id,sizeof(uint32_t),0);
		//
		t_mensaje* mensaje_enviar = malloc(sizeof(t_mensaje));
		char* linea_split = string_new();
		string_append_with_format(&linea_split, "%d,%d,%d", caught_pokemon->caught,caught_pokemon->id, caught_pokemon->correlation_id);

		mensaje_enviar -> tipo_mensaje = CAUGHT_POKEMON;
		mensaje_enviar -> parametros = string_split(linea_split, ",");

		char* log_envio_caught_pokemon = string_new();
		string_append_with_format(&log_envio_caught_pokemon ,"Se envio el mensaje CAUGHT_POKEMON con id: %d, al socket, %d",mensaje_id,mensaje->suscriptor);

		void _enviar_mensaje_broker(int cliente_a_enviar){
			return enviar_mensaje_broker(cliente_a_enviar, mensaje_enviar,mensaje_id,log_envio_caught_pokemon );
		}
		list_iterate(CAUGHT_POKEMON_QUEUE_SUSCRIPT, (void*)_enviar_mensaje_broker);
		free(mensaje->buffer);
		free(mensaje);
	}
}

void ejecutar_get_pokemon(){

	while(1){

		sem_wait(&get_pokemon_sem);
		pthread_mutex_lock(&get_pokemon_mutex);
		t_mensaje_broker* mensaje = queue_pop(GET_POKEMON_COLA);
		pthread_mutex_unlock(&get_pokemon_mutex);
//		puts("get pokemonnn");
		t_get_pokemon* get_pokemon;
		get_pokemon = deserializar_get_pokemon(mensaje->buffer);
		uint32_t mensaje_id;
		mensaje_id = mensaje->id;
		get_pokemon->id = mensaje_id;


		char* llegada_get_pokemon_log = string_new();
		string_append_with_format(&llegada_get_pokemon_log  ,"Llego un mensaje: GET_POKEMON %s %d del cliente: %d",get_pokemon->nombre.nombre,get_pokemon->id,mensaje->suscriptor);
		pthread_mutex_lock(&logger_mutex);
		log_info(logger,llegada_get_pokemon_log );
		pthread_mutex_unlock(&logger_mutex);

		pthread_mutex_lock(&get_pokemon_queue_mutex);
		list_add(GET_POKEMON_QUEUE,get_pokemon);
		pthread_mutex_unlock(&get_pokemon_queue_mutex);
		send(mensaje->suscriptor,&mensaje_id,sizeof(uint32_t),0);

		t_mensaje* mensaje_enviar = malloc(sizeof(t_mensaje));
		char* linea_split = string_new();
		string_append_with_format(&linea_split, "%s,%d", get_pokemon->nombre.nombre, mensaje_id);

		mensaje_enviar -> tipo_mensaje = GET_POKEMON;
		mensaje_enviar -> parametros = string_split(linea_split, ",");
		puts("antes de enviar");
		puts(string_itoa(GET_POKEMON_QUEUE_SUSCRIPT->elements_count));

		char* log_envio_get_pokemon = string_new();
		string_append_with_format(&log_envio_get_pokemon ,"Se envio el mensaje GET_POKEMON con id: %d, al socket, %d",mensaje_id,mensaje->suscriptor);

		void _enviar_mensaje_broker(int cliente_a_enviar){
			return enviar_mensaje_broker(cliente_a_enviar, mensaje_enviar,mensaje_id,log_envio_get_pokemon);
		}
		list_iterate(GET_POKEMON_QUEUE_SUSCRIPT, (void*)_enviar_mensaje_broker);
		puts("despues de enviar");
		free(mensaje->buffer);
		free(mensaje);
	}
}

void ejecutar_localized_pokemon(){

	while(1){

		sem_wait(&localized_pokemon_sem);
		pthread_mutex_lock(&localized_pokemon_mutex);
		t_mensaje_broker* mensaje = queue_pop(LOCALIZED_POKEMON_COLA);
		pthread_mutex_unlock(&localized_pokemon_mutex);
		t_localized_pokemon* localized_pokemon;
		localized_pokemon = deserializar_localized_pokemon(mensaje->buffer);
		uint32_t mensaje_id;
		mensaje_id = mensaje->id;
		localized_pokemon->id = mensaje_id;

		pthread_mutex_lock(&localized_pokemon_queue_mutex);
		list_add(LOCALIZED_POKEMON_QUEUE,localized_pokemon);
		pthread_mutex_unlock(&localized_pokemon_queue_mutex);
		send(mensaje->suscriptor,&mensaje_id,sizeof(uint32_t),0);

		t_mensaje* mensaje_enviar = malloc(sizeof(t_mensaje));
		char* linea_split = string_new();
		string_append_with_format(&linea_split, "%s,%d", localized_pokemon->nombre.nombre, localized_pokemon->cantidad);
		coordenadas_pokemon* coord;
		for(int i = 0; i<localized_pokemon->cantidad; i++){
			coord = list_get(localized_pokemon->listaCoordenadas, i);
			string_append_with_format(&linea_split, ",%d,%d", coord->pos_x, coord->pos_y);
		}
		string_append_with_format(&linea_split, ",%d,%d", mensaje_id,localized_pokemon->correlation_id);

		mensaje_enviar -> tipo_mensaje = LOCALIZED_POKEMON;
		mensaje_enviar -> parametros = string_split(linea_split, ",");

		char* llegada_localized_pokemon_log = string_new();
		string_append(&llegada_localized_pokemon_log,linea_split);
		string_append_with_format(&llegada_localized_pokemon_log," Llego LOCALIZED_POKEMON con id de cliente %d",mensaje->suscriptor);
		pthread_mutex_lock(&logger_mutex);
		log_info(logger,llegada_localized_pokemon_log);
		pthread_mutex_unlock(&logger_mutex);

		char* log_envio_localized_pokemon = string_new();
		string_append_with_format(&log_envio_localized_pokemon ,"Se envio el mensaje LOCALIZED_POKEMON con id: %d, al socket, %d",mensaje_id,mensaje->suscriptor);

		void _enviar_mensaje_broker(int cliente_a_enviar){
			return enviar_mensaje_broker(cliente_a_enviar, mensaje_enviar,mensaje_id,log_envio_localized_pokemon );
		}
		list_iterate(LOCALIZED_POKEMON_QUEUE_SUSCRIPT, (void*)_enviar_mensaje_broker);
//		free(mensaje->buffer);
//		free(mensaje);
		puts("codigo mensaje:");
		puts(string_itoa(mensaje_enviar->tipo_mensaje));
	}
}

void ejecutar_suscripcion(){

	while(1){

		sem_wait(&suscripcion_sem);
		pthread_mutex_lock(&suscripcion_mutex);
		t_mensaje_broker* mensaje = queue_pop(SUSCRIPCION_COLA);
		pthread_mutex_unlock(&suscripcion_mutex);
		op_code cola;
		void* buffer = mensaje->buffer;

		t_suscripcion* mensaje_suscripcion;

//		puts("suscripcion mensaje 1");

//		puts(string_itoa(mensaje->suscriptor));

		mensaje_suscripcion = deserializar_suscripcion(buffer);
		cola = mensaje_suscripcion->cola;
		int suscriptor = mensaje->suscriptor;
		char* log_debug_suscripcion = string_new();
		string_append_with_format(&log_debug_suscripcion ,"DEBUG:El cliente %d se suscribio a la cola %d",mensaje->suscriptor ,cola);
		pthread_mutex_lock(&logger_mutex);
		log_info(logger,log_debug_suscripcion);
		pthread_mutex_unlock(&logger_mutex);
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
//		case 0:
//			pthread_exit(NULL);
//		case -1:
//			pthread_exit(NULL);
		}
	}
}

void ejecutar_new_pokemon_suscripcion(int suscriptor){
	char* log_new_pokemon_suscriptor = string_new();
	string_append_with_format(&log_new_pokemon_suscriptor,"Se suscribio el proceso, %d ,a la cola NEW_POKEMON_QUEUE_SUSCRIPT",suscriptor);
	pthread_mutex_lock(&suscripcion_new_queue_mutex);
	list_add(NEW_POKEMON_QUEUE_SUSCRIPT,suscriptor);
	pthread_mutex_unlock(&suscripcion_new_queue_mutex);
	pthread_mutex_lock(&logger_mutex);
	log_info(logger,log_new_pokemon_suscriptor);
	pthread_mutex_unlock(&logger_mutex);
}

void ejecutar_appeared_pokemon_suscripcion(int suscriptor){
	char* log_appeared_pokemon_suscriptor = string_new();
	string_append_with_format(&log_appeared_pokemon_suscriptor,"Se suscribio el proceso, %d ,a la cola APPEAREAD_POKEMON",suscriptor);
	pthread_mutex_lock(&suscripcion_appeared_queue_mutex);
	list_add(APPEARED_POKEMON_QUEUE_SUSCRIPT,suscriptor);
	pthread_mutex_unlock(&suscripcion_appeared_queue_mutex);
	pthread_mutex_lock(&logger_mutex);
	log_info(logger,log_appeared_pokemon_suscriptor);
	pthread_mutex_unlock(&logger_mutex);
}

void ejecutar_catch_pokemon_suscripcion(int suscriptor){

	char* log_catch_pokemon_suscriptor = string_new();
	string_append_with_format(&log_catch_pokemon_suscriptor,"Se suscribio el proceso, %d ,a la cola CATCH_POKEMON",suscriptor);
	pthread_mutex_lock(&suscripcion_catch_queue_mutex);
	list_add(CATCH_POKEMON_QUEUE_SUSCRIPT,suscriptor);
	pthread_mutex_unlock(&suscripcion_catch_queue_mutex);
	pthread_mutex_lock(&logger_mutex);
	log_info(logger,log_catch_pokemon_suscriptor);
	pthread_mutex_unlock(&logger_mutex);
}

void ejecutar_caught_pokemon_suscripcion(int suscriptor){

	char* log_caught_pokemon_suscriptor = string_new();
	string_append_with_format(&log_caught_pokemon_suscriptor,"Se suscribio el proceso, %d ,a la cola CATCH_POKEMON",suscriptor);
	pthread_mutex_lock(&suscripcion_caught_queue_mutex);
	list_add(CATCH_POKEMON_QUEUE_SUSCRIPT,suscriptor);
	pthread_mutex_unlock(&suscripcion_caught_queue_mutex);
	pthread_mutex_lock(&logger_mutex);
	log_info(logger,log_caught_pokemon_suscriptor);
	pthread_mutex_unlock(&logger_mutex);
}

void ejecutar_get_pokemon_suscripcion(int suscriptor){
	char* log_get_pokemon_suscriptor = string_new();
	string_append_with_format(&log_get_pokemon_suscriptor,"Se suscribio el proceso, %d ,a la cola GET_POKEMON",suscriptor);
	pthread_mutex_lock(&suscripcion_get_queue_mutex);
	list_add(GET_POKEMON_QUEUE_SUSCRIPT,suscriptor);
	pthread_mutex_unlock(&suscripcion_get_queue_mutex);
	pthread_mutex_lock(&logger_mutex);
	log_info(logger,log_get_pokemon_suscriptor);
	pthread_mutex_unlock(&logger_mutex);
}

void ejecutar_localized_pokemon_suscripcion(int suscriptor){

	char* log_localized_pokemon_suscriptor = string_new();
	string_append_with_format(&log_localized_pokemon_suscriptor,"Se suscribio el proceso, %d ,a la cola LOCALIZED_POKEMON",suscriptor);
	pthread_mutex_lock(&suscripcion_localized_queue_mutex);
	list_add(LOCALIZED_POKEMON_QUEUE_SUSCRIPT,suscriptor);//Ver si va el & o no
	pthread_mutex_unlock(&suscripcion_localized_queue_mutex);
	pthread_mutex_lock(&logger_mutex);
	log_info(logger,log_localized_pokemon_suscriptor);
	pthread_mutex_unlock(&logger_mutex);
}

//------------MEMORIA------------//
void iniciar_memoria(t_config* config){

	configuracion_cache->tamanio_memoria = config_get_int_value(config, TAMANO_MEMORIA);
	configuracion_cache->tamanio_minimo_p = config_get_int_value(config, TAMANO_MINIMO_PARTICION);

	if(string_equals_ignore_case(config_get_string_value(config, ALGORITMO_MEMORIA), "BS")) configuracion_cache->algoritmo_memoria = BS;
	else if(string_equals_ignore_case(config_get_string_value(config, ALGORITMO_MEMORIA), "PARTICIONES")) configuracion_cache->algoritmo_memoria = PARTICIONES;

	if(string_equals_ignore_case(config_get_string_value(config, ALGORITMO_REEMPLAZO), "FIFO")) configuracion_cache->algoritmo_reemplazo = FIFO;
	else if(string_equals_ignore_case(config_get_string_value(config, ALGORITMO_REEMPLAZO), "LRU")) configuracion_cache->algoritmo_reemplazo = LRU;

	if(string_equals_ignore_case(config_get_string_value(config, ALGORITMO_PARTICION_LIBRE), "FF")) configuracion_cache->algoritmo_part_libre = FIRST_FIT;
	else if(string_equals_ignore_case(config_get_string_value(config, ALGORITMO_PARTICION_LIBRE), "BF")) configuracion_cache->algoritmo_part_libre = BEST_FIT;

	configuracion_cache->frecuencia_compact = config_get_int_value(config, FRECUENCIA_COMPACTACION);

	memoria_cache = malloc(configuracion_cache->tamanio_memoria);

	particiones_libres = list_create();
	t_particion* aux;

	aux->base = memoria_cache;

	aux->tamanio = configuracion_cache->tamanio_memoria;

	list_add(particiones_libres, aux);

	particiones_ocupadas = list_create();

	//claramente faltan semaforos

}

void almacenar_dato(void* datos, int tamanio){
	switch(configuracion_cache->algoritmo_memoria){
	case BS:
	//	almacenar_dato_bs(datos, tamanio);
		break;
	case PARTICIONES:
		almacenar_dato_particiones(datos, tamanio);
		break;
	}
}

void almacenar_dato_particiones(void* datos, int tamanio){

	t_particion* particion_libre;

	switch(configuracion_cache->algoritmo_part_libre){
	case FIRST_FIT:
		particion_libre = particion_libre_ff(tamanio);
		break;
	case BEST_FIT:
		particion_libre = buscar_particion_bf(tamanio);
	}

	asignar_particion(datos, particion_libre, tamanio);
}

void compactar(){
	int offset = memoria_cache;

	int cantidad_ocupadas = list_size(particiones_ocupadas) - 1;

}

t_particion* buscar_particion_ff(int tamanio_a_almacenar){ //falta ordenar lista

	t_particion* particion_libre;

	bool _puede_almacenar(t_particion* particion){
		return particion->tamanio>= tamanio_a_almacenar;
	}

//	ordenar_particiones_libres();

	particion_libre =  list_find(particiones_libres, (void*) _puede_almacenar); //list find agarra el primero que cumpla, asi que el primero que tenga tamanio mayor o igual será

	return particion_libre;
}

void ordenar_particionar_libres(){ //no se si anda esto

	bool _orden(t_particion* particion1, t_particion* particion2){
		return particion1->base < particion2->base;
	}

	list_sort(particiones_libres, _orden);
}

t_particion* particion_libre_ff(int tamanio_a_almacenar){
	t_particion* particion_libre = buscar_particion_ff(tamanio_a_almacenar);

	int contador = 0;

	while(particion_libre == NULL){
		contador++;
		if(contador < configuracion_cache->frecuencia_compact || configuracion_cache->frecuencia_compact == -1){
			//particion_libre = elegir_victima_particiones(tamanio_a_almacenar);
		}else{
			compactar();
			particion_libre = buscar_particion_ff(tamanio_a_almacenar);
			contador = 0;
		}
	}

	return particion_libre;
}

t_particion* buscar_particion_bf(int tamanio_a_almacenar){

	t_particion* particion_libre = malloc(sizeof(t_particion));

	int best_fit = best_fit_index(tamanio_a_almacenar);

	if(best_fit == -1){
		//compactar creo?
	}

	particion_libre = list_get(particiones_libres, best_fit);

	return particion_libre;
}

int best_fit_index(int tamanio_a_almacenar){ //se puede con fold creo

	int best_fit = -1;

	int cantidad_libres = list_size(particiones_libres) - 1; //para que comience por 0

	t_particion* aux = malloc(sizeof(t_particion));
	t_particion* mayor = malloc(sizeof(t_particion));

	for(int i = 0; i < cantidad_libres; i++){
		aux = list_get(particiones_libres, i);
		if(aux->tamanio >= tamanio_a_almacenar){
			if(best_fit == -1){
				best_fit = i;
				mayor = list_get(particiones_libres, i);
			}else if(mayor->tamanio > aux->tamanio){
				best_fit = i;
			}
		}
	}

	free(aux);
	free(mayor);

	return best_fit;

}

void asignar_particion(void* datos, t_particion* particion_libre, int tamanio){

	memcpy(memoria_cache + particion_libre->base, datos, tamanio); //copio a la memoria

	bool _es_la_particion(t_particion* particion){
		return particion == particion_libre;
	}
	list_remove_by_condition(particiones_libres, (void*) _es_la_particion); //esto funca?? saco de la lista la particion (no se si anda haciendo == particion_libre)

	if(particion_libre->tamanio != tamanio){ //si no entro justo (mismo tamanio), significa que queda una nueva particion de menor tamanio libre
		t_particion* particion_nueva = malloc(sizeof(t_particion));
		particion_nueva->base = particion_libre->base + tamanio;
		particion_nueva->tamanio = particion_libre->tamanio - tamanio;
		particion_libre->tamanio = tamanio;

		list_add(particiones_libres, particion_nueva);
	}

	list_add(particiones_ocupadas, particion_libre); //la particion ahora ya no está libre

}

//----------TRANSFORMAR MENSAJES EN VOID*????----------//


t_buffer_broker* deserializar_broker(void* buffer, int size){ //eso hay que probarlo que onda

	t_buffer_broker* buffer_broker = malloc(sizeof(t_buffer_broker));

	int tamanio_mensaje = size - sizeof(uint32_t) * 2;

	buffer_broker->buffer = malloc(sizeof(tamanio_mensaje)); //??????

	int offset = 0;
	memcpy(&buffer_broker->id + offset, buffer, sizeof(uint32_t));
	offset += sizeof(uint32_t);
	memcpy(&buffer_broker->correlation_id + offset, buffer, sizeof(uint32_t));
	offset += sizeof(uint32_t);
	memcpy(buffer_broker->buffer, buffer, tamanio_mensaje);


	return buffer_broker;
}





