/*
 ============================================================================
 Name        : Team.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include "Team.h"


int main(void) {
	t_config* config;
	t_log* logger;
	t_list* entrenadores;

	iniciarTeam(&config, &logger, &entrenadores);


	terminarTeam(1,logger, config, entrenadores);
	puts("termina");
	return EXIT_SUCCESS;
}
