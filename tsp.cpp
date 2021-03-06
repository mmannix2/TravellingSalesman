#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <math.h>
#include <float.h>
#include <mpi.h>

#include "City.h"
#include "Organism.h"

#ifdef DEBUG
#define DEBUG_PRINT(...) \
    do { \
        fprintf( stderr, __VA_ARGS__ ); \
    } \
    while(0)
#else
#define DEBUG_PRINT(...) \
    do { \
    } \
    while(0)
#endif

const int THREADS = 8;
const int POP = 500000;
const int GEN = 100;
const double SURVIVAL_RATE = 0.50;
const double MUTATE_RATE = 0.50;
const int NUM_CITIES = 100;

pthread_mutex_t printMutex;

City CITIES[NUM_CITIES];
Organism population[POP];

Organism* alpha[THREADS]; //The most-fit Organism
Organism* omega[THREADS]; //The least-fit Organism

pthread_rwlock_t alpha_lock;
pthread_rwlock_t omega_lock;

int seed = 0;

//For each tribe, looks for an alpha and omega Organism
void find_alpha_and_omega(int id, int size, int start, int end) {
    alpha[id] = &population[start];
    omega[id] = &population[start];
    //deathCount[g] = 0;  
    
    //Find the alpha of this generation
    for(int p = start; p < end; p++) {
        if(population[p].isMoreFit(alpha[id])) {
            alpha[id] = &population[p];
        }
        
        if(!population[p].isMoreFit(omega[id])) {
            omega[id] = &population[p];
        }
    }
}

void* genetic(void* data) {
    int id = *(int*) data;
    int size = (POP / THREADS);
    int start = size * id;
    int end = start + size;
    
    DEBUG_PRINT("id: %d, size: %d, start: %d, end: %d\n", id, size, start, end);

    //Initialization
    srand(id + seed);
    
    //Make initial population
    for(int p = start; p < end; p++) {
        population[p] = Organism();
    }

    //Simulate GEN generations
    for(int g = 0; g < GEN; g++) {
        find_alpha_and_omega(id, size, start, end);

        double percentile = 0.0;
        //Kill any Organisms that are out of the SURVIVAL_RATE
        for(int p = start; p < end; p++) {
            //Make a sort of inverted percentile of fitness
            //Lower is better
            percentile = 
                (alpha[id]->getFitness() - population[p].getFitness()) /
                (alpha[id]->getFitness() - omega[id]->getFitness());
            
            if( percentile > SURVIVAL_RATE) {
                if(rand() % 100 < MUTATE_RATE * 100) {
                    //Randomly mutate this Organism
                    population[p].mutate();
                }
                else {
                    //Mix this Organism's genes with the alpha's genes
                    population[p].breed(alpha[id]);
                }
            }
        }

    }

    //Find alpha and omega one last time
    find_alpha_and_omega(id, size, start, end);

    return (void*) alpha[id];
}    

int main(int argc, char** argv) {
    int rank, size;    
    int ids[THREADS];
    pthread_t threads[THREADS];
    Organism* best = NULL;
    
    MPI_Init(&argc, &argv);

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    
    double best_fitness; //The best fitness score

    const char* filename = "cities.txt";

    FILE* file;

    pthread_mutex_init(&printMutex, NULL);
    pthread_rwlock_init(&alpha_lock, NULL);    
    pthread_rwlock_init(&omega_lock, NULL);    
    
    //Check if a seed is given
    if(argc > 1) {
        seed = atoi(argv[1]);
        DEBUG_PRINT("Using %d as a seed.\n", seed);
    }
    
    //Open the file
    file = fopen(filename, "r");

    //Read in every City from the file
    double tempX, tempY;
    for(int i = 0; i < NUM_CITIES; i++) { 
        fscanf(file, "%lf %lf", &tempX, &tempY); 
        CITIES[i] = City(i, tempX, tempY);
    }
    
    DEBUG_PRINT("Cities loaded.\n");
    
    //TEST
    /*Organism o1 = Organism();
    Organism o2 = Organism();
    
    o1.print();
    o2.print();

    o1.breed(&o2);

    o1.print();
    */

    //Spawn threads
    for(int t = 0; t < THREADS; t++) {
        ids[t] = t; 
        pthread_create(&threads[t], NULL, genetic, &ids[t]);
    }
    
    //Let the threads work

    //Join the threads
    for(int t = 0; t < THREADS; t++) {
        Organism* temp = NULL;
        pthread_join(threads[t], (void**) &temp);
        if(best == NULL || temp->getFitness() < best->getFitness()) {
            best = temp;
        }
        DEBUG_PRINT("Process %d, thread %d's best fitness: %.02f\n",
            rank,
            t,
            best->getFitness());
    }
    
    //Give every process the best_fitness
    double temp = best->getFitness();
    MPI_Allreduce(&temp, &best_fitness, 1, MPI_DOUBLE, MPI_MIN, MPI_COMM_WORLD);
    
    //Print out the best path
    if(best_fitness == best->getFitness()) {
        DEBUG_PRINT("Absolute best: %.02f (from process %d)\n",
            best_fitness, 
            rank);
        best->print();
    }
    
    //Shut down MPI & pthreads
    MPI_Finalize();
    pthread_exit(NULL);
    
    exit(0);
}
