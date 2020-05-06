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
}  t_mensaje;

//TODO: Recibe un mensaje desde un suscriptor y lo deserializa transofrmando a un t_mensaje
void recibir_mensaje_broker(void);

void recibir_mensaje_queue(t_mensaje*);
int proceso_valido(char*);
int queue_valido(char*);
void suscribir_mensaje_queue(t_mensaje*);


#endif /* UTILS_BROKER_H_ */
