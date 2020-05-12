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

typedef enum{
	MENSAJE=1,
	NEW_POKEMON=2,
	APPEARED_POKEMON=3,
	CATCH_POKEMON=4,
	CAUGHT_POKEMON=5,
	GET_POKEMON=6,
	LOCALIZED_POKEMON=7,
}op_code;


t_log* iniciar_logger(t_config*);
t_config* leer_config(char* proceso);
void terminar_proceso(int, t_log*, t_config*);


#endif /* UTILS_H_ */

