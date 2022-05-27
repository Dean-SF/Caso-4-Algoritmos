#ifndef PROBABILISTICALGORITHM_H
#define PROBABILISTICALGORITHM_H

#include <map>
#include <ctime>
#include <random>
#include <vector>
#include <iostream>
#include <unistd.h>
#include <iterator>
#include "Box.h"
#include "Pixel.h"
#include "Range.h"
#include "../../libraries/CImg.h"

using namespace std;
using std::map;
using namespace cimg_library;

#define BACKGROUND_RANGE 20

const unsigned TOTAL_OF_PIXELS = 1166400; // 1080x1080

class ProbabilisticAlgorithm {
private:
    double totalBoxes;
    vector<Box*> boxes;
    double sizeOfSample;
    double pixelCounter;
    map<double,Box*> table;
    double samplePercentage;
    CImg<unsigned char> *picture;
    unsigned char backgroundColor;

    // Get the picture's background color by getting the color from a Pixel that belongs to the background
    void setBackgroundColor() {
        Pixel *temporalPixel = new Pixel(0, 0, *(picture->data(0,0,0,0)), *(picture->data(0,0,0,1)), *(picture->data(0,0,0,2)));
        backgroundColor = temporalPixel->getColor();
        delete temporalPixel;
    }

    // Check if the pixel located in given 'x' and 'y' axis has the same color as the background of the picture
    bool isBackground(int pXAxis, int pYAxis) {
        unsigned char pixelColor = 0.299*(*(picture->data(pXAxis,pYAxis,0,0))) + 0.587*(*(picture->data(pXAxis,pYAxis,0,1))) + 0.114*(*(picture->data(pXAxis,pYAxis,0,2))); 
        if(backgroundColor - BACKGROUND_RANGE < pixelColor && pixelColor < backgroundColor + BACKGROUND_RANGE) {
            return true;
        }
        return false;
    }

    // Function that returns a probability where the lower the amount of pixels, the higher the probability of being chosen
    double getInverseProbability(int pPixelsInBox) {
        if (totalBoxes == 1) 
            return 1;
        double copyTotalBoxes = totalBoxes;
        copyTotalBoxes--;   // Ignore the row reserved for the probability of creating a new box
        return (1.0 - (pPixelsInBox/pixelCounter)) / copyTotalBoxes;
    }

    // Return a pointer to the box that has a probability equal or greater than the given percentage
    Box* getBox(double pRandomPercentage) {
        return (*table.lower_bound(pRandomPercentage)).second;
    }

    // Return a new pixel with the attributes: x axis, y axis and RGB.
    Pixel getNewPixel(int pXAxis, int pYAxis) {
        Pixel newPixel(pXAxis, pYAxis, *(picture->data(pXAxis,pYAxis,0,0)), *(picture->data(pXAxis,pYAxis,0,1)), 
                        *(picture->data(pXAxis,pYAxis,0,2)));
        return newPixel;
    }

    // This function takes an image and changes the color of the pixels that belong to the boxes from the table
    void showGroupsOnPicture() {
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

    /* This function is called everytime we add a new row to the probability table or when a 
       new pixel is added to a box. */
    void updateTable() {
        table.clear();      // Clear the table (map) to insert updated information
        int pixelsInBox;    // Amount of pixels in a box
        Box* currentBox;    

        /* previousProbability: Store the higher limit of range from the previous box -> lower limit for current box
           updatedProbability: To store the new probability returned by the inverse probability function
           newProbability: The sum of the previous probability and the updated probability -> higher limit for current box */
        double previousProbability = 0.00;
        double updatedProbability, newProbability;
        int boxIterator = 0;

        for (; boxIterator != boxes.size(); boxIterator++) { 
            currentBox = boxes[boxIterator];    // Get pointer to current box
            if (currentBox == nullptr)  // Row from table reserved for the probability of creating a new box
                updatedProbability = getInverseProbability(1);
            else {
                pixelsInBox = currentBox->getPixelCount();
                // Get inverse probability with the amount of pixels from current box
                updatedProbability = getInverseProbability(pixelsInBox); 
            }
            newProbability = previousProbability + updatedProbability;  
            table.insert({newProbability, currentBox});
            previousProbability = newProbability;   // Update higher limit from current box -> lower limit for next iteration
        }
    }

public:
    ProbabilisticAlgorithm (double pSamplePercentage, CImg<unsigned char> *pPicture) {
        totalBoxes = 1;
        pixelCounter = 1;
        picture = pPicture;
        samplePercentage = pSamplePercentage;
        sizeOfSample = TOTAL_OF_PIXELS * samplePercentage;
    }

    int getPixelCounter() {
        return pixelCounter;
    }

    vector<Box*>* getBoxes() {
        return &boxes;
    }
 
    double getSizeOfSample() {
        return sizeOfSample;
    }

    double getSamplePercentage() {
        return samplePercentage;
    }

    map<double,Box*>* getTable() {
        return &table;
    }

    void setTable(map<double,Box*> pTable) {
        table = pTable;
    }

    void setPixelCounter(int pPixelCounter) {
        pixelCounter = pPixelCounter;
    }

    void setSizeOfSample(double pSizeOfSample) {
        sizeOfSample = pSizeOfSample;
    }

    void setSamplePercentage(double pSamplePercentage) {
        samplePercentage = pSamplePercentage;
    }

    void work() {
        srand (time(0));
        boxes.emplace_back(nullptr);    // null pointer to a box that represents the creation of a new box
        table.insert({1.0, nullptr});   // Row reserved for the probability of creating a new box
        setBackgroundColor();

        double randomPercentage;
        int minX, minY, maxX, maxY, randomX, randomY;
        
        for (int i = 0; i < sizeOfSample; i++) {    
            randomPercentage = (rand() % 100) / 100.00; // Get a random percentage 0.00 - 1.00
            // Get pointer to box which probability is the closest or is equal to the random percentage
            Box* chosenBox = getBox(randomPercentage);  

            if (chosenBox == nullptr) {         // This means that a new box will be created
                randomX = 30 + rand() % 1019;   
                randomY = 30 + rand() % 1019;
                if (isBackground(randomX,randomY)) 
                    continue;
                
                pixelCounter++;     // Increase pixel counter since we are creating a new random pixel
                totalBoxes++;       // Increase the amount of boxes since we are creating a new box
                Pixel randPixel = getNewPixel(randomX, randomY);
                Box* newBox = new Box(randPixel);
                boxes.emplace_back(newBox);     // Add probability and box to the table
                
            } else {    // A pixel will be added to the chosen box
                Pixel referencePixel = chosenBox->getReferencePixel(); // Reference pixel to get min and max coordinates
                
                minX = chosenBox->getMinCoords().getXAxis();
                maxX = chosenBox->getMaxCoords().getXAxis();
                minY = chosenBox->getMinCoords().getYAxis();
                maxY = chosenBox->getMaxCoords().getYAxis();

                randomX = abs(minX + (rand() % (maxX-minX)));   // Generate a random number in range min and max for each axis
                randomY = abs(minY + (rand() % (maxY-minY)));

                // Create a new pixel with the random axis within the range of the reference pixel
                Pixel newPixel = getNewPixel(randomX, randomY); 
                // Check if the color from the new pixel is similar to the reference pixel's color
                if (chosenBox->checkColor(newPixel)) {  
                    chosenBox->appendPixel(newPixel);   // Add pixel to the chosen box
                    pixelCounter++;
                }
            }
            updateTable();  // Update table with new information
        }
    }

};

#endif