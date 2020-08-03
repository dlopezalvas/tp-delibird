/*
 ============================================================================
 Name        : Broker.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */


#include <stdio.h>
#include <stdlib.h>
#include "Broker.h"
#define LOG_FILE "LOG_FILE"
#define LOG_NOMBRE_APLICACION "NOMBRE_APLICACION"
#define BROKER_CONFIG "Broker.config"

t_list* multihilos;

int main() {
	signal(SIGUSR1, dump_cache);

	iniciar_broker();
//	recibir_mensaje_broker(config);
//	terminar_broker(logger,config);
	return EXIT_SUCCESS;
}

void iniciar_broker(){
	config = leer_config(PATH);
	logger = iniciar_logger(config);
	char* ip = config_get_string_value(config,IP_BROKER);
	char* puerto = config_get_string_value(config,PUERTO_BROKER);

	sem_init(&new_pokemon_sem,0,0);
	sem_init(&appeared_pokemon_sem,0,0);
	sem_init(&catch_pokemon_sem,0,0);
	sem_init(&caught_pokemon_sem,0,0);
	sem_init(&localized_pokemon_sem,0,0);
	sem_init(&get_pokemon_sem,0,0);
	sem_init(&suscripcion_sem,0,0);
	sem_init(&ack_sem,0,0);

	pthread_t new_pokemon_thread;
	pthread_create(&new_pokemon_thread, NULL, (void*)ejecutar_new_pokemon, NULL);
	pthread_detach(new_pokemon_thread);

	pthread_t appeared_pokemon_thread;
	pthread_create(&appeared_pokemon_thread, NULL, (void*)ejecutar_appeared_pokemon, NULL);
	pthread_detach(appeared_pokemon_thread);

	pthread_t catch_pokemon_thread;
	pthread_create(&catch_pokemon_thread, NULL, (void*)ejecutar_catch_pokemon, NULL);
	pthread_detach(catch_pokemon_thread);

	pthread_t caught_pokemon_thread;
	pthread_create(&caught_pokemon_thread, NULL, (void*)ejecutar_caught_pokemon, NULL);
	pthread_detach(caught_pokemon_thread);

	pthread_t localized_pokemon_thread;
	pthread_create(&localized_pokemon_thread, NULL, (void*)ejecutar_localized_pokemon, NULL);
	pthread_detach(localized_pokemon_thread);

	pthread_t get_pokemon_thread;
	pthread_create(&get_pokemon_thread, NULL, (void*)ejecutar_get_pokemon, NULL);
	pthread_detach(get_pokemon_thread);

	pthread_t suscripcion_thread;
	pthread_create(&suscripcion_thread, NULL, (void*)ejecutar_suscripcion, NULL);
	pthread_detach(suscripcion_thread);

	pthread_t ack_thread;
	pthread_create(&ack_thread, NULL, (void*)ejecutar_ACK, NULL);
	pthread_detach(ack_thread);

	crear_queues();

	multihilos = list_create();

	iniciar_memoria(config);

	socketEscucha(ip,puerto);
}


void terminar_broker(t_log* logger, t_config* config)
{
	config_destroy(config);
	log_destroy(logger);
}

void dump_cache (int n){		//para usarla en cosola kill -SIGUSR1 <pidof Broker>
	switch(n){
	case SIGUSR1:
		switch(configuracion_cache->algoritmo_memoria){
		case BS:
			ver_estado_cache_buddy();
			break;
		case PARTICIONES:
			ver_estado_cache_particiones();
			break;
		}
	break;
	}
}


