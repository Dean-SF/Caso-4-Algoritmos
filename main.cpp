#include <iostream>
#include "classes/controller/Controller.h"

using namespace std;

int main () {
    Controller controlador("ref(1080).jpg");
    controlador.start();
    return 0;
}