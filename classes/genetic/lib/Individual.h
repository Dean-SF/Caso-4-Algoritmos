#ifndef _INDIVIDUAL_
#define _INDIVIDUAL_ 1

class Individual {
    private: 
        // any important related info
        // decide the size of your nibble for your problem
        double fitnessValue;
        unsigned int cromosoma;
    public:
        Individual(unsigned int pValue) {
            this->cromosoma = pValue;
        }

        unsigned int getCromosoma() {
            return this->cromosoma;
        }

        void setFitnessValue(double pValue) {
            this->fitnessValue = pValue;
        }

        double getFitnessValue() {
            return this->fitnessValue;
        }
};


#endif