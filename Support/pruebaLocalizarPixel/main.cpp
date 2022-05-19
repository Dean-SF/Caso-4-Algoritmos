#include <iostream>
#include <unistd.h>
#include "../../libraries/CImg.h"

using namespace std;
using namespace cimg_library;

void changeRGBPixel(CImg<float> *image ,int x, int y, int r, int g, int b) {
    (*image)(x,y,0,0) = r;
    (*image)(x,y,0,1) = g;
    (*image)(x,y,0,2) = b;
}

void printRGBPixel(CImg<float> *image ,int x, int y) {
    cout << "R: " << *(image->data(x,y,0,0)) << " ";
    cout << "G: " << *(image->data(x,y,0,1)) << " ";
    cout << "B: " << *(image->data(x,y,0,2)) << endl;
}

int main() {

    CImg imagen("../../samples/ref.jpg");

    printRGBPixel(&imagen,256,375);

    changeRGBPixel(&imagen,0,0,0,0,0);


    CImgDisplay display(imagen,"SoylaImagen");
    while(!display.is_closed()) {
        sleep(0.5);
    }
   
    return 0;
}
