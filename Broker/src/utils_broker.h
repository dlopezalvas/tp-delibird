#ifndef UTILS_BROKER_H_
#define UTILS_BROKER_H_

#include<stdio.h>
#include<stdlib.h>
#include<signal.h>
#include<unistd.h>
#include<netdb.h>
#include<string.h>
#include<commons/log.h>
#include<commons/string.h>
#include<commons/config.h>
#include<commons/collections/node.h>
#include<commons/collections/list.h>
#include<commons/string.h>
#include<stdbool.h>
#include<../CommonsMCLDG/utils.h>
#include<../CommonsMCLDG/socket.h>
#include<semaphore.h>
#include<pthread.h>
#include <commons/collections/queue.h>
#include <time.h>


#define PROCESOS_VALIDOS "PROCESOS_VALIDOS"
#define QUEUES_VALIDAS "QUEUES_VALIDAS"
#define NEW_POKEMON_QUEUE_NAME "NEW_POKEMON"
#define APPEARED_POKEMON_QUEUE_NAME "APPEARED_POKEMON"
#define CATCH_POKEMON_QUEUE_NAME "CATCH_POKEMON"
#define CAUGHT_POKEMON_QUEUE_NAME "CAUGHT_POKEMON"
#define GET_POKEMON_QUEUE_NAME "GET_POKEMON"


#define TAMANO_MEMORIA "TAMANO_MEMORIA"
#define TAMANO_MINIMO_PARTICION "TAMANO_MINIMO_PARTICION"
#define ALGORITMO_MEMORIA "ALGORITMO_MEMORIA"
#define ALGORITMO_REEMPLAZO "ALGORITMO_REEMPLAZO"
#define ALGORITMO_PARTICION_LIBRE "ALGORITMO_PARTICION_LIBRE"
#define FRECUENCIA_COMPACTACION "FRECUENCIA_COMPACTACION"

t_log* logger;
t_config* config;

typedef struct{
  void* buffer;
  int suscriptor;
}t_mensaje_broker;

typedef struct{
	uint32_t id;
	uint32_t correlation_id;
	uint32_t tamanio;
	void* buffer;
}t_buffer_broker;


typedef struct{
	op_code cola;
	uint32_t base;
	uint32_t tamanio;
	uint32_t id_mensaje;
	uint32_t ultimo_acceso;
}t_particion;

typedef struct{
	uint32_t base;
	int tamanio;
	uint32_t id;
	uint32_t id_mensaje;
	op_code cola;
	bool ocupado;
	uint32_t ultimo_acceso;
}t_particion_buddy;

typedef struct{
	uint32_t id;
	uint32_t correlation_id;
	t_particion* particion;
	t_particion_buddy* particion_buddy;
}t_bloque_broker;


typedef enum{
	BS = 1,
	PARTICIONES = 2,
}algtm_memoria;

typedef enum{
	FIFO = 1,
	LRU = 2,
}algtm_reemplazo;

typedef enum{
	FIRST_FIT = 1,
	BEST_FIT = 2,
}algtm_part_libre;

typedef struct{
	int tamanio_memoria;
	int tamanio_minimo_p;
	algtm_memoria algoritmo_memoria;
	algtm_reemplazo algoritmo_reemplazo;
	algtm_part_libre algoritmo_part_libre;
	int frecuencia_compact;
}t_config_cache;

void* memoria_cache;

t_list* particiones_libres;
t_list* particiones_ocupadas;

t_config_cache* configuracion_cache;

//Inicializo las colas como listas para tener mas flexibilidad a la hora de manejarlas
t_list* NEW_POKEMON_QUEUE;
t_list* APPEARED_POKEMON_QUEUE;
t_list* CATCH_POKEMON_QUEUE;
t_list* CAUGHT_POKEMON_QUEUE;
t_list* GET_POKEMON_QUEUE;
t_list* LOCALIZED_POKEMON_QUEUE;
t_list* NEW_POKEMON_QUEUE_SUSCRIPT;
t_list* APPEARED_POKEMON_QUEUE_SUSCRIPT;
t_list* CATCH_POKEMON_QUEUE_SUSCRIPT;
t_list* CAUGHT_POKEMON_QUEUE_SUSCRIPT;
t_list* GET_POKEMON_QUEUE_SUSCRIPT;
t_list* LOCALIZED_POKEMON_QUEUE_SUSCRIPT;
uint32_t unique_message_id;

t_queue* NEW_POKEMON_COLA;
t_queue* APPEARED_POKEMON_COLA;
t_queue* CATCH_POKEMON_COLA;
t_queue* CAUGHT_POKEMON_COLA;
t_queue* GET_POKEMON_COLA;
t_queue* LOCALIZED_POKEMON_COLA;
t_queue* SUSCRIPCION_COLA;
t_queue* ACK_COLA;


sem_t new_pokemon_sem, appeared_pokemon_sem, catch_pokemon_sem, caught_pokemon_sem,localized_pokemon_sem, get_pokemon_sem, suscripcion_sem, ack_sem;
pthread_mutex_t new_pokemon_mutex,appeared_pokemon_mutex, catch_pokemon_mutex, caught_pokemon_mutex,localized_pokemon_mutex, get_pokemon_mutex, suscripcion_mutex;
pthread_mutex_t new_pokemon_queue_mutex,appeared_pokemon_queue_mutex, catch_pokemon_queue_mutex, caught_pokemon_queue_mutex,localized_pokemon_queue_mutex, get_pokemon_queue_mutex, ack_queue_mutex;
pthread_mutex_t
suscripcion_new_queue_mutex,
suscripcion_get_queue_mutex,
suscripcion_caught_queue_mutex,
suscripcion_localized_queue_mutex,
suscripcion_catch_queue_mutex,
suscripcion_appeared_queue_mutex,
multhilos_mutex,
logger_mutex,
unique_id_mutex,
memoria_buddy_mutex,
buddy_id_mutex;

