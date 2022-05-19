#ifndef RANGE_H
#define RANGE_H

class Range {
private:
    double minProbability;
    double maxProbability;
public:
    Range(double pMinprobability, double pMaxProbability) {
        minProbability = pMinprobability;
        maxProbability = pMaxProbability;
    }

    ~Range() {}

    double getMinProbability() {
        return minProbability;
    }

    double getMaxProbability() {
        return maxProbability;
    }

    void setMinProbability(double pMinProbability) {
        minProbability = pMinProbability;
    }

    void setMaxProbability(double pMaxProbability) {
        maxProbability = pMaxProbability;
    }

    bool isInRange(double pNumber) {
        if(minProbability <= pNumber && pNumber < maxProbability) {
            return true;
        } 
        return false;
    }
};

#endif