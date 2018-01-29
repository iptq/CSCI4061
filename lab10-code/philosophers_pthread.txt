// MODIFY THIS FILE to use pthreads rather than IPC. Lines marked with
// **CHANGE** indicate lines that are higly likely to need alterations.
#include <error.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/sem.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

#define N_PHILOSOPHERS 5 // Number of philosophers

const int MEALS_TO_FINISH = 2; // Number of iterations before finish
const int MAX_DELAY = 50000;   // Maximum delay in nanos between meal iterations

pthread_mutex_t utensils[N_PHILOSOPHERS];

void *philosopher(void *n);
// **CHANGE** to have prototype compatible with thread entry points

int main() {
    setvbuf(stdout, NULL, _IONBF, 0);
    int i, status;
    pthread_t phil[N_PHILOSOPHERS];
    printf("The Dining Swansons (Philosophers) Problem\n");

    // Parent thread only:
    //
    // Allocate utensils: mutexes
    for (i = 0; i < N_PHILOSOPHERS; i++) {
        pthread_mutex_init(&utensils[i], NULL);
    }

    // Parent generates child processes
    int numbers[N_PHILOSOPHERS];
    for (i = 0; i < N_PHILOSOPHERS; i++) {
        numbers[i] = i; // workaround for wrong numbers
        status = pthread_create(&phil[i], NULL, &philosopher, &numbers[i]);
        if (status) {
            fprintf(stderr, "Could not create thread: %s\n", strerror(status));
            exit(1);
        }
    }

    // Parent waits on all children to finish
    for (i = 0; i < N_PHILOSOPHERS; i++) {
        pthread_join(phil[i], NULL);
    }

    printf("JJ: All Swansons finished, cleaning up the diner\n");

    // Eliminate the utensils and table semaphores
    for (i = 0; i < N_PHILOSOPHERS; i++) {
        pthread_mutex_destroy(&utensils[i]);
    }

    return 0;
}

// Code for dining philosopher child processes
void *philosopher(void *arg) {
    int n = *(int *)arg;
    int i, first, second;
    srand(time(NULL)); // Seed random number generator

    // Avoid deadlock via slightly different order of utensil requests
    // for last philospher

    // first utensil to get, most go for n first, last philospher goes for 0
    // first
    first = (n != N_PHILOSOPHERS - 1) ? n : 0;
    // second utensil to get, last philopher goes for n second
    second = (n != N_PHILOSOPHERS - 1) ? n + 1 : n;

    printf("Swanson %d wants utensils %d and %d\n", n, first, second);
    printf("Swanson %d at the table\n", n);

    // Main loop of thinking/eating cycles
    for (i = 0; i < MEALS_TO_FINISH; i++) {
        int sleep_time = rand() % MAX_DELAY;
        usleep(sleep_time); // sleep for for a short time

        printf("%2d: Swanson %d is contemplating his awesomeness ...\n", i, n);

        // get first utensil
        pthread_mutex_lock(&utensils[first]);
        printf("%2d: Swanson %d got utensil %d\n", i, n, first);

        // get second utensil
        pthread_mutex_lock(&utensils[second]);
        printf("%2d: Swanson %d got utensil %d\n", i, n, second);

        printf("%2d: Swanson %d is eating an egg ...\n", i, n);
        usleep(sleep_time); // sleep for for a short time

        // release first utensil
        pthread_mutex_unlock(&utensils[first]);

        // release second utensil
        pthread_mutex_unlock(&utensils[second]);
    }

    printf("Swanson %d leaving the diner\n", n);
    return NULL;
}
