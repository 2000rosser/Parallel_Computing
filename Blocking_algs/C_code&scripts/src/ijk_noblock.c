#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>



int main(int argc, char *argv[]){

  int i,j,k,N,b;
  float num;

//   printf ("Please enter matrix dimension N : ");
//  scanf("%d", &N);
  N=atoi(argv[1]);

//   printf("Please enter a decimal point number to fill out both matrix : ");
//  scanf("%f", &num);
  num=atoi(argv[2]);
  
  double *A, *B, *C;

  A = malloc(N*N*sizeof(double));
  B = malloc(N*N*sizeof(double));
  C = malloc(N*N*sizeof(double));

  for(i=0; i<N*N; i++)
    {
        A[i] = num;
        B[i] = num;
        C[i] = 0.;
    }

    struct timeval tv1, tv2;
    struct timezone tz;
    gettimeofday(&tv1, &tz);

    
    for(int i=0; i<N; i++){
        for(int j=0; j<N; j++){
            for(int k=0; k<N; k++){
                C[(i*N) +j] += A[(i)*N +k] * B[(k*N) +j];
            }
        }
    }
            
        
    

    gettimeofday(&tv2, &tz);
    double elapsed = (double) (tv2.tv_sec-tv1.tv_sec) + (double) (tv2.tv_usec-tv1.tv_usec) * 1.e-6;

//    printf("Multiplication of a %d x %d matrix filled with %f using the UNblocked ijk algorithm\n",N,N, num);
    printf("%lf\n", elapsed);

//    printf("Type 'y' to view the multiplied array\n");
//    char answer[1];
//    scanf("%s",answer);

//    if(answer[0] == 'y'){
//        for(i=0; i<N; i++){
//            for(j=0; j<N; j++){
//                printf("%f\t", C[i*N + j]);
//            }
//            printf("\n");
//        }
//    }
    exit(0);

}


