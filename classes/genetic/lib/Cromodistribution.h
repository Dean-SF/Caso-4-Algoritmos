#ifndef _CROMODISTRIBUTION_
#define _CROMODISTRIBUTION_

#define CROMO_MAX_VALUE 4294967296
#define NIBBLE_SIZE 32
#define MIN_GENOTYPE_SIZE_BY_PARENT 5

#include "GrayIntensity.h"
#include "TypeOfShape.h"

/*
Edited cromosomatic distribution representation structure
*/

struct Cromodistribution {
    int xAxis;
    int yAxis;
    GrayIntensity color;
    TypeOfShape shape;
};


#endif