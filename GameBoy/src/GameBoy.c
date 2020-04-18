/*
 ============================================================================
 Name        : GameBoy.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include "GameBoy.h"



int main(void) {

//	 char * linea;
//	  while(1) {
//	    linea = readline(">");
//	    if(linea)
//	      add_history(linea);
//
//	    if("BROKER" == linea){
//
//	    	puts("BROKER");
//
//	    }
//
//	    if(!strncmp(linea, "exit", 4)) {
//	       free(linea);
//	       break;
//	    }
//	    printf("%s\n", linea);
//	    free(linea);
//	  }


	  char* ip;
	  	char* puerto;

	  	t_log* logger;
	  	t_config* config;

	  	logger = iniciar_logger();

	  	log_info(logger,"-----------LOG START--------");
	  	//Loggear "soy un log"

	  	config = leer_config();


	  	/*---------------------------------------------------PARTE 3-------------------------------------------------------------*/

	  	//antes de continuar, tenemos que asegurarnos que el servidor esté corriendo porque lo necesitaremos para lo que sigue.
	  	ip = config_get_string_value(config,"IP");
	  	puerto = config_get_string_value(config,"PUERTO");
	  	log_info(logger,puerto);
	  	log_info(logger,ip);

	  log_destroy(logger);
	  config_destroy(config);

}





