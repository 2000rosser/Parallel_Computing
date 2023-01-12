#include <stdio.h>
#include <stdlib.h>
#include <cblas.h>
#include <omp.h>
#include <math.h>
#include <malloc.h>
#include <sys/time.h>

static omp_lock_t norm_lock;

void multiply(int col, int n, double *A, double *B, double *C){
    double Alpha, Beta;
    Alpha = 1;
    Beta = 0;
    ATL_dgemm(CblasNoTrans, CblasNoTrans, n, col, n, Alpha,
                A, n, B, n, Beta, C, n);
    
}

double get_norm(int col, int n, double *C){
    double norm=0;
    double total;
    
    
    for(int i=0; i<col; i++){
        total=0;
        for(int j=0; j<n; j++){
            double x=C[i*n + j];
            
            total += fabs(C[i*n + j]);
            
        }
        omp_init_lock(&norm_lock);
        if(total > norm){
            norm = total;
        }
        omp_destroy_lock(&norm_lock);
    }
   
    return norm;
}

int main(){
    printf("N\tparallel time\tserial time\tnorm\t\tnorm\n");
    for(int n=2000; n<=4000; n+=100){
        double *A, *B, *C;

        A = malloc(n*n*sizeof(double));
        B = malloc(n*n*sizeof(double));
        C = malloc(n*n*sizeof(double));

        for(int i=0; i<n*n; i++){
            A[i] = 5;
            B[i] = 5;
        }

        omp_set_num_threads(8);
        

        struct timeval tv1, tv2;
        struct timezone tz;
        gettimeofday(&tv1, &tz);

        int num_threads=8;
        int i;

        #pragma omp parallel private(i) shared(A, B, C, n)
        #pragma omp for schedule(guided,1)
        for(i=0; i<num_threads; i++){
            int part = n/num_threads;
            int col = i * part;
            if(i == 7){
                part = n-(part*i);
            }
            multiply(part, n, A, &B[col*n], &C[col*n]);
        }
        

        #pragma omp barrier
        double norm;
        double norm_temp=0;
        #pragma omp parallel private(i) shared(C, n)
        #pragma omp for schedule(guided,1)
        for(i=0; i<num_threads; i++){
            int part = n/num_threads;
            int col = i * part;
            if(i == 7){
                part = n-(part*i);
            }
            norm_temp = get_norm(part, n,&C[col*n]);
            if(norm_temp>norm){
                norm=norm_temp;
            }
        }

        gettimeofday(&tv2, &tz);
        double time_parallel = (double)(tv2.tv_sec - tv1.tv_sec) + (double)(tv2.tv_usec - tv1.tv_usec) * 1.e-6;
        


        gettimeofday(&tv1, &tz);
        multiply(n, n, A, B, C);

        double serialnorm=0;

        serialnorm = get_norm(n, n ,C);

        gettimeofday(&tv2, &tz);
        double serial_time_parallel = (double)(tv2.tv_sec - tv1.tv_sec) + (double)(tv2.tv_usec - tv1.tv_usec) * 1.e-6;

        
        printf("%d\t%f\t%f\t%f\t%f\n",n,time_parallel, serial_time_parallel, norm, serialnorm);
                

    }
    return (EXIT_SUCCESS);
        
    }
    
