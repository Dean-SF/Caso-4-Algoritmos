#ifndef GENETICALGORITHM_H
#define GENETICALGORITHM_H

#include "../probabilistic/Box.h"
#include "geneticbase.h"
#include "lib/GrayIntensity.h"
#include <map>
#include <utility>
#include "lib/Probability.h"
#include "lib/Individual.h"
#include "lib/Cromodistribution.h"
#include <cmath>

using std::map;
using std::pair;

#define PIXEL_SEPARATION BOX_SIZE/3


class GeneticAlgorithm {
private:
    int totalAxis;
    int totalPixels;
    Pixel minCoords;
    Pixel maxCoords;
    double rangeExcess;
    GeneticBase *genetic;
    vector<Box*> *sampling;
    map<int,Probability> xAxisAproximation;
    map<int,Probability> yAxisAproximation; 

    /*
    Calculates the middle coords for from the reduced area of the sampling
    */
    void setMiddleCoords() {
        genetic->setMiddleXAxis((minCoords.getXAxis()+maxCoords.getXAxis())/2);
        genetic->setMiddleYAxis((minCoords.getYAxis()+maxCoords.getYAxis())/2);
    }

    /*
    Sometimes, getting a percentage of the number 4294967296 give a value with decimals,
    in the long run, if we cut this percentages, we will lose 8500 values aprox, so this function
    saves the decimal part, and when this part is more or equal to 0, is given to the next range
    */
    unsigned int distrituteExcess(double pDecimalNumber) {
        unsigned int wholePart = (unsigned int)pDecimalNumber;
        rangeExcess += pDecimalNumber - wholePart;
        if(rangeExcess >= 1) {
            int rangeExcessWhole = rangeExcess;
            wholePart += rangeExcessWhole;
            rangeExcess = rangeExcess - rangeExcessWhole;
        }
        return wholePart;
    }

    /*
    this function converts a gray from 0 to 255, into 3
    different types of gray, dark = 45, mid = 128, light = 210
    */
    GrayIntensity getReduceColor(unsigned char pColor) {
        unsigned char darkGray = abs(pColor - dark);
        unsigned char midGray = abs(pColor - mid);
        unsigned char lightGray = abs(pColor - light);

        if(darkGray <= midGray && darkGray <= lightGray)
            return dark;

        if(midGray <= darkGray && midGray <= lightGray)
            return mid;
        
        if(lightGray <= darkGray && lightGray <= midGray)
            return light;

        return mid;
    }

    /*
    This fuction takes the min and max coordinates from the sampling
    to get a reduce square like area
    */
    void getMinMaxCoords() {
        for(Box *actual : *sampling) {
            if(actual->getMinCoords().getXAxis() < minCoords.getXAxis()) {
                minCoords.setXAxis(actual->getMinCoords().getXAxis());
            }

            if(actual->getMinCoords().getYAxis() < minCoords.getYAxis()) {
                minCoords.setYAxis(actual->getMinCoords().getYAxis());
            }

            if(actual->getMaxCoords().getXAxis() > maxCoords.getXAxis()) {
                maxCoords.setXAxis(actual->getMaxCoords().getXAxis());
            }

            if(actual->getMaxCoords().getYAxis() > maxCoords.getYAxis()) {
                maxCoords.setYAxis(actual->getMaxCoords().getYAxis());
            }
        }
    }

