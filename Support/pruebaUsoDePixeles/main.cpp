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

void changeToGrayScale(CImg<float> *image ,int x, int y) {
    int grayscale = 0.299*(*(image->data(x,y,0,0))) + 0.587*(*(image->data(x,y,0,1))) + 0.114*(*(image->data(x,y,0,2)));
    (*image)(x,y,0,0) = grayscale;
    (*image)(x,y,0,1) = grayscale;
    (*image)(x,y,0,2) = grayscale;
}

int main() {

    CImg imagen("../../samples/repre(1080).jpg");

    for(int i = 0; i < 1080; i++) {
        for(int j = 0; j < 1080; j++) {
            changeToGrayScale(&imagen,i,j);
        }
    }

    for(int i = 256; i <= 860; i+=20) {
        for(int j = 140; j <= 980; j+=20) {
            changeRGBPixel(&imagen,i,j,175,0,0);
        }
    }

    imagen.save("../../samples/nueva.jpg");

    CImgDisplay display(imagen,"SoylaImagen");
    while(!display.is_closed()) {
        sleep(0.5);
    }
   
    return 0;
}
