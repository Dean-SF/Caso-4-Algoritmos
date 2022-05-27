#ifndef PROBABILISTICALGORITHM_H
#define PROBABILISTICALGORITHM_H

#include <map>
#include <random>
#include <ctime>
#include <iostream>
#include <unistd.h>
#include <vector>
#include <iterator>
#include "Box.h"
#include "Pixel.h"
#include "Range.h"
#include "../../libraries/CImg.h"

using namespace std;
using namespace cimg_library;
using std::map;

#define BACKGROUND_RANGE 20

const unsigned TOTAL_OF_PIXELS = 1166400; // 1080x1080

class ProbabilisticAlgorithm {
private:
    unsigned char backgroundColor;
    double samplePercentage;
    double sizeOfSample;
    double pixelCounter;
    vector<Box*> boxes;
    map<double,Box*> table;
    double totalBoxes;
    CImg<unsigned char> *picture;

    void printTable() {
        double suma = 0.00;
        for (auto& row : table) {
            cout << "[ ";
            cout << "Probability = " << row.first << endl;
            cout << "Pixels = ";
            Box* currentBox = row.second;
            if (currentBox == nullptr) {
                cout << "random" << endl;
            } else {
                currentBox->printPixels();
                cout << "]" << endl;
            }
        }
    }

    void setBackgroundColor(){
        Pixel *temporalPixel = new Pixel(0, 0, *(picture->data(0,0,0,0)), *(picture->data(0,0,0,1)), *(picture->data(0,0,0,2)));
        backgroundColor = temporalPixel->getColor();
        delete temporalPixel;
    }

    bool isBackground(int pXAxis, int pYAxis) {
        unsigned char pixelColor = 0.299*(*(picture->data(pXAxis,pYAxis,0,0))) + 0.587*(*(picture->data(pXAxis,pYAxis,0,1))) + 0.114*(*(picture->data(pXAxis,pYAxis,0,2))); 
        if(backgroundColor - BACKGROUND_RANGE < pixelColor && pixelColor < backgroundColor + BACKGROUND_RANGE) {
            return true;
        }
        return false;
    }

    double getInverseProbability(int pPixelsInBox) {
        if (totalBoxes == 1) 
            return 1;
        double copyTotalBoxes = totalBoxes;
        copyTotalBoxes--;
        return (1.0 - (pPixelsInBox/pixelCounter)) / copyTotalBoxes;
    }

    void updateTable() {
        table.clear();
        int pixelsInBox;
        Box* currentBox;

        double previousProbability = 0.00;
        double updatedProbability, newProbability;
        int boxIterator = 0;

        for (; boxIterator != boxes.size(); boxIterator++) { 
            currentBox = boxes[boxIterator];
            if (currentBox == nullptr) 
                updatedProbability = getInverseProbability(1);
            else {
                pixelsInBox = currentBox->getPixelCount();
                updatedProbability = getInverseProbability(pixelsInBox);
            }
            newProbability = previousProbability + updatedProbability;
            table.insert({newProbability, currentBox});
            previousProbability = newProbability;
        }
    }

    void addPixelToBox(Pixel pPixelToAdd, Pixel pReferencePixel) {
        for (int boxIter = 1; boxIter < boxes.size(); boxIter++) {
            if (boxes[boxIter]->getReferencePixel().getXAxis() == pReferencePixel.getXAxis()
            || boxes[boxIter]->getReferencePixel().getYAxis() == pReferencePixel.getYAxis()) {
                boxes[boxIter]->appendPixel(pPixelToAdd);
            }
        }
    }

    // Segun el porcentaje random, ver en cual box cae 
    Box* getBox(double pRandomPercentage) {
        return (*table.lower_bound(pRandomPercentage)).second;
    }

    Pixel getNewPixel(int pXAxis, int pYAxis) {
        Pixel newPixel(pXAxis, pYAxis, *(picture->data(pXAxis,pYAxis,0,0)), *(picture->data(pXAxis,pYAxis,0,1)), *(picture->data(pXAxis,pYAxis,0,2)));
        return newPixel;
    }

public:
    ProbabilisticAlgorithm (double pSamplePercentage, CImg<unsigned char> *pPicture) {
        samplePercentage = pSamplePercentage;
        sizeOfSample = TOTAL_OF_PIXELS * samplePercentage;
        pixelCounter = 1;
        totalBoxes = 1;
        picture = pPicture;
    }

    double getSamplePercentage() {
        return samplePercentage;
    }

