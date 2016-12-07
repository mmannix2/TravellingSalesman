#include <stdlib.h>
#include <stdio.h>
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

double DISTANCE[NUM_CITIES][NUM_CITIES];

int* gen_path(int seed) {
    int added[NUM_CITIES];
    for(int i = 0; i < NUM_CITIES; i++) {
        added[i] = 0; 
    }

    int* path = malloc(sizeof(int[NUM_CITIES]));
    
    for(int i = 0; i < NUM_CITIES; i++) {
        while(1) {
            int random = rand() % NUM_CITIES;
            if(!added[random]) {
                path[i] = random;
                added[random] = 1;
                //DEBUG_PRINT("%d added.\n", path[i]);
                
                break;
            }
            else {
                random = rand() % NUM_CITIES;
            }
        }
    }

    return path;
}

int main(int argc, char** argv) {
    int NUM_CITIES = 5;

    char* filename;
    FILE* file;
    City cities[NUM_CITIES];
    
    srand(0);
    
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
        city_init(&cities[i], tempX, tempY);
        //city_print(&cities[i]);
    }
    
    DEBUG_PRINT("Cities loaded.\n");
    
    //Populate the DISTANCE lookup table
    for(int i = 0; i < NUM_CITIES; i++) {
        for(int j = 0; j < NUM_CITIES; j++) {
            DISTANCE[i][j] = city_calc_distance(&cities[i], &cities[j]); 
            //DEBUG_PRINT("%d,%d:%.02f\n", i, j, DISTANCE[i][j]);
        }
    }
    
    DEBUG_PRINT("DISTANCE lookup table populated.\n");
    
    int** paths = malloc(sizeof(int[POP_SIZE][NUM_CITIES]));
    for(int i = 0; i < POP_SIZE; i++) {
        paths[i] = gen_path(1);
        DEBUG_PRINT("Path %d created:", i);
        for(int j = 0; j < NUM_CITIES; j++) { 
            DEBUG_PRINT(" %d", paths[i][j]);
        }
        DEBUG_PRINT("\n");
    }
    
    int fittestIndex = 0;
    double fittestDist = DBL_MAX;
    double distance = 0;
    for(int i = 0; i < POP_SIZE; i++) {
        path_calc_fitness(paths[i], &distance);
        if(distance < fittestDist) {
            fittestIndex = i;
            fittestDist = distance;
        }

        printf("Distance of path %d: %.02f\n", i, distance); 
    }
    
    printf("Path %d is fittest. Distance: %.02f\n", fittestIndex, fittestDist); 

    return 0;
}
