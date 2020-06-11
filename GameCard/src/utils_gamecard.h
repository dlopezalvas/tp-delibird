#ifndef UTILS_H_
#define UTILS_H_

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
#include<../CommonsMCLDG/utils.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define PUNTO_MONTAJE_TALLGRASS "PUNTO_MONTAJE_TALLGRASS"


void crear_tall_grass(t_config* config);
void crear_metadata(t_config* config);



#endif /* UTILS_H_ */