void ver_estado_cache_particiones(){

	t_list* particiones = list_create();

	void _agregar_bit_ocupado(t_particion* particion){
		t_particion_dump* p_dump = malloc(sizeof(t_particion_dump));
		p_dump->particion = particion;
		p_dump->ocupado = 'X';
		list_add(particiones, p_dump);
	}

	void _agregar_bit_libre(t_particion* particion){
		t_particion_dump* p_dump = malloc(sizeof(t_particion_dump));
		p_dump->particion = particion;
		p_dump->ocupado = 'L';
		list_add(particiones, p_dump);
	}

	list_iterate(particiones_libres, (void*)_agregar_bit_libre);
	list_iterate(particiones_ocupadas, (void*)_agregar_bit_ocupado);

	bool _orden(t_particion_dump* particion1, t_particion_dump* particion2){
			return particion1->particion->base < particion2->particion->base;
		}

	list_sort(particiones, (void*)_orden);

	FILE* dump_cache = fopen("/home/utnso/workspace/tp-2020-1c-MCLDG/Broker/Dump_cache.txt", "a");

	fseek(dump_cache, 0, SEEK_END); //me paro al final

	time_t fecha = time(NULL);

	struct tm *tlocal = localtime(&fecha);
	char output[128];

	strftime(output, 128, "%d/%m/%Y %H:%M:%S", tlocal);

	fprintf(dump_cache, "Dump:%s\n\n", output);

	int i = 1;

	void _imprimir_datos(t_particion_dump* particion){
		char* cola = cola_segun_cod(particion->particion->cola);
		fprintf(dump_cache, "Partición %d: %p - %p [%c]   Size: %db     LRU: %s     COLA: %s     ID: %d\n",
				i, (void*)particion->particion->base, (void*)(particion->particion->base + particion->particion->tamanio), particion->ocupado, particion->particion->tamanio,
				transformar_a_fecha(particion->particion->ultimo_acceso), cola, particion->particion->id_mensaje);
		i++;
	}

	list_iterate(particiones, (void*)_imprimir_datos);

	fprintf(dump_cache, "------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n\n");

	fclose(dump_cache);
}


void ver_estado_cache_buddy(){

	bool _orden(t_particion_buddy* particion1, t_particion_buddy* particion2){
			return particion1->base < particion2->base;
		}
	t_list* dump_buddy = list_create();
	pthread_mutex_lock(&memoria_buddy_mutex);
	list_add_all(dump_buddy,memoria_buddy);
	pthread_mutex_unlock(&memoria_buddy_mutex);
	list_sort(dump_buddy, (void*)_orden);

	FILE* dump_cache = fopen("/home/utnso/workspace/tp-2020-1c-MCLDG/Broker/Dump_cache.txt", "a");

	fseek(dump_cache, 0, SEEK_END); //me paro al final

	time_t fecha = time(NULL);

	struct tm *tlocal = localtime(&fecha);
	char output[128];

	strftime(output, 128, "%d/%m/%Y %H:%M:%S", tlocal);

	fprintf(dump_cache, "Dump:%s\n\n", output);

	int i = 1;

	void _imprimir_datos(t_particion_buddy* particion){
		char* cola = cola_segun_cod(particion->cola);
		fprintf(dump_cache, "Partición %d: %p - %p [%d]   Size: %db     LRU: %s     COLA: %s     ID: %d\n",
				i, (void*)particion->base, (void*)(particion->base + particion->tamanio), particion->ocupado, particion->tamanio,
				transformar_a_fecha(particion->ultimo_acceso), cola, particion->id_mensaje);
		i++;
	}

	list_iterate(dump_buddy, (void*)_imprimir_datos);

	fprintf(dump_cache, "------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n\n");

	fclose(dump_cache);
}

char* transformar_a_fecha(uint32_t nro_fecha){
	char* fecha = string_new();

	time_t aux = (time_t) nro_fecha;
	struct tm *tlocal = localtime(&aux);
	char output[128];

	strftime(output, 128, "%d/%m/%Y %H:%M:%S", tlocal);

	string_append(&fecha, output);

	return fecha;
}

char* cola_segun_cod(op_code cod_op){
	char* cola = string_new();
	switch(cod_op){
	case NEW_POKEMON:
		string_append(&cola, "NEW_POKEMON");
		break;
	case GET_POKEMON:
		string_append(&cola, "GET_POKEMON");
		break;
	case LOCALIZED_POKEMON:
		string_append(&cola, "LOCALIZED_POKEMON");
		break;
	case CATCH_POKEMON:
		string_append(&cola, "CATCH_POKEMON");
		break;
	case CAUGHT_POKEMON:
		string_append(&cola, "CAUGHT_POKEMON");
		break;
	case APPEARED_POKEMON:
		string_append(&cola, "APPEARED_POKEMON");
		break;
	}

	return cola;
}

