#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "../genetic/GeneticAlgorithm.h"
#include "../probabilistic/ProbabilisticAlgorithm.h"
#include "../../libraries/CImg.h"
#include "../probabilistic/Box.h"

class Controller {
private:
    CImg<unsigned char> *picture;
    ProbabilisticAlgorithm *probabilistic;
    GeneticAlgorithm *genetic;
    
public:
    Controller() {
        picture = new CImg<unsigned char>("../../samples/repre(1080).jpg");
        probabilistic = new ProbabilisticAlgorithm(0.085,picture); 
        genetic = new GeneticAlgorithm();
    }
    ~Controller() {
        delete probabilistic;
        delete genetic;
        delete picture;
    }
    void start() {
        probabilistic->work();
        vector<Box*> *result = probabilistic->getBoxes();
        int totalPixels = probabilistic->getPixelCounter();
        genetic->work(result, totalPixels);
    }
};

#endif