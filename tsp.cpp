#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <math.h>
#include <float.h>

#include "City.h"
#include "Organism.h"

#define DEBUG

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
const int POP = 1000000;
const int GEN = 10;
const double SURVIVAL_RATE = 0.70;
const int NUM_CITIES = 100;

pthread_mutex_t printMutex;

pthread_rwlock_t fittest_lock;
pthread_rwlock_t least_fit_lock;

City CITIES[NUM_CITIES];
Organism population[POP];

Organism* fittest = NULL;
Organism* least_fit = NULL; 

void* genetic(void* data) {
    int id = *(int*) data;
    int size = (POP / THREADS);
    int start = size * id;
    int end = start + size;
    
    DEBUG_PRINT("id: %d, size: %d, start: %d, end: %d\n", id, size, start, end);

    //Initialization
    srand(id);
    
    //Make initial population
    for(int p = start; p < end; p++) {
        population[p] = Organism();
    }

    //Simulate GEN generations
    for(int g = 0; g < GEN; g++) {
        fittest = &population[0];
        least_fit = &population[0];
        //deathCount[g] = 0;  
        
        //Find the fittest of this generation
        for(int p = start; p < end; p++) {
            if(population[p].isMoreFit(fittest)) {
                //Critical section
                pthread_rwlock_wrlock(&fittest_lock);
                fittest = &population[p];
                pthread_rwlock_unlock(&fittest_lock);
            }
            
            if(!population[p].isMoreFit(least_fit)) {
                //Critical section
                pthread_rwlock_wrlock(&least_fit_lock);
                least_fit = &population[p];
                pthread_rwlock_unlock(&least_fit_lock);
            }
        }
        
        //Barrier
        if(id == 0) {
            DEBUG_PRINT("Gen %02d Complete\nFittest: %.02f\n",
                g,
                fittest->getFitness());
        }
        
        double percentile = 0.0;
        //Kill any Organisms that are out of the SURVIVAL_RATE
        for(int p = start; p < end; p++) {
            //Make a sort of inverted percentile of fitness
            //Lower is better
            percentile = 
                (fittest->getFitness() - population[p].getFitness()) /
                (fittest->getFitness() - least_fit->getFitness());
            
            if( percentile > SURVIVAL_RATE) {
                population[p].mutate();
                //DEBUG_PRINT("An organism died.\n");
                /*
                if(rand() % 2 == 0){
                    population[id][p] = Organism(fittest, p);
                }
                else {
                    population[id][p] = Organism();
                }
                deathCount[g]++;
                */
            }
        }

    }
    return (void*) fittest;
}    

int main(int argc, char** argv) {
    pthread_t threads[THREADS];
    Organism* best = NULL;

    char* filename;
    FILE* file;

    pthread_mutex_init(&printMutex, NULL);
    pthread_rwlock_init(&fittest_lock, NULL);    
    pthread_rwlock_init(&least_fit_lock, NULL);    
    
    //Check if a filename is given
    if(argc > 1) {
        filename = argv[1];
    }
    else {
        filename = "cities.txt";
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
    /*
    Organism o = Organism();

    o.print();

    o.mutate();

    o.print();
    */

    int ids[THREADS];
    //Spawn threads
    for(int t = 0; t < THREADS; t++) {
        ids[t] = t; 
        pthread_create(&threads[t], NULL, genetic, &ids[t]);
    }
    
    //Let the threads work

    //Join the threads
    for(int t = 0; t < THREADS; t++) {
        pthread_join(threads[t], (void**) &best);
        if(t == 0) {
            printf("Best from thread %d:\n", t);
            best->print();
        }
    }
     
    pthread_exit(NULL);
}