    void setSamplePercentage(double pSamplePercentage) {
        samplePercentage = pSamplePercentage;
    }

    double getSizeOfSample() {
        return sizeOfSample;
    }

    void setSizeOfSample(double pSizeOfSample) {
        sizeOfSample = pSizeOfSample;
    }

    int getPixelCounter() {
        return pixelCounter;
    }

    void setPixelCounter(int pPixelCounter) {
        pixelCounter = pPixelCounter;
    }

    vector<Box*>* getBoxes() {
        return &boxes;
    }

    void setTable(map<double,Box*> pTable) {
        table = pTable;
    }

    map<double,Box*>* getTable() {
        return &table;
    }

    void changeColors() {
        for (int i = 1; i < boxes.size(); i++) {
            unsigned char red = rand() % 255;
            unsigned char green = rand() % 255;
            unsigned char blue = rand() % 255;
            vector<Pixel> group = boxes[i]->getPixelGroup();
            for (int j = 0; j < group.size(); j++) {
                int x = group[j].getXAxis();
                int y = group[j].getYAxis();
               (*picture)(x,y,0,0) = red;
               (*picture)(x,y,0,1) = green;
               (*picture)(x,y,0,2) = blue;
            }
        }
        CImgDisplay display(*picture,"SoylaImagen");
        display.resize(720,720,true);
        while(!display.is_closed()) {
            sleep(0.5);
        }
    }

    void work() {
        srand (time(0));
        boxes.emplace_back(nullptr);
        table.insert({1.0, nullptr});
        setBackgroundColor();
      
        int randomNumber, minX, minY, maxX, maxY, randomX, randomY;
        double randomPercentage, probabilityForNewBox;

        int temp = 0;

        for(int i = 0; i < sizeOfSample; i++) {
            //cout << i << " : " << pixelCounter << endl;
            randomPercentage = (rand() % 100) / 100.00;
            Box* chosenBox = getBox(randomPercentage);

            if (chosenBox == nullptr) {
                randomX = 30 + rand() % 1019;
                randomY = 30 + rand() % 1019;
                if(isBackground(randomX,randomY)) {
                    continue;
                }
                pixelCounter++;
                totalBoxes++;
                Pixel randPixel = getNewPixel(randomX, randomY);
                Box* newBox = new Box(randPixel);
                boxes.emplace_back(newBox);
                
            } else {
                Pixel referencePixel = chosenBox->getReferencePixel();
                
                minX = chosenBox->getMinCoords().getXAxis();
                maxX = chosenBox->getMaxCoords().getXAxis();
                minY = chosenBox->getMinCoords().getYAxis();
                maxY = chosenBox->getMaxCoords().getYAxis();

                randomX = abs(minX + (rand() % (maxX-minX)));
                randomY = abs(minY + (rand() % (maxY-minY)));
            
                Pixel newPixel = getNewPixel(randomX, randomY);
                if (chosenBox->checkColor(newPixel)) {
                    chosenBox->appendPixel(newPixel);
                    pixelCounter++;
                }
            }
            updateTable();
        }
        /*
        while (pixelCounter <= 50000) {
            cout << pixelCounter << endl;
            randomPercentage = (rand() % 100) / 100.00;
            Box* chosenBox = getBox(randomPercentage);

            if (chosenBox == nullptr) {
                randomX = 30 + rand() % 1019;
                randomY = 30 + rand() % 1019;
                if(isBackground(randomX,randomY)) {
                    continue;
                }
                pixelCounter++;
                totalBoxes++;
                Pixel randPixel = getNewPixel(randomX, randomY);
                Box* newBox = new Box(randPixel);
                boxes.emplace_back(newBox);
                
            } else {
                Pixel referencePixel = chosenBox->getReferencePixel();
                
                minX = chosenBox->getMinCoords().getXAxis();
                maxX = chosenBox->getMaxCoords().getXAxis();
                minY = chosenBox->getMinCoords().getYAxis();
                maxY = chosenBox->getMaxCoords().getYAxis();

                randomX = abs(minX + (rand() % (maxX-minX)));
                randomY = abs(minY + (rand() % (maxY-minY)));
            
                Pixel newPixel = getNewPixel(randomX, randomY);
                if (chosenBox->checkColor(newPixel)) {
                    chosenBox->appendPixel(newPixel);
                    pixelCounter++;
                }
            }
            updateTable();
        }*/
        // cout << "RESULTADO FINAL " << endl;
        //printTable();
        //changeColors();
    }

};

#endif