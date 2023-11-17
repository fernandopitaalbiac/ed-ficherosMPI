#include <math.h>
#include "mpi.h"
#include <stdio.h>

int main(argc,argv)
int argc; char *argv[];
{
	int i,size, rank, testigo, proximo, anterior;
	MPI_Status status;

	MPI_Init (&argc, &argv); /* comienza MPI */
	MPI_Comm_rank (MPI_COMM_WORLD, &rank); /* obtiene el rango */
	MPI_Comm_size (MPI_COMM_WORLD, &size); /* número de procesos */
	testigo = 0; /* iniciamos el testigo */

	//Indicamos que proceso nos precede y cúal nos sigue
	if (rank == 0) {
		proximo = rank+1;
		anterior = size-1;
	}
	else if(rank==size-1){
		proximo = 0 ;
		anterior = rank-1;
	}
	else
	{
		proximo = rank+1;
		anterior = rank-1;
	}
	printf("## Soy el proceso: %d de %d\n", rank,size);
	printf("## Mi testigo ahora vale: %d\n", testigo);
	printf("## El proceso anterior es %d\n", anterior);
	printf("## El proceso siguiente es %d\n\n", proximo);
	
	/* Éstas serían unas hipóteticas acciones propias que realizaría
	 * el proceso correspondiente*/
	
	//acciones1();

	if (rank == 0) {
		//El maestro incrementa el testigo
		testigo =1;
		printf("## Soy el proceso:    %d\n", rank);
		printf("## Mi testigo ahora es:         %d\n\n", testigo);
		//Se manda el testigo al siguiente proceso
		MPI_Send(&testigo, 1, MPI_INT, proximo, 0, MPI_COMM_WORLD);
		
		/*Otras posibles acciones a realizar mientras espera a que le 
		sea devuelto el testigo por su proceso anterior*/
		//acciones2();

		/* se recibe el testigo con valor == 1, que
		significa que todos los procesos terminaron acciones1() */
		MPI_Recv(&testigo, 1, MPI_INT,anterior,0,MPI_COMM_WORLD, &status);
		
		//Nuevas acciones a realizar
		//acciones3();

		testigo = 2; /* para avisar a todos que hemos terminado */
		printf("## Soy el proceso:    %d\n", rank);
		printf("## Mi testigo ahora es:         %d\n\n", testigo);
		
		//Se manda al siguiente proceso
		MPI_Send(&testigo, 1, MPI_INT, proximo, 0, MPI_COMM_WORLD);
		//Se espera del último proceso para finalizar el programa
		MPI_Recv(&testigo, 1, MPI_INT,anterior,0,MPI_COMM_WORLD, &status);
		printf("## Se cerro finalmente el anillo");
		} 
	else {
		/* recibe el testigo con valor == 1 y se manda al siguiete proceso*/
		MPI_Recv(&testigo, 1, MPI_INT,anterior,0,MPI_COMM_WORLD, &status);
		MPI_Send(&testigo, 1, MPI_INT,proximo, 0, MPI_COMM_WORLD);

		printf("## Soy el proceso:    %d de %d\n", rank, size);
		printf("## Mi testigo ahora es:         %d\n\n", testigo);
		
		//Posibles acciones a realizar
		//acciones2();

		/* recibe el testigo con valor == 2 */
		MPI_Recv(&testigo, 1, MPI_INT,anterior,0,MPI_COMM_WORLD, &status);
		
		//Realiza sus últimas operaciones antes de mandar el testigo y finalizar
		//acciones3();
				
		printf("## Soy el proceso:    %d de %d\n", rank, size);
		printf("## TMi testigo ahora es:         %d\n\n", testigo);
		MPI_Send(&testigo, 1, MPI_INT, proximo, 0, MPI_COMM_WORLD);

		}
		
	MPI_Finalize();
	return 0;
}
