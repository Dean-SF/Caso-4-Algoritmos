#include <iostream>
#include <unistd.h>
#include "../../classes/genetic/cromodistribution.h"
#include <vector>
#include <cstdlib>
#include <ctime>

using namespace std;

void firstTry() {
    int tabla[50];
    int suma = 0;
    double total = 0;
    srand(time(0));
    for(int i = 0; i < 50; i++) {
        tabla[i] = rand();
        suma += tabla[i];
    }
    
    for(int i = 0; i < 50; i++) {
        total += 1 - (tabla[i]/(double)suma);
    }

    for(int i = 0; i < 50; i++) {
       cout << tabla[i] << ", ";
    }
    cout << endl;

    cout << total << endl;
}

void secondTry() {
    const int size = 3;
    int tabla[] = {1,1,1};
    int suma = 0;
    double total = 0;
    srand(time(0));

    for(int i = 0; i < size; i++) {
       suma += tabla[i];
    }
    cout << "Suma: " << suma << endl;
    for(int i = 0; i < size; i++) {
        total += 1 - (tabla[i]/(double)suma);
        cout << 1 - (tabla[i]/(double)suma) << ", ";
    }
    cout << endl;
    cout << endl;
    cout << "----- tabla ----" << endl;
    for(int i = 0; i < size; i++) {
       cout << tabla[i] << ", ";
    }
    cout << endl;

    cout << total << endl;
}

double getInverseProbability(int count, double total, double totalElements) {
    if(totalElements == 1) {
        return 1;
    }
    totalElements--;
    return (1.0-(count/total))/totalElements;
}

void thirdTry() {
    const int size = 3;
    int tabla[] = {1,5,8};
    int suma = 0;
    for(int i = 0; i < size; i++) {
       suma += tabla[i];
    }
    for(int i = 0; i < size; i++) {
       cout << "Para el numero: " << tabla[i];
       cout << " Hay una probabilidad de: " << getInverseProbability(tabla[i],suma,size) << endl;
    }

}

void thirdTryRand() {
    const int size = 30;
    int tabla[size];
    int suma = 0;
    double total = 0;
    srand(time(0)*time(0));

    for(int i = 0; i < size; i++) {
        tabla[i] = rand();
        suma += tabla[i];
    }

    double esTotal = 0;
    for(int i = 0; i < size; i++) {
       cout << "Para el numero: " << tabla[i];
       double porcentaje = getInverseProbability(tabla[i],suma,size);
       cout << " Hay una probabilidad de: " << porcentaje<< endl;
       esTotal += porcentaje;
    }

    cout << "Suma: " << esTotal << endl;
}

int main () {
    thirdTryRand(); 
}