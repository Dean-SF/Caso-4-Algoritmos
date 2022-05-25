#include <iostream>
#include <map>
#include <random>
#include <ctime>

using namespace std;

int main () {
    srand(time(0));
    
    map<double,string> mapa;

    mapa.insert({0.30,"NEW BOX"});
    mapa.insert({0.51,"C1"});
    mapa.insert({0.69,"C2"});
    mapa.insert({1.00,"C3"});

    for(int i = 0; i < 40; i++) {
        double randDouble = (rand()%1000)/1000.00;
        string pr = (*mapa.lower_bound(randDouble)).second;

        cout << "Para el numero: " << randDouble << " la caja: " << pr << endl;
    }
    return 0;
}