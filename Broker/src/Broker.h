/*
 * Broker.h
 *
 *  Created on: 18 abr. 2020
 *      Author: MCLDG
 */

#ifndef BROKER_H_
#define BROKER_H_

#include "utils_broker.h"

#define PATH "/home/utnso/workspace/tp-2020-1c-MCLDG/Broker/BROKER.config"

typedef struct{
	t_particion* particion;
	char ocupado;
}t_particion_dump;

void iniciar_broker();
void terminar_broker(t_log*, t_config*);
void dump_cache (int n);
void ver_estado_cache_particiones();

#endif /* BROKER_H_ */
