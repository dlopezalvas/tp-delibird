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

//Defino diferentes procesos
extern const char *BROKER;
extern const char *TEAM;
extern const char *GAMECARD;
extern const char *GAMEBOY;

//Defino tipo mensajes
extern const char *BROKER_MENSAJES_NEW_POKEMON;
extern const char *BROKER_MENSAJES_APPEARED_POKEMON;
extern const char *BROKER_MENSAJES_CATCH_POKEMON;
extern const char *BROKER_MENSAJES_CAUGHT_POKEMON;
extern const char *BROKER_MENSAJES_GET_POKEMON;
extern const char *TEAM_APPEARED_POKEMON;
extern const char *GAMECARD_NEW_POKEMON;
extern const char *GAMECARD_CATCH_POKEMON;
extern const char *GAMECARD_GET_POKEMON;

//Defino mensajes de errores
extern const char *argumentos_invalidos;
extern const char *argumento_invalidos;
extern const char *procesos_invalidos;

//Defino comandos
extern const char *comando_help;
extern const char *comando_exit;

//Mensajes help
extern const char *help_procesos;
extern const char *help_formato_argumentos;
extern const char *help_argumentos;

void iniciar_consola(t_log*);
void iniciar_gameboy(void);
void ejecutar_broker(char*,...);
void ejecutar_team(char*,...);
void ejecutar_gamecard(char*,...);
t_log* iniciar_logger(t_config*);
t_config* leer_config(void);
void terminar_gameboy(int, t_log*, t_config*);


#endif /* UTILS_H_ */
