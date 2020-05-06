#ifndef UTILS_BROKER_H_
#define UTILS_BROKER_H_

#include<stdio.h>
#include<stdlib.h>
#include<signal.h>
#include<unistd.h>
#include<sys/socket.h>
#include<netdb.h>
#include<string.h>
#include<commons/log.h>
#include<commons/string.h>
#include<commons/config.h>
#include<../CommonsMCLDG/utils.h>


typedef struct{
  char *proceso;
  char **parametros; //ejemplo: ["PARAM1","PARAM2","PARAM3"]
  char *queue;
  uint32_t *id;
}  t_mensaje;

//Recibe un mensaje desde un suscriptor y lo deserializa transofrmando a un t_mensaje
void recibir_mensaje_broker(void);

//
void recibir_mensaje_queue(t_mensaje*);

//Valida que un proceso sea existente
int proceso_valido(char*);

//Valida que una queue sea valida
int queue_valida(char*);

//Suscribe un mensaje de tipo t_mensaje a una queue
void suscribir_mensaje_queue(t_mensaje*);

//Inicializa todas las queues
void crear_queues();

//Libera la memoria de las queues
void terminar_queues();


#endif /* UTILS_BROKER_H_ */