    /*
    This function takes the closest box from the sampling from only the xAxis
    or only the yAxis, and saves the amount of pixels it have. additionally, it finds
    the closest box for each color and saves the amount of pixels it has. All of this
    for future coordinates percentages calculations
    */
    Probability aproxColorDistanceProbability(int pAxisCoord, bool pIsXAxis) {
        int minDark = 1090, minMid = 1090, minLight = 1090, minAbsolute = 1090;
        Probability colorDistance;
        for(Box *actual : *sampling) {
            int distance;
            Pixel actualReference = actual->getReferencePixel();
            if(pIsXAxis) {
                distance = abs(actualReference.getXAxis()-pAxisCoord);
            } else {
                distance = abs(actualReference.getYAxis()-pAxisCoord);
            }
            GrayIntensity actualReduceColor = getReduceColor(actualReference.getColor());

            if(minAbsolute > distance) {
                minAbsolute = distance;
                colorDistance.amountClosest = actual->getPixelCount();
                colorDistance.distanceClosest = distance;
            }

            switch (actualReduceColor) {
                case dark:
                    if(minDark > distance) {
                        minDark = distance;
                        colorDistance.amountDark = actual->getPixelCount();
                    }
                    continue;
                case mid:
                    if(minMid > distance) {
                        minMid = distance;
                        colorDistance.amountMid = actual->getPixelCount();
                    }
                    continue;
                case light:
                    if(minLight > distance) {
                        minLight = distance;
                        colorDistance.amountLight = actual->getPixelCount();
                    }
                    continue;
                default:
                    continue;
            }
        }

        colorDistance.totalAmount = colorDistance.amountLight+colorDistance.amountMid+colorDistance.amountDark;
        totalAxis += colorDistance.amountClosest;

        return colorDistance;
    }

    /*
    this basically creates a Cromodistribution for given values
    */
    Cromodistribution createDistribution(int pXAxis, int pYAxis, GrayIntensity pColor, TypeOfShape pShape) {
        Cromodistribution newDistribution;
        newDistribution.xAxis = pXAxis;
        newDistribution.yAxis = pYAxis;
        newDistribution.color = pColor;
        newDistribution.shape = pShape;
        return newDistribution;
    }
    
    /*
    For each coordinate, we will be making 6 rows, for every type of shape (dot and line), 
    we will make a row for every color (dark, mid and light gray), making it in total, 6 rows,
    so the positional probability is distributed between each color according to amount of pixel
    of the closest box of that color for the coordinate we are calculating.
    */
    void addAllDistributions(int pXAxis, int pYAxis, long double pDarkProb, long double pMidProb, long double pLightProb, unsigned int *pLastRange) {
        *pLastRange += distrituteExcess((CROMO_MAX_VALUE-1)*(pDarkProb/2));
        genetic->addDistribution(createDistribution(pXAxis,pYAxis,dark,line),*pLastRange);
        *pLastRange += distrituteExcess((CROMO_MAX_VALUE-1)*(pMidProb/2));
        genetic->addDistribution(createDistribution(pXAxis,pYAxis,mid,line),*pLastRange);
        *pLastRange += distrituteExcess((CROMO_MAX_VALUE-1)*(pLightProb/2));
        genetic->addDistribution(createDistribution(pXAxis,pYAxis,light,line),*pLastRange);
        *pLastRange += distrituteExcess((CROMO_MAX_VALUE-1)*(pDarkProb/2));
        genetic->addDistribution(createDistribution(pXAxis,pYAxis,dark,dot),*pLastRange);
        *pLastRange += distrituteExcess((CROMO_MAX_VALUE-1)*(pMidProb/2));
        genetic->addDistribution(createDistribution(pXAxis,pYAxis,mid,dot),*pLastRange);
        *pLastRange += distrituteExcess((CROMO_MAX_VALUE-1)*(pLightProb/2));
        genetic->addDistribution(createDistribution(pXAxis,pYAxis,light,dot),*pLastRange);
    
    }


    /*
    We need a probability that goes from 0 to 1, for that we need to divide every probability by
    sum of all the probabilities, so this function gives the sum of all the probabilities
    */
    double getProbabilitySum() {
        double probabilitySum = 0;
        for(int xAxis = minCoords.getXAxis(); xAxis < maxCoords.getXAxis(); xAxis+=PIXEL_SEPARATION) {
            Probability xActual = xAxisAproximation.lower_bound(xAxis)->second;
            for(int yAxis = minCoords.getYAxis(); yAxis < maxCoords.getYAxis(); yAxis+=PIXEL_SEPARATION) {
                Probability yActual = yAxisAproximation.lower_bound(yAxis)->second; 
                int xAbs = xActual.amountClosest;
                int yAbs = yActual.amountClosest;
                double xDistance = xActual.distanceClosest;
                double yDistance = yActual.distanceClosest;
                double distanceCalibrator = ((xDistance+yDistance)/2.0);
                if(distanceCalibrator < 1)
                    distanceCalibrator = 1.0;
                distanceCalibrator = 1.0/distanceCalibrator;
                probabilitySum += (((xAbs + yAbs)/(double)totalAxis)*distanceCalibrator);
            }
        }
        return probabilitySum;
    }

