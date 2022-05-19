#ifndef _CROMODISTRIBUTION_
#define _CROMODISTRIBUTION_

#include <string>

using std::string;

using namespace std;

struct cromodistribution {
    string name;
    int minValue;
    int maxValue;
    float minProbability;
    float maxProbability;
    int quantity;
    int totalPopulation;
};


#endif