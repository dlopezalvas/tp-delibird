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


#define PROCESOS_VALIDOS "PROCESOS_VALIDOS"
#define QUEUES_VALIDAS "QUEUES_VALIDAS"
#define NEW_POKEMON_QUEUE_NAME "NEW_POKEMON"
#define APPEARED_POKEMON_QUEUE_NAME "APPEARED_POKEMON"
#define CATCH_POKEMON_QUEUE_NAME "CATCH_POKEMON"
#define CAUGHT_POKEMON_QUEUE_NAME "CAUGHT_POKEMON"
#define GET_POKEMON_QUEUE_NAME "GET_POKEMON"

t_log* logger;

typedef struct{
  op_code tipo_mensaje;
  void* buffer; //ejemplo: ["PARAM1","PARAM2","PARAM3"]
  int suscriptor;
  uint32_t id;
}t_mensaje_broker;

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



sem_t new_pokemon_sem, appeared_pokemon_sem, catch_pokemon_sem, caught_pokemon_sem,localized_pokemon_sem, get_pokemon_sem, suscripcion_sem;
pthread_mutex_t new_pokemon_mutex,appeared_pokemon_mutex, catch_pokemon_mutex, caught_pokemon_mutex,localized_pokemon_mutex, get_pokemon_mutex, suscripcion_mutex;
pthread_mutex_t new_pokemon_queue_mutex,appeared_pokemon_queue_mutex, catch_pokemon_queue_mutex, caught_pokemon_queue_mutex,localized_pokemon_queue_mutex, get_pokemon_queue_mutex;
pthread_mutex_t
suscripcion_new_queue_mutex,
suscripcion_get_queue_mutex,
suscripcion_caught_queue_mutex,
suscripcion_localized_queue_mutex,
suscripcion_catch_queue_mutex,
suscripcion_appeared_queue_mutex,
multhilos_mutex;

//Recibe un mensaje desde un suscriptor y lo deserializa transofrmando a un t_mensaje
void recibir_mensaje_broker(t_config*);

//
void recibir_mensaje_queue(t_mensaje*,t_config*);

//Valida que un proceso sea existente
int proceso_valido(char*,char*);

//Valida que una queue sea valida
int queue_valida(char*,char*);

//Suscribe un mensaje pasandole el tipo de mensaje y el buffer
int suscribir_mensaje(int cod_op,void* buffer,int client_fd);

//Inicializa todas las queues
void crear_queues();

//Libera la memoria de las queues
void terminar_queues();

//Loguea que un proceso especifico se suscribe a determinada queueu
void log_suscribir_mensaje_queue(char*,char*);

void esperar_cliente(int servidor);
void serve_client(int* socket);
void socketEscucha(char*IP, char* Puerto);
void process_request(int cod_op, int cliente_fd);
//t_config* leer_config(char* proceso);
void enviar_mensaje_broker(int cliente_a_enviar,t_mensaje* mensaje_enviar,char* mensaje_log);
void ejecutar_new_pokemon();
void ejecutar_appeared_pokemon();
void ejecutar_catch_pokemon();
void ejecutar_caught_pokemon();
void ejecutar_get_pokemon();
void ejecutar_localized_pokemon();
void ejecutar_suscripcion();
void ejecutar_new_pokemon_suscripcion(int suscriptor);
void ejecutar_appeared_pokemon_suscripcion(int suscriptor);
void ejecutar_catch_pokemon_suscripcion(int suscriptor);
void ejecutar_caught_pokemon_suscripcion(int suscriptor);
void ejecutar_get_pokemon_suscripcion(int suscriptor);
void ejecutar_localized_pokemon_suscripcion(int suscriptor);

#endif /* UTILS_BROKER_H_ */
