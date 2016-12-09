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

const int THREADS = 4;
const int POP = 1000;
const int GEN = 1;
const double SURVIVAL_RATE = 0.70;
const int NUM_CITIES = 100;

pthread_mutex_t printMutex;

City* CITIES = new City[NUM_CITIES];
Organism** population = new Organism*[THREADS];

void* genetic(void* data) {
    int id = *(int*) data;
    
    //Seed srand
    srand(id);
    
    //Make initial population
    for(int p = 0; p < POP; p++) {
        population[id] = new Organism[POP];
    }
    
    //Setup variables
    Organism* fittest = new Organism[GEN];
    Organism* least_fit = new Organism[GEN];
    int deathCount[GEN];
    Organism* best = &population[id][0];

    //Simulate GEN generations
    for(int g = 0; g < GEN; g++) {
        fittest[g] = population[id][0];
        least_fit[g] = population[id][0];
        deathCount[g] = 0;  
        
        //Find the fittest of this generation
        for(int p = 0; p < POP; p++) {
            if(population[id][p].isMoreFit(&fittest[g])) {
                fittest[g] = population[id][p];
            }
            
            if(!population[id][p].isMoreFit(&least_fit[g])) {
                least_fit[g] = population[id][p];
            }
        }
        
        double percentile = 0.0;
        //Kill any Organisms that are out of the SURVIVAL_RATE
        for(int p = 0; p < POP; p++) {
            //Make a sort of inverted percentile of fitness
            //Lower is better
            percentile = 
                (fittest[g].getFitness() - population[id][p].getFitness()) /
                (fittest[g].getFitness() - least_fit[g].getFitness());
            
            if( percentile > SURVIVAL_RATE) {
                if(p % 2 == 0){
                    population[id][p] = Organism(fittest[g], p);
                }
                else {
                    population[id][p] = Organism();
                }
                deathCount[g]++;
            }
        }
        
        if(fittest[g].isMoreFit(best)) {
            best = &fittest[g];
        }
    }
    return (void*) best;
}    

int main(int argc, char** argv) {
    pthread_t threads[THREADS];
    Organism* best = new Organism();

    char* filename;
    FILE* file;

    pthread_mutex_init(&printMutex, NULL);
    
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
        //CITIES[i].print();
    }
    
    DEBUG_PRINT("Cities loaded.\n");

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
        //Print the best Organism that each thread found
        printf("Best from thread %02d:\n", t);
        best->print();
        printf("\n");
    }
     
    pthread_exit(NULL);
}