//Recibe un mensaje desde un suscriptor y lo deserializa transofrmando a un t_mensaje
void recibir_mensaje_broker(t_config*);

//
void recibir_mensaje_queue(t_mensaje*,t_config*);

//Valida que un proceso sea existente
int proceso_valido(char*,char*);

//Valida que una queue sea valida
int queue_valida(char*,char*);

//Suscribe un mensaje pasandole el tipo de mensaje y el buffer
int suscribir_mensaje(int cod_op,void* buffer,int cliente_fd,uint32_t size);

//Inicializa todas las queues
void crear_queues();

//Libera la memoria de las queues
void terminar_queues();

//Loguea que un proceso especifico se suscribe a determinada queueu
void log_suscribir_mensaje_queue(char*,char*);

void esperar_cliente(int servidor);
void serve_client(int socket);
void socketEscucha(char*IP, char* Puerto);
void process_request(int cod_op, int cliente_fd);
//t_config* leer_config(char* proceso);
void enviar_mensaje_broker(int cliente_a_enviar,void* a_enviar,int bytes);
void ejecutar_new_pokemon();
void ejecutar_appeared_pokemon();
void ejecutar_catch_pokemon();
void ejecutar_caught_pokemon();
void ejecutar_get_pokemon();
void ejecutar_localized_pokemon();
void ejecutar_suscripcion();
void ejecutar_ACK();
void ejecutar_new_pokemon_suscripcion(int suscriptor);
void ejecutar_appeared_pokemon_suscripcion(int suscriptor);
void ejecutar_catch_pokemon_suscripcion(int suscriptor);
void ejecutar_caught_pokemon_suscripcion(int suscriptor);
void ejecutar_get_pokemon_suscripcion(int suscriptor);
void ejecutar_localized_pokemon_suscripcion(int suscriptor);


bool es_mensaje_respuesta(op_code cod_op);
t_buffer_broker* deserializar_broker_vuelta(void* buffer, uint32_t size);
t_paquete* preparar_mensaje_a_enviar(t_bloque_broker* bloque_broker, op_code codigo_operacion);

void ordenar_particiones_libres();
void iniciar_memoria(t_config* config);
void* almacenar_dato(void* datos, int tamanio, op_code codigo_op, uint32_t id);
void asignar_particion(void* datos, t_particion* particion_libre, int tamanio, op_code codigo_op, uint32_t id);
t_particion* almacenar_dato_particiones(void* datos, int tamanio, op_code codigo_op, uint32_t id);
t_particion* buscar_particion_ff(int tamanio_a_almacenar);
t_particion* buscar_particion_bf(int tamanio_a_almacenar);
t_particion* particion_libre_bf(int tamanio_a_almacenar);
t_particion* particion_libre_ff(int tamanio_a_almacenar);

void consolidar(t_particion* particion_liberada);
void compactar();

t_particion* elegir_victima_particiones(int tamanio_a_almacenar);
t_particion* elegir_victima_particiones_LRU(int tamanio_a_almacenar);
void eliminar_particion(t_particion* particion_a_liberar);


t_buffer_broker* deserializar_broker_ida(void* buffer, uint32_t size);

//buddy

t_list* memoria_buddy;

uint32_t buddy_id;

t_particion_buddy* almacenar_datos_buddy(void* datos, int tamanio,op_code cod_op,uint32_t id_mensaje);
void eleccion_victima_fifo_buddy(int tamanio);
bool validar_condicion_fifo_buddy(t_particion_buddy* bloque_buddy,t_particion_buddy* bloque_buddy_old);
void eleccion_victima_lru_buddy(int tamanio);
void eleccion_particion_buddy(t_particion_buddy* bloque_buddy,t_particion_buddy* bloque_buddy_particion,void* datos,int tamanio);
void asignar_particion_buddy(t_particion_buddy* bloque_buddy_particion, void* datos, int tamanio,op_code cod_op,uint32_t id_mensaje);
bool sort_byId_memoria_buddy(t_particion_buddy* bloque_buddy,t_particion_buddy* bloque_buddy2);
bool mismo_id_buddy(t_particion_buddy* bloque_buddy,uint32_t id_viejo);
t_particion_buddy* generar_particion_buddy(t_particion_buddy* bloque_buddy);
bool validar_condicion_buddy(t_particion_buddy* bloque_buddy,int tamanio);
bool eleccion_victima_fifo_a_eliminar(t_particion_buddy* bloque_buddy, int tamanio);
void consolidar_buddy(t_particion_buddy* bloque_buddy_old,t_list* lista_fifo_buddy);
bool remove_by_id(t_particion_buddy* bloque_buddy,uint32_t id_remover);
t_particion_buddy* encontrar_y_consolidar_buddy(t_particion_buddy* bloque_buddy,t_particion_buddy* bloque_buddy_old);
bool sort_by_acceso_memoria_buddy(t_particion_buddy* bloque_buddy,t_particion_buddy* bloque_buddy2);
t_particion_buddy* eleccion_particion_asignada_buddy(void* datos,int tamanio);
bool encontrar_bloque_valido_buddy(t_particion_buddy* bloque_buddy,int tamanio);
bool ordenar_menor_a_mayor(t_particion_buddy* bloque_buddy,t_particion_buddy* bloque_buddy2);

//


#endif /* UTILS_BROKER_H_ */
