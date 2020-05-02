#ifndef UTILSGAMEBOY_H_
#define UTILSGAMEBOY_H_

#include<stdio.h>
#include<stdlib.h>
#include<signal.h>
#include<unistd.h>
#include<sys/socket.h>
#include<netdb.h>
#include<commons/log.h>
#include<commons/string.h>
#include<commons/config.h>
#include<readline/readline.h>
#include<../CommonsMCLDG/utils.h>

//Config
#define IP_BROKER "IP_BROKER"
#define PUERTO_BROKER "PUERTO_BROKER"
#define PROCESOS "PROCESOS"
#define MENSAJES "MENSAJES"
#define FORMATO "FORMATO"

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

//mensajes de errores
#define argumentos_invalidos "Por favor ingrese un formato valido. Para obtener ayuda ingrese el comando help"
#define procesos_invalidos "Por favor ingrese un proceso valido. Para obtener ayuda ingrese el comando help"
#define mensaje_invalido "Por favor ingrese un mensaje valido. Para obtener ayuda ingrese el comando help"
#define argumento_invalido "Argumento invalido\n"

//Defino comandos
//Comandos
#define comando_help "help"
#define comando_exit "exit"

//Mensajes help
#define help_procesos "Los procesos validos son BROKER || TEAM || GAMECARD"
#define help_formato_argumentos "El unico formato valido para ingresar es: [PROCESO] [TIPO_MENSAJE] [ARGUMENTOS]*"
#define help_argumentos "Help admite los siguientes argumentos: \n 1- FORMATO\n 2- PROCESOS "
#define help_mensajes "Las combinaciones de mensajes validas son: ... "


void iniciar_consola(t_log*);
void iniciar_gameboy(void);
void ejecutar_broker(char*,...);
void ejecutar_team(char*,...);
void ejecutar_gamecard(char*,...);
void help(char* mensaje);
bool validar_mensaje(char* proceso, char*mensaje);


#endif /* UTILS_H_ */
