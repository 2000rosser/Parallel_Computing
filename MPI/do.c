#include <stdio.h>
#include "mpi.h"
#include <sys/time.h>

int main(int argc, char *argv[])
{
  
  MPI_Status status;
  int rank,p,i,j,k;
  double start_time, end_time;

  int N=100;

  double** a;
  double** b;
  double** c; 

  a =(double **)malloc(N*sizeof(double *));
  a[0] = (double *)malloc(N*N*sizeof(double));
  if(!a)
  {
    printf("memory failed \n");
    exit(1);
  }
  for(i=1; i<N; i++)
  {
    a[i] = a[0]+i*N;
    if (!a[i])
    {
	  printf("memory failed \n");
	  exit(1);
	}
  }

  b =(double **)malloc(N*sizeof(double *));
  b[0] = (double *)malloc(N*N*sizeof(double));
  if(!b)
  {
    printf("memory failed \n");
    exit(1);
  }
  for(i=1; i<N; i++)
  {
    b[i] = b[0]+i*N;
    if (!b[i])
    {
        printf("memory failed \n");
        exit(1);
    }
  }

  c =(double **)malloc(N*sizeof(double *));
  c[0] = (double *)malloc(N*N*sizeof(double));
  if(!c)
    {
      printf("memory failed \n");
      exit(1);
    }
  for(i=1; i<N; i++)
    {
      c[i] = c[0]+i*N;
      if (!c[i])
	{
	  printf("memory failed \n");
	  exit(1);
	}
    } 

  for(int i=0; i<N; i++){
    for(int j=0; j<N; j++){
        a[i][j] = 5.;
        b[i][j] = 5.;
    }
  }

  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &p);


  if (!rank)
         start_time = MPI_Wtime(); 

  if (rank == 0) 
  {
      for (i=1; i<p; i++)
      {
          MPI_Send(&a[i * N / p][0], N * N / p, MPI_INT, i, 0, MPI_COMM_WORLD);
          MPI_Send(b, N * N, MPI_INT, i, 0, MPI_COMM_WORLD);
      }
  }
  else
  {
      MPI_Recv(&a[rank * N / p][0], N * N / p, MPI_INT, i, 0, MPI_COMM_WORLD, 0);
      MPI_Recv(b, N * N, MPI_INT, i, 0, MPI_COMM_WORLD, 0);

  }
  for(i=rank * N / p; i<(rank+1) * N/p; ++i)
  {
      for (j=0; j < N; j++)
      {
          c[i][j] = 0;
          for (k=0; k<N; k++)
          {
              c[i][j] += a[i][k] * b[k][j];
          }
      }
  }

  if (rank != 0 )
  {
      MPI_Send(&c[(rank)*N/p][0], N*N/p, MPI_INT, 0, 0, MPI_COMM_WORLD);
  }
  else
  {
      for (i=1; i<p; i++)
      {
          MPI_Recv(&c[i*N/p][0], N*N/p, MPI_INT, i, 0, MPI_COMM_WORLD, 0);
      }
  }
  
  if (!rank)
         end_time = MPI_Wtime();

  MPI_Barrier(MPI_COMM_WORLD);

  if (rank == 0)
  {
      printf("\nMPI = %f\n",  end_time-start_time);
    //   for (i=0; i<N; i++) {
    //       printf("\n\t| ");
    //       for (j=0; j<N; j++)
    //           printf("%2f ", c[i][j]);
    //       printf("|");
    //   }
  }


  MPI_Finalize();
  struct timeval tv1, tv2;
  struct timezone tz;
  gettimeofday(&tv1, &tz);

  for(int i = 0; i < N; i++) {
        for(int j = 0; j < N; j++){
        c[i][j] = 0;
            for(int k = 0; k < N; k++) {
				c[i][j] +=  a[i][k] * b[k][j];
            }
        }
  }
  gettimeofday(&tv2, &tz);
  double time_parallel = (double)(tv2.tv_sec - tv1.tv_sec) + (double)(tv2.tv_usec - tv1.tv_usec) * 1.e-6;

  printf("Serial = %f\n\n", time_parallel);
  return 0;
}