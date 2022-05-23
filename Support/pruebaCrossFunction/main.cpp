#include <iostream>
#include <unistd.h>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <bitset>
#include <random>

#define CROMO_MAX_VALUE 4294967296
#define NIBBLE_SIZE 32
#define MIN_GENOTYPE_SIZE_BY_PARENT 14

using namespace std;

typedef mt19937 rng_type;

uniform_int_distribution<unsigned int> udist(1,CROMO_MAX_VALUE-1);
rng_type rng;

void randomNumber1() {
    srand(time(0));
    unsigned int random;
    for(int i = 0; i < 50; i++) {
        random = 0 + rand() * ((CROMO_MAX_VALUE-1)-0) / RAND_MAX;
        bitset<32> randomb(random);
        cout << "Numero: " << random << " Bits: " << randomb << endl;
    }
}

void randomNumber2() {
    rng.seed(time(0));
    unsigned int random;
    for(int i = 0; i < 4000; i++) {
        random = udist(rng);
        bitset<32> randomb(random);
        cout << "Numero: " << random << " Bits: " << randomb << endl;
    }
}

int cross(int pParent1, int pParent2) {
    // this operation will depends on: nibble size, fixed o variable point
    srand(time(0));
    int cut_position = (rand() % (NIBBLE_SIZE-MIN_GENOTYPE_SIZE_BY_PARENT*2)) + MIN_GENOTYPE_SIZE_BY_PARENT;

    unsigned int mask_a = CROMO_MAX_VALUE - 1;
    mask_a <<= cut_position;

    unsigned int mask_b = CROMO_MAX_VALUE - 1;
    mask_b >>= NIBBLE_SIZE - cut_position;

    bitset<32> mask1(mask_a);
    bitset<32> mask2(mask_b);
    cout << "mask1: " << mask1 << " mask2: " << mask2 << endl;

    unsigned int kid = (pParent1 & mask_a) | (pParent2 & mask_b);

    return kid;
}

void pruebaCross() {
    unsigned int n1 = 324343434;
    unsigned int n2 = 234234324;
    bitset<32> n1b(n1);
    bitset<32> n2b(n2);
    int temp = cross(n1,n2);
    bitset<32> result(temp);
    cout << "NUMERO 1: " << n1b << endl; 
    cout << "NUMERO 2: " << n2b << endl; 
    cout << "NUMERO 3: " << result << endl; 
    cout << temp << endl;
}


void pruebaMutacion() {
    unsigned int n1 = 324343434;
    bitset<32> n1b(n1);
    srand(time(0));
    for(int i = 0; i < 4000;i++) {
        double percentage = (rand() % 1000)/1000.00;
        if(percentage <=0.07) {
            unsigned int mutationMask = 2147483648;
            int bit = rand() % 32;
            mutationMask >>= bit;
            bitset<32> mask(mutationMask);
            bitset<32> mutation = (n1^mutationMask);
            cout << "original: " << n1b << " muto: " << mask << " mutacion: " << mutation << endl;
        }
    }
}

int main () {
    pruebaCross();
    //pruebaMutacion();

}