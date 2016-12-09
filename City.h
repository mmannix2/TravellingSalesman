#ifndef CITY_INCLUDED
#define CITY_INCLUDED

#include <stdio.h>
#include <math.h>

extern const int NUM_CITIES;

class City {
private:
    int number;
    double x;
    double y;

public:
    //const int NUM_CITIES = 100;
    //City cities[NUM_CITIES];

    City() {
        this->number = -1;
        this->x = 0;
        this->y = 0;
    }
    
    City(int number, double x, double y) {
        this->number = number;
        this->x = x;
        this->y = y;
    }

    void print() {
        printf("City #%d: x:%.02f, y:%.02f\n", this->number, this->x, this->y);
    }

    double calcDistance(City* other) {
        return sqrt( pow(other->x - this->x, 2) + pow(other->y - this->y, 2) );
    }

    double getX() { return this->x; }
    double getY() { return this->y; }
    
    int getNumber() { return this->number; }
};
#endif
