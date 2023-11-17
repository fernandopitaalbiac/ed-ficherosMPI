#include "mpi.h"
#include <stdio.h>
#include <math.h>
#include <stdlib.h>

#define ARRAY_SIZE 10000
#define LIM_SUP 100
#define LIM_INF -100

void makeArray();
int rank, size;
int namelen, resto, dest;
float array[ARRAY_SIZE];
float *A, extremo, extremolocal;
int i, j, porcion;
char processor_name[MPI_MAX_PROCESSOR_NAME];

int main(int argc, char *argv[])
{
	//Inicio de MPI
	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Get_processor_name(processor_name, &namelen);
	printf( "Proceso %d en la máquina %s\n", rank, processor_name);
	
	//Se calcula el tamaño del vector a calcular por cada proceso
	porcion = (int) floor(ARRAY_SIZE/size);
	A = (float *)malloc(porcion*sizeof(float *));

	//Ahora repartimos el array entre todos los procesos dependiendo del numero de procesos y tamaño del array
	if (rank == 0){
		makeArray();    
	   } 
	MPI_Scatter(&array, porcion, MPI_FLOAT, A, porcion, MPI_FLOAT, 0, MPI_COMM_WORLD);

	//Ahora para cada proceso se calcula su mínimo
	extremolocal =100;
	for (i=0; i<porcion; i++){
		if (extremolocal>A[i]) extremolocal = A[i];
	}
	printf(" Proceso %d y su extremo es %f\n\n",rank,extremolocal);
	//Se calcula el mínimo global
	MPI_Reduce(&extremolocal, &extremo, 1, MPI_FLOAT, MPI_MIN, 0, MPI_COMM_WORLD);

	//El proceso raíz muestra por pantalla el resultado
	if (rank==0)
	{
		printf("El máximo: %f \n\n",extremo);
		}

	free(A);
	MPI_Finalize();
	return 0;
}
//Con esta función creamos un array de números aleatorios
void makeArray(){
	for (i = 0; i < ARRAY_SIZE; i++) {
		array[i] = (((LIM_SUP - LIM_INF) * ((double) rand()/(double) RAND_MAX)) + LIM_INF);
	}
}
