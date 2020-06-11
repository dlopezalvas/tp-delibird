
#include "utils_gamecard.h"

//creacion de archivos y directorios (checkpoint 3)

void crear_tall_grass(t_config* config){
	pto_montaje = config_get_string_value(config,PUNTO_MONTAJE_TALLGRASS);

	mkdir(pto_montaje, 0777);

	crear_metadata(pto_montaje);
	crear_bitmap(pto_montaje);
}

void crear_metadata(char* punto_montaje){

	char* path_metadata = string_new();

	string_append(&path_metadata, punto_montaje);
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

void crear_bitmap(char* punto_montaje){

	char* path_bitarray = string_new();

	string_append(&path_bitarray, punto_montaje);
	string_append(&path_bitarray,"/Metadata/Bitmap.bin");

	int blocks = 5192/8; //esto esta para probar, depues hay tirar datos del metadata a alguna estructura

	int bitarray_file = open(path_bitarray, O_RDWR | O_CREAT);  //uso open porque necesito el int para el mmap

	char* mapeo_bitarray = mmap(0, blocks, PROT_WRITE | PROT_READ, MAP_SHARED, bitarray_file, 0);

	bitarray = bitarray_create(mapeo_bitarray, blocks); //MODE DEFAULT

	free(path_bitarray);
}




