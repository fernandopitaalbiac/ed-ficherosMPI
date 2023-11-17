#include <math.h>
#include <stdlib.h>
#include "mpi.h"
#include <stdio.h>

int main (int argc, char *argv[])
{
	int i,size, N, rank, porcion, datos_recep;
	float *A, *B, *V1, *V2;
	int *lsize, *despl, less;
	float local_dot,dot;
	//N = atoi ( argv [2]) ;
	N = 10000;
	
	/* Inicializamos el entorno MPI*/
	MPI_Init(&argc,&argv);
	MPI_Comm_rank(MPI_COMM_WORLD,&rank);
	MPI_Comm_size(MPI_COMM_WORLD,&size);

	/* Unicamente el maestro imprime en pantalla los datos iniciales 
	 * e inicializa los vectores*/
    if (rank == 0)
    {
		printf("## Cálculo del producto escalar de dos vectores \n\n");
        printf("## Total de procesadores:                       %d\n", size);
        printf("## Tamaño de los vectores         %d\n\n", N);

		/*Se asigna memoria y se inicializan los vectores */
		V1 = (float *)malloc(N*sizeof(float *));
		V2 = (float *)malloc(N*sizeof(float *));
		
		/* Inicializamos el array local después del ajuste */
		for(i=0;i<N;i++){
			V1[i]=((float) rand()/(float) RAND_MAX)*10;
			V2[i]=((float) rand()/(float) RAND_MAX)*10;
		}
		
		/*   Se divide el tamaño por los procesadores y se almacena en un array LSIZE 
		* de tamaño size-1 */
		lsize = (int *)malloc(size*sizeof(int));
		despl = (int *)malloc(size*sizeof(int));

		porcion = N%size; //modulo para decidir como repartir
		/*Indicamos cuantos elementos corresponden a cada proceso */
		for (i=0;i<size;i++){
			if(i < porcion){
				lsize[i] = 1+(N-porcion)/size;
				}
			else
				lsize[i]=(N-porcion)/size;
		}        
		
		/*Creamos el vetor de desplazamientos para repartir los datos*/
		despl[0] = 0;
		for (i=1;i<size;i++){
			despl[i] = lsize[i-1]+despl[i-1];
		} 
     }
     /*Indicamos a cada proceso cuantos datos de cada vector va a 
      * procesar, es decir su parte de producto escalar */
     MPI_Scatter(lsize, 1, MPI_INT, &datos_recep, 1, MPI_INT, 0, MPI_COMM_WORLD);
	 /*Se asigna la memoria para los vectores locales */
	 A = (float *)malloc(datos_recep*sizeof(float *));
	 B = (float *)malloc(datos_recep*sizeof(float *));
	 
	 /*Repartimos la parte de los vectores que cada proceso debe 
	  * multiplicar */
	 MPI_Barrier(MPI_COMM_WORLD);
	 MPI_Scatterv(V1, lsize ,despl, MPI_FLOAT, A, datos_recep, MPI_FLOAT, 0, MPI_COMM_WORLD);
	 MPI_Scatterv(V2, lsize ,despl, MPI_FLOAT, B, datos_recep, MPI_FLOAT, 0, MPI_COMM_WORLD);
	 MPI_Barrier(MPI_COMM_WORLD);
	
     /*Cálculo del producto escalar local*/
     local_dot=0.0;
     for(i=0;i<datos_recep;i++)local_dot=local_dot+A[i]*B[i];

     /* Se calcula el producto escalar total en todos los procesos 
      * y todos muestran el resultado obtenido*/
     MPI_Allreduce(&local_dot,&dot,1,MPI_FLOAT,MPI_SUM,MPI_COMM_WORLD);
     printf("Soy el proceso %d y el producto escalar es %f\n",rank,dot);

     /* Finalización del entorno MPI*/ 
	 free(A);
	 free(B);			
     MPI_Finalize();
     return 0;
}
