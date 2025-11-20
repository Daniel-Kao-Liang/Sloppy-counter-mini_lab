#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>

#define NUM_THREADS 4
#define TOTAL_INCREMENTS_PER_THREAD 10000000 

int global_counter = 0;
pthread_mutex_t lock;
int threshold = 1; 

double get_time() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec + tv.tv_usec * 1e-6;
}

void* worker(void* arg) {
    int local_counter = 0;

    for (int i = 0; i < TOTAL_INCREMENTS_PER_THREAD; i++) {
        local_counter++; 
        
        //achive Th.. 
        if (local_counter >= threshold) {
            pthread_mutex_lock(&lock);
            global_counter += local_counter;
            pthread_mutex_unlock(&lock);
            //clear local
            local_counter = 0;
        }
    }

    if (local_counter > 0) {
        pthread_mutex_lock(&lock);
        global_counter += local_counter;
        pthread_mutex_unlock(&lock);
    }
    
    return NULL;
}

int main(int argc, char *argv[]) {
    if (argc > 1) {
        threshold = atoi(argv[1]);
    }

    printf("=== lab start: Threshold = %d ===\n", threshold);
    printf("active %d differents thread, everyone add %d times...\n", NUM_THREADS, TOTAL_INCREMENTS_PER_THREAD);

    pthread_t threads[NUM_THREADS];
    pthread_mutex_init(&lock, NULL);

    double start = get_time();

    // form thread
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_create(&threads[i], NULL, worker, NULL);
    }

    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    double end = get_time();

    printf("final global counter: %d (expected: %d)\n", global_counter, NUM_THREADS * TOTAL_INCREMENTS_PER_THREAD);
    printf("total time: %.4f s\n", end - start);
    printf("==================================\n\n");

    return 0;
}