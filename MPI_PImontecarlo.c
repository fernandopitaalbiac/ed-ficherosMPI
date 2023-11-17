#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <sys/time.h>
#include "mpi.h"
 
int main (int argc, char *argv[])
{
        //Declaramos las variables que se usarán en el programa
        int rank, size, limite_inf, limite_sup, puntos_interiores, i, N;
        double pi_local, pi_total, x, y, z, error, start_time, end_time, PI;
        struct timeval stime;
		//Se asignan los valores inciales para el cálculo
		N = 10000;
		//N = atoi ( argv [2]) ;
		PI = 3.141592;
        limite_inf   = -1;
        limite_sup   = 1;
        puntos_interiores = 0;

        //Rutina de inicio del entorno MPI
        MPI_Init(&argc, &argv);

        /*Obtenemos el rango del proceso del proceso y el número de 
         * procesos dentro del MPI_COMM_WORLD */
        MPI_Comm_rank(MPI_COMM_WORLD, &rank);
        MPI_Comm_size(MPI_COMM_WORLD, &size);

        //El maestro imprime los datos iniciales
        if (rank == 0)
        {
                printf("## Cálculo de PI por el método Montecarlo en MPI \n\n");
                printf("## Número de procesadores usados:                       %d\n", size);
                printf("## Número de valores aleatorios generados por procesador:         %d\n\n", N);
 
                start_time = MPI_Wtime();
        }

        //Cálculamos un número aleatorio dentro de la región de interés
        for (i = 1; i <= N; i++)
        {
			x=(((limite_sup - limite_inf) * ((double) rand()/(double) RAND_MAX)) + limite_inf);
            y=(((limite_sup - limite_inf) * ((double) rand()/(double) RAND_MAX)) + limite_inf);

                z = pow(x, 2) + pow(y, 2);
				//Comprobamos si está dentro del círculo unidad
                if (z <= 1.0)
                        puntos_interiores+=1;
        }

        /*Cálculamos el valor de PI dentro del proceso local y se 
         * mandan mediante una operación de reducción y suna el valor
         * al proceso maestro el valor total calculado de PI */
        pi_local = 4 * (double)puntos_interiores/(double)N;
        MPI_Reduce(&pi_local, &pi_total, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

        /* Se muestran los resultados en el proceso maestro */
        if (rank == 0)
        {
                pi_total = pi_total / size;
                error = fabs((pi_total - PI)/PI) * 100;

                end_time = MPI_Wtime();

                printf("## Valor real de PI:          %11.10f\n", PI);
                printf("## Valor calculado por el método:     %11.10f\n", pi_total);
                printf("## Diferencia entre ambos (error de cálculo):               %10.8f\n", error);
                printf("## Tiempo empleado:     %10.8f\n\n", end_time - start_time);
        }

        //Finalizamos el entorno MPI
        MPI_Finalize();

        return 0;
}

