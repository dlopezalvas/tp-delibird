/*
 * Broker.h
 *
 *  Created on: 18 abr. 2020
 *      Author: MCLDG
 */

#ifndef BROKER_H_
#define BROKER_H_

#include<stdio.h>
#include<stdlib.h>
#include<../CommonsMCLDG/utils.h>
#include "utils_broker.h"

void iniciar_broker(t_config**, t_log**);
t_log* iniciar_logger(t_config*);
t_config* leer_config(void);
void terminar_broker(t_log*, t_config*);


#endif /* BROKER_H_ */
