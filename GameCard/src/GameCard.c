/*
 ============================================================================
 Name        : GameCard.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include "GameCard.h"


int main(void) {

	t_config* config = leer_config(PATH);
	t_log* logger = iniciar_logger(config);

	crear_tall_grass(config);
	//iniciar_servidor("127.0.0.3", "5001");

}
