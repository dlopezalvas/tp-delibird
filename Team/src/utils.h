#ifndef UTILS_H_
#define UTILS_H_

#include<stdio.h>
#include<stdlib.h>
#include<signal.h>
#include<unistd.h>
#include<sys/socket.h>
#include<netdb.h>
#include<commons/log.h>
#include<commons/string.h>
#include<commons/config.h>

//Config
#define IP_BROKER "IP_BROKER"
#define PUERTO_BROKER "PUERTO_BROKER"


void iniciarTeam(void);
t_log* iniciar_logger(t_config*);
t_config* leer_config(void);
void terminarTeam(int, t_log*, t_config*);


#endif /* UTILS_H_ */
