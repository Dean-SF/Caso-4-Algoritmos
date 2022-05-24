#ifndef BOX_H
#define BOX_H

#define BOX_SIZE 60
#define GRAY_RANGE 15

#include <iostream>
#include "Pixel.h"
#include <vector>

using std::vector;
using std::cout;
using std::endl;

class Box {
private:
    Pixel minPixelCoords;
    Pixel maxPixelCoords;
    vector<Pixel> pixelGroup;

public:
    Box() {
    }

    Box(Pixel pReferencePixel) {
        minPixelCoords.setXAxis(pReferencePixel.getXAxis() - BOX_SIZE/2.0);
        minPixelCoords.setYAxis(pReferencePixel.getYAxis() - BOX_SIZE/2.0);
        maxPixelCoords.setXAxis(pReferencePixel.getXAxis() + BOX_SIZE/2.0);
        maxPixelCoords.setYAxis(pReferencePixel.getYAxis() + BOX_SIZE/2.0);
        pixelGroup.emplace_back(pReferencePixel);
    }

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

    bool checkColor(Pixel pPixel) {
        unsigned char reference = pixelGroup[0].getColor();
        if (reference - GRAY_RANGE <= pPixel.getColor() && pPixel.getColor() <= reference + GRAY_RANGE) 
            return true;
        return false;
    }

    void appendPixel(Pixel pPixel) {
        pixelGroup.emplace_back(pPixel);
    }

    void printPixels() {
        for (int i = 0; i < getPixelCount(); i++) {
            cout << "(" << pixelGroup[i].getXAxis() << "," << pixelGroup[i].getYAxis() << ")" << endl;
        }
    }
};

#endif