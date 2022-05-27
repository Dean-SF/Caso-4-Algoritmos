#ifndef BOX_H
#define BOX_H

#define BOX_SIZE 60
#define GRAY_RANGE 15

#include <iostream>
#include "Pixel.h"
#include <vector>
#include <cmath>

using std::vector;
using std::cout;
using std::endl;

/*
Class for pixel storage
*/
class Box {
private:
    Pixel minPixelCoords;
    Pixel maxPixelCoords;
    vector<Pixel> pixelGroup;

public:
    // constructors
    Box() {

    }

    Box(const Box &other) {
        minPixelCoords = other.minPixelCoords;
        maxPixelCoords = other.maxPixelCoords;
        pixelGroup = other.pixelGroup;
    }

    Box(Pixel pReferencePixel) {
        minPixelCoords.setXAxis(pReferencePixel.getXAxis() - BOX_SIZE/2.0);
        minPixelCoords.setYAxis(pReferencePixel.getYAxis() - BOX_SIZE/2.0);
        maxPixelCoords.setXAxis(pReferencePixel.getXAxis() + BOX_SIZE/2.0);
        maxPixelCoords.setYAxis(pReferencePixel.getYAxis() + BOX_SIZE/2.0);
        pixelGroup.emplace_back(pReferencePixel);
    }

    // setters & getters
    Pixel getMinCoords() {
        return minPixelCoords;
    }

    Pixel getMaxCoords() {
        return maxPixelCoords;
    }

    Pixel getReferencePixel() {
        return pixelGroup[0];
    }

    vector<Pixel> getPixelGroup() {
        return pixelGroup;
    }

    int getPixelCount() {
        return pixelGroup.size();
    }

    void setMinCoords(Pixel pMinCoords) {
        minPixelCoords = pMinCoords;
    }

    void setMaxCoords(Pixel pMaxCoords) {
        minPixelCoords = pMaxCoords;
    }

    void setReferencePixel(Pixel pPixel) {
        pixelGroup[0] = pPixel;
    }

    /*
    Method to calculate the distance of a coodinate to the center of the box
    */
    double distance(int pXAxis, int pYAxis) {
        Pixel reference = getReferencePixel();
        return sqrt(pow(pXAxis-reference.getXAxis(),2)-pow(pYAxis-reference.getYAxis(),2));
    }

    /*
    Method to check if the pixel color is in range
    */
    bool checkColor(Pixel pPixel) {
        unsigned char reference = pixelGroup[0].getColor();
        if (reference - GRAY_RANGE <= pPixel.getColor() && pPixel.getColor() <= reference + GRAY_RANGE) 
            return true;
        return false;
    }

    /*
    Method to append a pixel to the vector
    */
    void appendPixel(Pixel pPixel) {
        pixelGroup.emplace_back(pPixel);
    }
    
    /*
    Method for printing the vector
    */
    void printPixels() {
        for (int i = 0; i < getPixelCount(); i++) {
            cout << "(" << pixelGroup[i].getXAxis() << "," << pixelGroup[i].getYAxis() << ") : " << (int)(pixelGroup[i].getColor()) << endl;
        }
    }
};

#endif