#ifndef ORGANISM_DEFINED
#define ORGANISM_DEFINED
#include <stdlib.h>

#include "City.h"

extern const int NUM_CITIES;
extern City* CITIES;

class Organism{
private:
    City* path;
    bool alive;
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

    void generateMutatedPath(const Organism& ancestor, int& swaps) {
        int random1, random2;
        for(int i = 0; i < swaps; i++) {
            random1 = rand() % NUM_CITIES;
            do {
                random2 = rand() % NUM_CITIES;
            }
            while(random1 == random2);
        }
    }
public:
    //Default constructor. Creates an Organism with a random path.
    Organism() {
        generateRandomPath(CITIES);
        this->alive = true;
        calcFitness();
    }
    
    //Creates a new Organism based on an ancestor Organism.
    Organism(const Organism& ancestor, int& swaps) {
        generateMutatedPath(ancestor, swaps);
        this->alive = true;
        calcFitness();
    }
    
    double getFitness() { return this->fitness; }

    void mutate();
    void die() { this->alive = false; }
    bool isAlive() { return alive; }

    void print() {
        printf("Organism:\n\tPath:");
        for(int i = 0; i < NUM_CITIES; i++) {
            //printf(" %d", this->path[i].getNumber());
        }
    }

    bool isMoreFit(Organism* other) {
        return this->fitness < other->fitness;
    }

    bool isMoreFit(double fitness) {
        return this->fitness < fitness;
    }
    
    void operator = (const Organism& other) {
        for(int i = 0; i < NUM_CITIES; i++) {
            this->path[i] = other.path[i];
        }
        this->alive = other.alive;
        calcFitness();
    }
    
    ~Organism() {
        delete [] path;
    }
};
#endif
