#include <iostream>
#include "../../classes/probabilistic/Range.h"
#include "../../classes/probabilistic/Pixel.h"
#include "../../classes/probabilistic/Box.h"

using namespace std;

int main() {

    Range rango(10,50);

    if(rango.isInRange(25))
        cout << "true" << endl;
    else
        cout << "false" << endl;

    Pixel unPixel(512, 512, 128);
    Pixel unPixel2(300, 300, 114);
    Pixel unPixel3(200, 200, 112);

    cout << unPixel.getColor() << endl;

    Box caja1(unPixel);

    cout << endl;

    cout << caja1.getMinCoords().getXAxis() << endl;
    cout << caja1.getMinCoords().getYAxis() << endl;

    cout << endl;

    cout << caja1.getMaxCoords().getXAxis() << endl;
    cout << caja1.getMaxCoords().getYAxis() << endl;

    cout << endl;

    cout << caja1.getPixelCount() << endl; 

    cout << endl;

    if(caja1.appendPixel(unPixel2))
        cout << "true" << endl;
    else
        cout << "false" << endl;

    cout << endl;

    cout << caja1.getPixelCount() << endl; 

    cout << endl;

    if(caja1.appendPixel(unPixel3))
        cout << "true" << endl;
    else
        cout << "false" << endl;

    cout << endl;

    cout << caja1.getPixelCount() << endl; 

    cout << endl;


    return 0;
}
