#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <math.h>
#include <sys/time.h>
#include <cblas.h>

typedef struct {
    double *A;
    double *B;
    double *C;
    
    int m;
    int n;
    int k;
    double *max_value;
    pthread_mutex_t *mutex;
} multiply_getnorm;

struct timeval tv1, tv2;
struct timezone tz;

double calculations(int threads, int n, double *A, double *B, double *C);
void *multiply(void *arg);
void *getnorm(void *arg);


int main() {    

    for (int n=100; n<=1000; n+=100) {
        for (int threads = 1; threads <= n; threads += threads) {

            double *A = (double *) malloc(n*n*sizeof(double));
            double *B = (double *) malloc(n*n*sizeof(double));
            double *C = (double *) malloc(n*n*sizeof(double));

            
            for (int i = 0; i < n * n; i++) {
                A[i] = 5;
                B[i] = 5;
                C[i] = 0.0;
            }
            
            gettimeofday(&tv1, &tz);
            double norm = calculations(threads, n, A, B, C);
            gettimeofday(&tv2, &tz);
            double time = (double) (tv2.tv_sec - tv1.tv_sec) + (double) (tv2.tv_usec - tv1.tv_usec) * 1.e-6;
            printf("%d\t%d\t%f\t%f\n", n, threads, norm, time);
            free(A);
            free(B);
            free(C);
        }
    }
    exit(0);
}

double calculations(int threads, int n, double *A, double *B, double *C) {
    
    pthread_t *working_thread;
    multiply_getnorm *multiply_getnorm_vals;
    void *status;
    pthread_mutex_t *mutex;

    if (threads > n || threads < 1) {
        printf("invalid");
        return -1;
    } 

    double norm = 0.;
    mutex = malloc(sizeof (pthread_mutex_t));
    working_thread = malloc(threads * sizeof (pthread_t));
    multiply_getnorm_vals = malloc(threads * sizeof (multiply_getnorm));
    pthread_mutex_init(mutex, NULL);

    for (int i=0; i<threads; i++) {
        int columns = n/threads;

        int col_num = i*columns;

        if (i == threads-1) {
            columns = n-(i*columns);
        }

        multiply_getnorm_vals[i].A = A;
        multiply_getnorm_vals[i].B = &B[col_num];
        multiply_getnorm_vals[i].C = &C[col_num];
        multiply_getnorm_vals[i].m = n;
        multiply_getnorm_vals[i].n = columns;
        multiply_getnorm_vals[i].k = n;
        multiply_getnorm_vals[i].max_value = &norm;
        multiply_getnorm_vals[i].mutex = mutex;

        pthread_create(&working_thread[i], NULL, multiply, (void *) &multiply_getnorm_vals[i]);

    }
    for (int i=0; i<threads; i++) {
        pthread_join(working_thread[i], &status);
        
    }

    for (int i=0;i<threads; i++) {
        int columns = n/threads;

        int col_num = i*columns;

        if (i == threads-1) {
            columns = n-(i*columns);
        }

        multiply_getnorm_vals[i].C = &C[col_num];
        multiply_getnorm_vals[i].n = columns;
        multiply_getnorm_vals[i].m = n;
        multiply_getnorm_vals[i].k = n;
        multiply_getnorm_vals[i].max_value = &norm;
        multiply_getnorm_vals[i].mutex = mutex;

        pthread_create(&working_thread[i], NULL, getnorm,(void *) &multiply_getnorm_vals[i]);
        

    }
    for (int i=0; i<threads; i++) {
        pthread_join(working_thread[i], &status);
    }

    return norm;
}

void *multiply(void *arg) {
    multiply_getnorm *values = arg;
    double alpha = 1.;
    double beta = 0.;

    ATL_dgemm(CblasNoTrans, CblasNoTrans, values->m, values->n, values->k,
            alpha, values->A, values->k, values->B, values->k,
            beta, values->C, values->n);

}


void *getnorm(void *arg) {
    multiply_getnorm *values = arg;

    for (int i=0; i<values->m; i++) {
        double total = 0.;
        for (int j=0; j<values->k; j++) {
            double add = values->C[(j*values->m) + i];

            total += fabs(add);
        }

        pthread_mutex_lock(values->mutex);

        if (*values->max_value < total) {
            *values->max_value = total;
        }

        pthread_mutex_unlock(values->mutex);

        pthread_exit(NULL);
    }
}



