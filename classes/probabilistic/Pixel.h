#ifndef PIXEL_H
#define PIXEL_H

class Pixel {
private:
    int xAxis;
    int yAxis;
    unsigned char color;
public:
    Pixel(int pXAxis, int pYAxis) {
        xAxis = pXAxis;
        yAxis = pYAxis;
    }

    Pixel(int pXAxis, int pYAxis, unsigned char pColor) {
        xAxis = pXAxis;
        yAxis = pYAxis;
        color = pColor;
    }

    Pixel(int pXAxis, int pYAxis, unsigned char red, unsigned char green, unsigned char blue) {
        xAxis = pXAxis;
        yAxis = pYAxis;
        setColor(red,green,blue);
    }

    Pixel() {
        xAxis = 0;
        yAxis = 0;
        color = 0;
    }

    int getXAxis() {
        return xAxis;
    }

    int getYAxis() {
        return yAxis;
    }

    unsigned char getColor() {
        return color;
    }

    void setXAxis(int pXAxis) {
        xAxis = pXAxis;
    }

    void setYAxis(int pYAxis) {
        yAxis = pYAxis;
    }

    void setColor(unsigned char pColor) {
        color = pColor;
    }

    void setColor(unsigned char red, unsigned char green, unsigned char blue) {
        color =  0.299*red + 0.587*green + 0.114*blue;
    }

    void operator=(const Pixel &other) {
        xAxis = other.xAxis;
        yAxis = other.yAxis;
        color = other.color;
    }

};

#endif