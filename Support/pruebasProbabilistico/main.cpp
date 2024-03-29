#include <iostream>
#include <vector>
#include <time.h>
#include <algorithm>
#include <unistd.h>
#include "../../libraries/CImg.h"
#include "../../classes/probabilistic/ProbabilisticAlgorithm.h"

using namespace std;
using namespace cimg_library;

int main() {
    CImg<unsigned char> pic("../../samples/ref(1080).jpg");
    ProbabilisticAlgorithm alg(0.085, &pic);
    alg.work();

    return 0;
}