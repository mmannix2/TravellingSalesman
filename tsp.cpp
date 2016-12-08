#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <math.h>
#include <float.h>

#include "City.h"
#include "Organism.h"

#define POP_SIZE 10

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

const int THREADS = 1;
const int POP = 100;
const int GEN = 10;
const double SURVIVAL_RATE = 0.70;
const int NUM_CITIES = 100;

City* CITIES = new City[NUM_CITIES];
Organism** population = new Organism*[THREADS];

void* genetic(void* data) {
    int id = *(int*) data;
    DEBUG_PRINT("Thread %02d reporting in.\n", id);
    
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
        DEBUG_PRINT("[Trd %02d Gen %03d] Fittest: %.02f\n",
            id, g, fittest[g].getFitness() );
        DEBUG_PRINT("[Trd %02d Gen %03d] Least Fit: %.02f\n",
            id, g, least_fit[g].getFitness() );
        
        double percentile = 0.0;
        //Kill any Organisms that are out of the SURVIVAL_RATE
        for(int p = 0; p < POP; p++) {
            //Make a sort of inverted percentile of fitness
            //Lower is better
            percentile = 
                (fittest[g].getFitness() - population[id][p].getFitness()) /
                (fittest[g].getFitness() - least_fit[g].getFitness());
            
            if( percentile > SURVIVAL_RATE) {
                DEBUG_PRINT("\tAn Organism died (%%: %.02f).\n", percentile);
                DEBUG_PRINT("\tFitness %.02f.\n", population[id][p].getFitness());
                //population[id][p] = Organism();
                population[id][p] = Organism(fittest[g]);
                //population[id][p] = Organism(fittest[g], p);
                DEBUG_PRINT("\tFitness %.02f.\n", population[id][p].getFitness());
                deathCount[g]++;
            }
        }
        DEBUG_PRINT("DeathCount: %d\n", deathCount[g]);
    }
    DEBUG_PRINT("\n");
    
    pthread_exit(NULL);
}    

int main(int argc, char** argv) {
    pthread_t threads[THREADS];
    
    char* filename;
    FILE* file;
    
    //srand(0);
    
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
        CITIES[i] = City(tempX, tempY);
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
        pthread_join(threads[t], NULL);
    }
     
    pthread_exit(NULL);
}
