
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
		if(rec == -1 || rec == 0)
				cod_op = -1;
		process_request(cod_op, socket);
	}
}

void process_request(int cod_op, int cliente_fd) {
	int size = 0;
	void* buffer = recibir_mensaje(cliente_fd, &size);

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
	pthread_mutex_lock(&logger_mutex);
	mensaje->id = unique_message_id++;
	pthread_mutex_unlock(&logger_mutex);

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
		puts("appeared");
		//ejecutar_appeared_pokemon(mensaje);
		pthread_mutex_lock(&appeared_pokemon_mutex);
		queue_push(APPEARED_POKEMON_COLA,mensaje);
		pthread_mutex_unlock(&appeared_pokemon_mutex);
		sem_post(&appeared_pokemon_sem);
		break;
	case CATCH_POKEMON:
		puts("catch");
		//ejecutar_catch_pokemon(mensaje);
		pthread_mutex_lock(&catch_pokemon_mutex);
		queue_push(CATCH_POKEMON_COLA,mensaje);
		pthread_mutex_unlock(&catch_pokemon_mutex);
		sem_post(&catch_pokemon_sem);
		break;
	case CAUGHT_POKEMON:
		puts("caugh");
		//ejecutar_caught_pokemon(mensaje);
		pthread_mutex_lock(&caught_pokemon_mutex);
		queue_push(CAUGHT_POKEMON_COLA,mensaje);
		pthread_mutex_unlock(&caught_pokemon_mutex);
		sem_post(&caught_pokemon_sem);
		break;
	case GET_POKEMON:
		puts("GETPOKEMON");
		//ejecutar_get_pokemon(mensaje);
		pthread_mutex_lock(&get_pokemon_mutex);
		queue_push(GET_POKEMON_COLA,mensaje);
		pthread_mutex_unlock(&get_pokemon_mutex);
		sem_post(&get_pokemon_sem);
		break;
	case LOCALIZED_POKEMON:
		puts("localized");
		//ejecutar_localized_pokemon(mensaje);
		pthread_mutex_lock(&localized_pokemon_mutex);
		queue_push(LOCALIZED_POKEMON_COLA,mensaje);
		pthread_mutex_unlock(&localized_pokemon_mutex);
		sem_post(&localized_pokemon_sem);
		break;
	case SUSCRIPCION:
		puts("suscripcion");
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

void enviar_mensaje_broker(int cliente_a_enviar,t_mensaje* mensaje_enviar,char* mensaje_log){
//	puts(string_itoa(cliente_a_enviar));
	enviar_mensaje(mensaje_enviar,cliente_a_enviar);
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

		pthread_mutex_lock(&new_pokemon_queue_mutex);
		list_add(NEW_POKEMON_QUEUE,new_pokemon);
		pthread_mutex_unlock(&new_pokemon_queue_mutex);

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
		char* log_envio_new_pokemon = string_new();
		string_append_with_format(&log_envio_new_pokemon,"Se envio el mensaje NEW_POKEMON con id: %d, al socket, %d",mensaje_id,mensaje->suscriptor);

		void _enviar_mensaje_broker(void* cliente_a_enviar){
			return enviar_mensaje_broker(cliente_a_enviar, mensaje_enviar,log_envio_new_pokemon);
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

		pthread_mutex_lock(&appeared_pokemon_queue_mutex);
		list_add(APPEARED_POKEMON_QUEUE,appeared_pokemon);
		pthread_mutex_unlock(&appeared_pokemon_queue_mutex);

		send(mensaje->suscriptor,&mensaje_id,sizeof(uint32_t),0);
		t_mensaje* mensaje_enviar = malloc(sizeof(t_mensaje));
		char* linea_split = string_new();

		string_append_with_format(&linea_split, "%s,%d,%d,%d,%d", appeared_pokemon->nombre.nombre,appeared_pokemon->coordenadas.pos_x,appeared_pokemon->coordenadas.pos_y, appeared_pokemon->id,appeared_pokemon->correlation_id);
		mensaje_enviar -> tipo_mensaje = CATCH_POKEMON;
		mensaje_enviar -> parametros = string_split(linea_split, ",");

		void _enviar_mensaje_broker(void* cliente_a_enviar){
			return enviar_mensaje_broker(cliente_a_enviar, mensaje_enviar,"");
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

		pthread_mutex_lock(&catch_pokemon_queue_mutex);
		list_add(CATCH_POKEMON_QUEUE,catch_pokemon);
		pthread_mutex_unlock(&catch_pokemon_queue_mutex);

		send(mensaje->suscriptor,&mensaje_id,sizeof(uint32_t),0);
		t_mensaje* mensaje_enviar = malloc(sizeof(t_mensaje));
		char* linea_split = string_new();

		string_append_with_format(&linea_split, "%s,%d,%d,%d,%d", catch_pokemon->nombre.nombre,catch_pokemon->coordenadas.pos_x,catch_pokemon->coordenadas.pos_y, catch_pokemon->id,catch_pokemon->correlation_id);
		mensaje_enviar -> tipo_mensaje = APPEARED_POKEMON;
		mensaje_enviar -> parametros = string_split(linea_split, ",");

		void _enviar_mensaje_broker(void* cliente_a_enviar){
			return enviar_mensaje_broker(cliente_a_enviar, mensaje_enviar,"");
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
		pthread_mutex_lock(&caught_pokemon_queue_mutex);
		list_add(CAUGHT_POKEMON_QUEUE,caught_pokemon);
		pthread_mutex_unlock(&caught_pokemon_queue_mutex);
		send(mensaje->suscriptor,&mensaje_id,sizeof(uint32_t),0);
		t_mensaje* mensaje_enviar = malloc(sizeof(t_mensaje));
		char* linea_split = string_new();
		string_append_with_format(&linea_split, "%d,%d,%d", caught_pokemon->caught,caught_pokemon->id, caught_pokemon->correlation_id);

		mensaje_enviar -> tipo_mensaje = CAUGHT_POKEMON;
		mensaje_enviar -> parametros = string_split(linea_split, ",");

		void _enviar_mensaje_broker(void* cliente_a_enviar){
			return enviar_mensaje_broker(cliente_a_enviar, mensaje_enviar,"");
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
		//TODO: Log
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
		void _enviar_mensaje_broker(void* cliente_a_enviar){
			return enviar_mensaje_broker(cliente_a_enviar, mensaje_enviar,"");
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
		puts(linea_split);
		void _enviar_mensaje_broker(void* cliente_a_enviar){
			return enviar_mensaje_broker(cliente_a_enviar, mensaje_enviar,"");
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
		printf("El cliente %d se suscribio a la cola %d\n",mensaje->suscriptor ,cola);
//		puts("suscribir_cola");
//		puts(string_itoa(cola));
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
			puts("LOCALIZED POKEMON SUSCRIPCION");
			ejecutar_localized_pokemon_suscripcion(suscriptor);
			break;
		case 0:
			pthread_exit(NULL);
		case -1:
			pthread_exit(NULL);
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
	puts("Se suscribio al get pokemon el suscriptor");
	puts(string_itoa(suscriptor));
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

