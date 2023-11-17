#include "stdio.h"
#include "stdlib.h"
#include "time.h"
#include "math.h"
#include "mpi.h"

int main(int argc, char *argv[])
{
    int i,j,k,m , N_TAREAS , N_MEZCLA ,N =3, size, rank;
    int * matriz_a ,* matriz_b ,* matriz_c ,* mezcla ,* mezcla_resultados ,* fila_columna ;
    int elemento [3];
    MPI_Comm everyone;
    MPI_Comm padre;
    
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_get_parent(&padre);

    if (argc > 1)
	N = atoi (argv[1]);
    
    N_TAREAS = N*N;
    
    if (padre ==MPI_COMM_NULL) {
	/* Proceso estático. Únicamente realiza el spawn */ 
	MPI_Comm_spawn(argv[0], &argv[1], N_TAREAS, MPI_INFO_NULL, 0, MPI_COMM_WORLD, &everyone, MPI_ERRCODES_IGNORE);
    }
    else if ((padre !=MPI_COMM_NULL)&&(rank==0)) {
	/* Proceso dinámico de rango 0 */
	N_MEZCLA = 2*(N*N*N + N*N);
	
	matriz_a = (int *) malloc (N * N * sizeof ( int ));
	matriz_b = (int *) malloc (N * N * sizeof ( int ));
	matriz_c = (int *) malloc (N * N * sizeof ( int ));
	mezcla = (int *) malloc ( N_MEZCLA * sizeof ( int ));
	mezcla_resultados = (int *) malloc (3 * N * N * sizeof (int ));
	fila_columna = ( int *) malloc (2 * (1+N) * sizeof ( int ));
	
	for (i =0; i<N*N; i ++) {
	    matriz_a [i] = ( rand () /( float ) RAND_MAX ) * 10;
	    matriz_b [i] = ( rand () /( float ) RAND_MAX ) * 10;
	}
	
	k = 0;
	for (i =0; i<N; i++) {
	    for (j =0; j<N; j++) {
		mezcla [k] = i +1;
		k ++;
		for (m =0; m<N; m++) {
		    mezcla [k] = matriz_a [i*N+m];
		    k ++;
		}
		mezcla [k] = j +1;
		k ++;
		for (m =0; m<N; m++) {
		    mezcla [k] = matriz_b [m*N+j];
		    k ++;
		}
	    }
	}
	
	printf (" Matriz A:\n");
	for (i =0; i<N; i++) {
	    for (j =0; j<N; j++) printf ("\t%d", matriz_a [i*N+j]);
	    printf ("\n");
	}
	printf (" Matriz B:\n");
	for (i =0; i<N;i ++) {
	    for (j =0; j<N; j++) printf ("\t%d", matriz_b [i*N+j]);
	    printf ("\n");
	}
	
	
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	if ( size != N_TAREAS ) {
	    printf("No se han creado todas las tareas:%d .\n",size);
	    //return -1;
	}
	else {
	    printf ("Ejecutándose %d tareas para la múltiplicación de matrices.\n", size) ;
	}
	
	MPI_Barrier(MPI_COMM_WORLD);
	
	MPI_Scatter(mezcla, 2*(1+ N), MPI_INT, fila_columna, 2*(1+ N), MPI_INT, 0, MPI_COMM_WORLD);
	
	elemento [0] = fila_columna [0];
	elemento [1] = fila_columna [N +1];
	elemento [2] = 0;
	for (i=0 ; i<N; i ++) elemento [2] += fila_columna [i +1] *fila_columna [i +2+ N];
	
	MPI_Barrier(MPI_COMM_WORLD);
	MPI_Gather(&elemento, 3, MPI_INT, mezcla_resultados, 3, MPI_INT, 0, MPI_COMM_WORLD);
	
	for (i =0; i <3* N*N; i=i +3) {
	    j = N*( mezcla_resultados [i] -1) + ( mezcla_resultados [i +1] -1) ;
	    matriz_c [j] = mezcla_resultados [i +2];
	}
	
	printf (" Matriz C:\n");
	for (i =0; i<N; i++) {
	    for (j =0; j<N; j++) printf ("\t%d", matriz_c [i*N+j]);
	    printf ("\n");
	}
    } else {
	/* Resto de procesos dinámicos */
	
	N_TAREAS = N*N;
	N_MEZCLA = 2 * (N*N*N + N*N);
	fila_columna = ( int *) malloc (2*(1+ N) * sizeof ( int ));
	
	MPI_Barrier(MPI_COMM_WORLD);
	MPI_Scatter(mezcla, 2*(1+ N), MPI_INT, fila_columna, 2*(1+ N), MPI_INT, 0, MPI_COMM_WORLD);
	
	elemento [0] = fila_columna [0];
	elemento [1] = fila_columna [N +1];
	elemento [2] = 0;
	for (i=0 ; i<N; i ++) elemento [2] += fila_columna [i +1] *fila_columna [i +2+ N];
	
	MPI_Barrier(MPI_COMM_WORLD);
	MPI_Gather(&elemento, 3, MPI_INT, mezcla_resultados, 3, MPI_INT, 0, MPI_COMM_WORLD);
    }
    MPI_Finalize();
    return 0;
}
