#ifndef _CROMODISTRIBUTION_
#define _CROMODISTRIBUTION_

#include "GrayIntensity.h"
#include "TypeOfShape.h"

struct Cromodistribution {
    int xAxis;
    int yAxis;
    GrayIntensity color;
    TypeOfShape shape;
};


#endif