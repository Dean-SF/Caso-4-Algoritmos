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
    vector<Box*> *sampling;
    GeneticBase *genetic;
    int totalPixels;
    Pixel minCoords;
    Pixel maxCoords;
    int totalAxis;
    double rangeExcess;
    map<int,Probability> xAxisAproximation;
    map<int,Probability> yAxisAproximation; 

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

    Probability aproxColorDistanceProbability(int axisCoord, bool isXAxis) {
        int minDark = 1090, minMid = 1090, minLight = 1090, minAbsolute = 1090;
        Probability colorDistance;
        for(Box *actual : *sampling) {
            int distance;
            Pixel actualReference = actual->getReferencePixel();
            if(isXAxis) {
                distance = abs(actualReference.getXAxis()-axisCoord);
            } else {
                distance = abs(actualReference.getYAxis()-axisCoord);
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

    Cromodistribution createDistribution(int pXAxis, int pYAxis, GrayIntensity pColor, TypeOfShape pShape) {
        Cromodistribution newDistribution;
        newDistribution.xAxis = pXAxis;
        newDistribution.yAxis = pYAxis;
        newDistribution.color = pColor;
        newDistribution.shape = pShape;
        return newDistribution;
    }
    
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
                /*if(0 <= xAxis && xAxis <= 100 && 0 <= yAxis && yAxis <= 100) {
                cout << endl << distanceCalibrator << endl;
                cout << (xAbs + yAbs/(double)totalAxis) << endl;
                cout << (xAbs + yAbs/(double)totalAxis)*distanceCalibrator << endl;}*/
                probabilitySum += (((xAbs + yAbs)/(double)totalAxis)*distanceCalibrator);
            }
        }
        //cout << "si" << probabilitySum << endl;
        return probabilitySum;
    }

    void initDistribution() {
        for(int yAxis = minCoords.getYAxis(); yAxis < maxCoords.getYAxis(); yAxis+=BOX_SIZE+PIXEL_SEPARATION)
            yAxisAproximation.insert({yAxis+BOX_SIZE,aproxColorDistanceProbability(yAxis,false)});

        for(int xAxis = minCoords.getXAxis(); xAxis < maxCoords.getXAxis(); xAxis+=BOX_SIZE+PIXEL_SEPARATION)
            xAxisAproximation.insert({xAxis+BOX_SIZE,aproxColorDistanceProbability(xAxis,true)});
        
        double probabilitySum = getProbabilitySum();
        
        double suma = 0;
        unsigned int lastRange = 0;
        for(int xAxis = minCoords.getXAxis(); xAxis < maxCoords.getXAxis(); xAxis+=PIXEL_SEPARATION) {
            for(int yAxis = minCoords.getYAxis(); yAxis < maxCoords.getYAxis(); yAxis+=PIXEL_SEPARATION) {
                Probability xAxisTable = xAxisAproximation.lower_bound(xAxis)->second;
                Probability yAxisTable = yAxisAproximation.lower_bound(yAxis)->second;
                int xAbsolute = xAxisTable.amountClosest;
                int yAbsolute = yAxisTable.amountClosest;

                int totalAmount = xAxisTable.totalAmount + yAxisTable.totalAmount;

                //cout << totalAmount << endl;

                long double partialDarkProb= (xAxisTable.amountDark + yAxisTable.amountDark)/(double)totalAmount;
                long double partialMidProb = (xAxisTable.amountMid + yAxisTable.amountMid)/(double)totalAmount;
                long double partialLightProb= (xAxisTable.amountLight + yAxisTable.amountLight)/(double)totalAmount;

                double xDistance = xAxisTable.distanceClosest;
                double yDistance = yAxisTable.distanceClosest;
                double distanceCalibrator = ((xDistance+yDistance)/2.0);
                if(distanceCalibrator < 1)
                    distanceCalibrator = 1.0;
                distanceCalibrator = 1.0/distanceCalibrator;

                long double coordProbability = (((xAbsolute + yAbsolute)/(double)totalAxis)*distanceCalibrator)/(double)probabilitySum;
                suma += coordProbability;
                long double darkProb = coordProbability*partialDarkProb;
                long double midProb = coordProbability*partialMidProb;
                long double lightProb = coordProbability*partialLightProb;

                addAllDistributions(xAxis,yAxis,darkProb,midProb,lightProb,&lastRange);
                /*
                cout << endl << "TOTAL PROBABILITY: " << coordProbability << endl;
                cout << "-- Probability Distribution ---" << endl;
                cout << "Probability For Dark: " << darkProb << endl;
                cout << "Probability For Mid: " << midProb << endl;
                cout << "Probability For Light: " << lightProb << endl;
                cout << "Probability Sum: " << darkProb + midProb + lightProb << endl;*/
            }
        }
        //cout << suma << endl;
        /*
        int xAxis = 27*PIXEL_SEPARATION;
        int yAxis = 27*PIXEL_SEPARATION;
        Probability xAxisTable = xAxisAproximation.lower_bound(xAxis)->second;
        Probability yAxisTable = yAxisAproximation.lower_bound(yAxis)->second;
        int xAbsolute = xAxisTable.amountClosest;
        int yAbsolute = yAxisTable.amountClosest;

        int totalAmount = xAxisTable.totalAmount + yAxisTable.totalAmount;

        //cout << totalAmount << endl;

        long double partialDarkProb= (xAxisTable.amountDark + yAxisTable.amountDark)/(double)totalAmount;
        long double partialMidProb = (xAxisTable.amountMid + yAxisTable.amountMid)/(double)totalAmount;
        long double partialLightProb= (xAxisTable.amountLight + yAxisTable.amountLight)/(double)totalAmount;

        double xDistance = xAxisTable.distanceClosest;
        double yDistance = yAxisTable.distanceClosest;
        double distanceCalibrator = ((xDistance+yDistance)/2.0);
        if(distanceCalibrator==0)
            distanceCalibrator = 1.0;
        distanceCalibrator = 1.0/distanceCalibrator;

        long double coordProbability = (((xAbsolute + yAbsolute)/(double)totalAxis)*distanceCalibrator)/(double)probabilitySum;

        long double darkProb = coordProbability*partialDarkProb;
        long double midProb = coordProbability*partialMidProb;
        long double lightProb = coordProbability*partialLightProb;

        //addAllDistributions(xAxis,yAxis,darkProb,midProb,lightProb,&lastRange);
        
        cout << endl << "TOTAL PROBABILITYaaa: " << coordProbability << endl;
        cout << "-- Probability Distribution ---" << endl;
        cout << "Probability For Dark: " << darkProb << endl;
        cout << "Probability For Mid: " << midProb << endl;
        cout << "Probability For Light: " << lightProb << endl;
        cout << "Probability Sum: " << darkProb + midProb + lightProb << endl;*/
    }

    void printRepresentation() {
        map<unsigned int,Cromodistribution> *representation = genetic->getRepresentation();
        for(pair<unsigned int,Cromodistribution> actual : *representation) {
            cout << actual.first << ", ";
        }
        cout << endl;
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

    void work(vector<Box*> *pSampling, int pTotalPixels) {
        sampling = pSampling;
        totalPixels = pTotalPixels;

        sampling->erase(sampling->begin());
        getMinMaxCoords();

        cout << "minCoords: (" << minCoords.getXAxis() << ", " << minCoords.getYAxis() << ")" << endl;
        cout << "maxCoords: (" << maxCoords.getXAxis() << ", " << maxCoords.getYAxis() << ")" << endl;
        cout << sampling->size() << endl;

        initDistribution();

        genetic->initPopulation(200);

        genetic->produceGenerations(100,500);

        //printRepresentation();
    }


};


#endif