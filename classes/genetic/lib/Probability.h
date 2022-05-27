#ifndef PROBABILITY_H
#define PROBABILITY_H

struct Probability {
    int amountDark = 0;
    int amountMid = 0;
    int amountLight = 0;
    int totalAmount = 0;
    int amountClosest = 0;
    double distanceClosest = 0;
};

#endif