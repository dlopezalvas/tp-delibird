#ifndef UTILSCOMMONS_H_
#define UTILSCOMMONS_H_

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

//Config
#define IP_BROKER "IP_BROKER"
#define PUERTO_BROKER "PUERTO_BROKER"


//Defino diferentes procesos
#define BROKER "BROKER"
#define TEAM "TEAM"
#define GAMECARD "GAMECARD"
#define GAMEBOY "GAMEBOY"

//Defino tipo mensajes
#define BROKER_MENSAJES_NEW_POKEMON "NEW_POKEMON"
#define BROKER_MENSAJES_APPEARED_POKEMON "APPEARED_POKEMON"
#define BROKER_MENSAJES_CATCH_POKEMON "CATCH_POKEMON"
#define BROKER_MENSAJES_CAUGHT_POKEMON "CAUGHT_POKEMON"
#define BROKER_MENSAJES_GET_POKEMON "GET_POKEMON"
#define TEAM_APPEARED_POKEMON "APPEARED_POKEMON"
#define GAMECARD_NEW_POKEMON "NEW_POKEMON"
#define GAMECARD_CATCH_POKEMON "CATCH_POKEMON"
#define GAMECARD_GET_POKEMON "GET_POKEMON"

t_log* iniciar_logger(t_config*);
t_config* leer_config(char* proceso);
void terminar_proceso(int, t_log*, t_config*);


#endif /* UTILS_H_ */

