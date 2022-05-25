#include <iostream>
#include <unistd.h>
#include <cmath>

using namespace std;

int grayScale(unsigned char red, unsigned char green, unsigned char blue) {
        return 0.299*red + 0.587*green + 0.114*blue;
    }

double colorDistance(int pRed1, int pBlue1, int pGreen1, int pRed2, int pBlue2, int pGreen2) {
    int rmean = (pRed1 + pRed2) / 2;
    int red = pRed1 - pRed2;
    int blue = pBlue1 - pBlue2;
    int green = pGreen1 - pGreen2;
    return sqrt( ( ( (512+rmean)*red*red ) >> 8) + 4*green*green + ( ( (767-rmean)*blue*blue) >> 8 ) );
}

int main() {
    cout << colorDistance(255,255,255,219,218,216) << endl;
    cout << colorDistance(255,255,255,241, 199, 177) << endl;
    cout << colorDistance(255,255,255,255, 255, 255) << endl;

    cout << grayScale(241, 199, 177) << endl;
    cout << grayScale(219,218,216) << endl;
    cout << grayScale(255, 255, 255) << endl;

    cout << colorDistance(217,212,209,219, 218, 216) << endl;
    cout << colorDistance(217,212,209,241, 199, 177) << endl;
    

    return 0;
}
