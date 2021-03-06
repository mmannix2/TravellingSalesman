#ifndef ORGANISM_DEFINED
#define ORGANISM_DEFINED
#include <stdlib.h>

#include "City.h"

extern const int NUM_CITIES;
extern City CITIES[];

class Organism{
private:
    City* path;
    double fitness;

    void calcFitness() {
        this->fitness = 0.0;
        for(int i = 1; i < NUM_CITIES; i++) {
            //Calculate the distance between each City in the Path
            this->fitness += path[i-1].calcDistance(&path[i]);
        }
        //Calculate the distance back to the home City
        this->fitness += path[NUM_CITIES-1].calcDistance(&path[0]);
    }

    void generateRandomPath(City* cities) {
        //Instantiate my path array
        this->path = new City[NUM_CITIES];
        
        int random;
        bool cityUsed[NUM_CITIES];
        
        //Create an array of bools starting out false
        for(int i = 0; i < NUM_CITIES; i++) {
            cityUsed[i] = false;
        }

        //Populate the path with City objs from CITIES
        for(int i = 0; i < NUM_CITIES; i++) {
            //Find a random City number that hasn't been used
            do {     
                random = rand() % NUM_CITIES;
            }
            while(cityUsed[random]);
            
            //Use the City number
            path[i] = CITIES[random];
            cityUsed[random] = true;
        }
    }

    void inheritPath(Organism* ancestor) {
        this->path = new City[NUM_CITIES];
        int random1, random2;
        int swaps = rand() % 50;

        //Copy every value from ancestor.path into this->path
        for(int i = 0; i < NUM_CITIES; i++) {
            this->path[i] = ancestor->path[i];
        }
        
        //Swap some of the items
        for(int i = 0; i < swaps; i++) {
            random1 = rand() % (NUM_CITIES /2);
            do {
                random2 = rand() % NUM_CITIES;
            }
            while(random1 == random2);
            
            City pointer = this->path[random1];
            this->path[random1] = this->path[random2];
            this->path[random2] = pointer;
        }
    }

    void randomizePath() {
        int random;
        bool cityUsed[NUM_CITIES];
        
        //Create an array of bools starting out false
        for(int i = 0; i < NUM_CITIES; i++) {
            cityUsed[i] = false;
        }

        //Populate the path with City objs from CITIES
        for(int i = 0; i < NUM_CITIES; i++) {
            //Find a random City number that hasn't been used
            do {     
                random = rand() % NUM_CITIES;
            }
            while(cityUsed[random]);
            
            //Use the City number
            path[i] = CITIES[random];
            cityUsed[random] = true;
        }
    }

public:
    //Default constructor. Creates an Organism with a random path.
    Organism() {
        generateRandomPath(CITIES);
        calcFitness();
    }
    
    double getFitness() { return this->fitness; }

    void mutate() {
        randomizePath();
        calcFitness();
    }

    void breed(Organism* ancestor) {
        inheritPath(ancestor);
        calcFitness();
    }

    void print() {
        printf("Organism:\n\tFitness: %.02f\n\tPath:", this->fitness);
        for(int i = 0; i < NUM_CITIES; i++) {
            if(i % 10 == 0) {
                printf("\n\t\t");
            }
            printf(" %02d", this->path[i].getNumber());
        }
        printf("\n");
    }

    bool isMoreFit(Organism* other) {
        return this->fitness < other->fitness;
    }

    bool isMoreFit(double fitness) {
        return this->fitness < fitness;
    }
};
#endif
