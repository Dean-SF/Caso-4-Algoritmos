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

const unsigned TOTAL_OF_PIXELS = 1166400; // 1080x1080

class ProbabilisticAlgorithm {
private:
    double samplePercentage;
    double sizeOfSample;
    double pixelCounter;
    vector<Box*> boxes;
    multimap<double,Box*> table;
    double totalBoxes;
    CImg<unsigned char> *picture;

    void printTable() {     //DEBUG
        double suma = 0.00;
        for (auto& row : table) {
            suma += row.first;
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
        cout << "suma total = " << suma << endl;
    }

    double getInverseProbability(int pPixelsInBox, double pPixelCounter) {
        if (totalBoxes == 1) 
            return 1;
        double totalBoxesCopy = totalBoxes;
        totalBoxesCopy--;
        return (1.0 - (pPixelsInBox/pPixelCounter)) / totalBoxesCopy;
    }

    // Aquí se recalcula la probabilidad de NewBox en la tabla
    double updateRandomProbability(double pPixelCounter) {
        double newProbability = getInverseProbability(1, pPixelCounter);
        table.insert({newProbability, nullptr});
        return newProbability;
    }

    void updateTable(double pPixelCounter) {
        table.clear();
        double currentProbability, updatedProbability, lastProbability;
        int pixelsInBox;
        Box* currentBox;

        updateRandomProbability(pPixelCounter);
        int boxIterator = 0;

        for (; boxIterator != boxes.size(); boxIterator++) { 
            currentBox = boxes[boxIterator];
            pixelsInBox = currentBox->getPixelCount();
            updatedProbability = getInverseProbability(pixelsInBox, pPixelCounter);
            table.insert({updatedProbability, currentBox});
        }
    }

    // Buscar box mediante el pixel de referencia, cuando lo encuentra agrega el pixel nuevo
    void addPixelToBox(Pixel pPixelToAdd, Pixel pReferencePixel) {
        for (int boxIter = 0; boxIter < boxes.size(); boxIter++) {
            if (boxes[boxIter]->getReferencePixel().getXAxis() == pReferencePixel.getXAxis()
            || boxes[boxIter]->getReferencePixel().getYAxis() == pReferencePixel.getYAxis()) {
                boxes[boxIter]->appendPixel(pPixelToAdd);
            }
        }
    }

    // A veces agarra la fila del random cuando va a elegir pixel para un box, entonces se usa esto para que tome otra box
    Box* getValidBox() {
        multimap<double, Box*>::iterator boxIter = table.end();
        boxIter--;
        for (; boxIter != table.begin(); boxIter--) {
            if ((*boxIter).second != nullptr) {
                return (*boxIter).second;
            }
        }
        return nullptr;
    }

    // Segun el porcentaje random, ver en cual box cae 
    Box* getBox(double pRandomPercentage) {
        double min = 0.00;
        double max;
        multimap<double, Box*>::iterator boxIter = table.begin();
       
        for (; boxIter != table.end(); boxIter++) {
            max = boxIter->first + min;
            Range range(min, max);
            if (range.isInRange(pRandomPercentage)) {
                // cout << "random percent = " << pRandomPercentage << endl;
                // cout << "min and max = " << "(" << min << "," << max << ")" << endl;
                return boxIter->second;
            }
            min = max;
        }
        return nullptr;
    }

    // Crear un pixel segun sus coordenadas (x,y) y el color que se consigue de la imagen
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

    void setTable(multimap<double,Box*> pTable) {
        table = pTable;
    }

    multimap<double,Box*>* getTable() {
        return &table;
    }

    int getTotalBoxes() {
        return totalBoxes;
    }

    void setTotalBoxes(int pTotalBoxes) {
        totalBoxes = pTotalBoxes;
    }

    void changeColors() {   // DEBUG
        for (int i = 0; i < boxes.size(); i++) {
            vector<Pixel> group = boxes[i]->getPixelGroup();
            
            for (int j = 0; j < group.size(); j++) {
                int x = group[j].getXAxis();
                int y = group[j].getYAxis();
               (*picture)(x,y,0,0) = 255;
               (*picture)(x,y,0,1) = 0;
               (*picture)(x,y,0,2) = 0;
            }
        }
        CImgDisplay display(*picture,"SoylaImagen");
        display.resize(720,720,true);
        while(!display.is_closed()) {
            sleep(0.5);
        }
    }

    void work() {
        table.insert({1.0, nullptr});   //Al iniciar tendremos la fila de la tabla para NewBox

        srand (time(0));
        int randomNumber, minX, minY, maxX, maxY, randomX, randomY;
        double randomPercentage, probabilityForNewBox;

        while (pixelCounter <= 168) {
            randomPercentage = (rand() % 100) / 100.00;

            Range rangeForNewBox(0.00, getInverseProbability(1,pixelCounter));

            if (rangeForNewBox.isInRange(randomPercentage)) {   // crear new box
                pixelCounter++;
                totalBoxes++;

                randomX = rand() % 1080;
                randomY = rand() % 1080;
                Pixel randPixel = getNewPixel(randomX, randomY);
                Box* newBox = new Box(randPixel);
                
                table.insert({getInverseProbability(1,pixelCounter), newBox});
                boxes.emplace_back(newBox);

            } else {    // crear pixel dentro de un box
                Box* chosenBox = getBox(randomPercentage);

                if (chosenBox == nullptr) 
                    chosenBox = getValidBox();
               
                Pixel referencePixel = chosenBox->getReferencePixel();
                
                minX = chosenBox->getMinCoords().getXAxis();
                maxX = chosenBox->getMaxCoords().getXAxis();
                minY = chosenBox->getMinCoords().getYAxis();
                maxY = chosenBox->getMaxCoords().getYAxis();

                randomX = abs(minX + (rand() % maxX - minX + 1));
                randomY = abs(minY + (rand() % maxY - minY + 1));
            
                Pixel newPixel = getNewPixel(randomX, randomY);

                if (chosenBox->checkColor(newPixel)) {
                    addPixelToBox(newPixel, referencePixel);
                    pixelCounter++;
                }
            }
            updateTable(pixelCounter);
        }
        cout << "RESULTADO FINAL " << endl;
        printTable();
        changeColors();
    }

};

#endif