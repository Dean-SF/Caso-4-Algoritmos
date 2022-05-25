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

    CImg imagen("../../samples/ref(1080).jpg");

    printRGBPixel(&imagen,1,1);
    printRGBPixel(&imagen,50,51);
    printRGBPixel(&imagen,50,52);
    printRGBPixel(&imagen,50,53);

    changeRGBPixel(&imagen,555,645,0,0,0);


    CImgDisplay display(imagen,"SoylaImagen");
    display.resize(900,900,true);
    while(!display.is_closed()) {
        sleep(0.5);
    }
   
    return 0;
}
