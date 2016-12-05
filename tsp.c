#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#define NUM_CITIES 5

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

typedef struct {
    double x;
    double y;
} City;

//typedef int Path[NUM_CITIES];

void city_init(City* city, double x, double y) {
    if(city != NULL) {
        city->x = x;
        city->y = y;
    }
    //DEBUG
    else {
        DEBUG_PRINT("city_init failed. city pointer is NULL.\n");
    }
}

void city_print(City* city) {
    printf("City: x:%.02f, y:%.02f\n", city->x, city->y);
}

double city_calc_distance(City* a, City* b) {
    return sqrt( pow(b->x - a->x, 2) + pow(b->y - a->y, 2) );
}

void path_calc_fitness(int path[NUM_CITIES], double* total_distance) {
    *total_distance = 0.0;
    for(int i = 1; i < NUM_CITIES; i++) {
        //Calculate the distance between each City in the Path
        *total_distance += DISTANCE[path[i-1]][path[i]];
    }
    //Calculate the distance back to the home City
    *total_distance += DISTANCE[path[NUM_CITIES-1]][path[0]];
}

int main(int argc, char** argv) {
    char* filename;
    FILE* file;
    City cities[NUM_CITIES];

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
    
    //Make a Path
    //Path* path = malloc(sizeof(Path));
    int path[NUM_CITIES];
    for(int i = 0; i < NUM_CITIES; i++) {
        path[i] = i;
    }
    
    double distance;
    path_calc_fitness(path, &distance);
 
    printf("Distance of path 1: %.02f\n", distance); 
    return 0;
}
