
#include "utils_gamecard.h"

//creacion de archivos y directorios (checkpoint 3)

void crear_tall_grass(t_config* config){
	char* punto_de_montaje = config_get_string_value(config,PUNTO_MONTAJE_TALLGRASS);

	mkdir(punto_de_montaje, 0777);

	crear_metadata(config);
}

void crear_metadata(t_config* config){

	char* path_metadata = config_get_string_value(config,PUNTO_MONTAJE_TALLGRASS);

	string_append(&path_metadata,"/Metadata");
	mkdir(path_metadata, 0777);

	string_append(&path_metadata, "/Metadata.bin");

	FILE * metadata = fopen(path_metadata, "w+");

	fprintf(metadata, "BLOCK_SIZE=64\n");
	fprintf(metadata, "BLOCKS=5192\n");
	fprintf(metadata, "MAGIC_NUMBER=TALL_GRASS\n");

	fclose(metadata);

	free(path_metadata);

}