    /*
    This function starts the process for the distribution of the nibble/cromosomatic representation.
    For this we will calculate aproximate boxes for all the area calculated from the sampling with 
    the necessary information to generate probability of each coordinate we want. 
    */
    void initDistribution() {
        for(int yAxis = minCoords.getYAxis(); yAxis < maxCoords.getYAxis(); yAxis+=BOX_SIZE+PIXEL_SEPARATION)
            yAxisAproximation.insert({yAxis+BOX_SIZE,aproxColorDistanceProbability(yAxis,false)});

        for(int xAxis = minCoords.getXAxis(); xAxis < maxCoords.getXAxis(); xAxis+=BOX_SIZE+PIXEL_SEPARATION)
            xAxisAproximation.insert({xAxis+BOX_SIZE,aproxColorDistanceProbability(xAxis,true)});
        
        double probabilitySum = getProbabilitySum();
        
        unsigned int lastRange = 0;
        for(int xAxis = minCoords.getXAxis(); xAxis < maxCoords.getXAxis(); xAxis+=PIXEL_SEPARATION) {
            for(int yAxis = minCoords.getYAxis(); yAxis < maxCoords.getYAxis(); yAxis+=PIXEL_SEPARATION) {
                Probability xAxisTable = xAxisAproximation.lower_bound(xAxis)->second;
                Probability yAxisTable = yAxisAproximation.lower_bound(yAxis)->second;
                int xAbsolute = xAxisTable.amountClosest;
                int yAbsolute = yAxisTable.amountClosest;

                double xDistance = xAxisTable.distanceClosest;
                double yDistance = yAxisTable.distanceClosest;
                double distanceCalibrator = ((xDistance+yDistance)/2.0);
                if(distanceCalibrator < 1)
                    distanceCalibrator = 1.0;
                distanceCalibrator = 1.0/distanceCalibrator;

                int totalAmount = xAxisTable.totalAmount + yAxisTable.totalAmount;

                long double partialDarkProb= (xAxisTable.amountDark + yAxisTable.amountDark)/(double)totalAmount;
                long double partialMidProb = (xAxisTable.amountMid + yAxisTable.amountMid)/(double)totalAmount;
                long double partialLightProb= (xAxisTable.amountLight + yAxisTable.amountLight)/(double)totalAmount;

                long double coordProbability = (((xAbsolute + yAbsolute)/(double)totalAxis)*distanceCalibrator)/(double)probabilitySum;
                long double darkProb = coordProbability*partialDarkProb;
                long double midProb = coordProbability*partialMidProb;
                long double lightProb = coordProbability*partialLightProb;

                addAllDistributions(xAxis,yAxis,darkProb,midProb,lightProb,&lastRange);
            }
        }
    }
public:
    GeneticAlgorithm() {
        genetic = new GeneticBase();
        minCoords.setXAxis(1090); 
        minCoords.setYAxis(1090);
        maxCoords.setXAxis(0);
        maxCoords.setYAxis(0);
        totalAxis = 0;
        rangeExcess = 0;
    }
    ~GeneticAlgorithm() {
        delete genetic;
    }

    /*
    Process to start the genetic algorithm
    */
    void work(vector<Box*> *pSampling, int pTotalPixels) {
        sampling = pSampling;
        totalPixels = pTotalPixels;

        sampling->erase(sampling->begin());
        getMinMaxCoords();
        setMiddleCoords();

        initDistribution();

        genetic->initPopulation(200);

        genetic->produceGenerations(100,500);

    }


};


#endif