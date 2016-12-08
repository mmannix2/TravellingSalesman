#ifndef CITY_INCLUDED
#define CITY_INCLUDED

#include <stdio.h>
#include <math.h>

extern const int NUM_CITIES;

class City {
private:
    double x;
    double y;

public:
    //const int NUM_CITIES = 100;
    //City cities[NUM_CITIES];

    City() {
        this->x = 0;
        this->y = 0;
    }
    
    City(double x, double y) {
        this->x = x;
        this->y = y;
    }

    void print() {
        printf("City: x:%.02f, y:%.02f\n", this->x, this->y);
    }

    double calcDistance(City* other) {
        return sqrt( pow(other->x - this->x, 2) + pow(other->y - this->y, 2) );
    }

    double getX() { return this->x; }
    
    double getY() { return this->y; }
};
#endif
