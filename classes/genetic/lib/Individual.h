#ifndef _INDIVIDUAL_
#define _INDIVIDUAL_ 1

/*
Basic structure to save every individual information
*/
class Individual {
    private: 
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