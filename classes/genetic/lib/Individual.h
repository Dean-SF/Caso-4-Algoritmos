#ifndef _INDIVIDUAL_
#define _INDIVIDUAL_ 1

#define CROMO_MAX_VALUE 4294967296
#define NIBBLE_SIZE 32
#define MIN_GENOTYPE_SIZE_BY_PARENT 14

class Individual {
    private: 
        // any important related info
        // decide the size of your nibble for your problem
        float fitnessValue;
        unsigned int cromosoma;
    public:
        Individual(unsigned short pValue) {
            this->cromosoma = pValue;
        }

        unsigned short getCromosoma() {
            return this->cromosoma;
        }

        void setFitnessValue(float pValue) {
            this->fitnessValue = pValue;
        }

        float getFitnessValue() {
            return this->fitnessValue;
        }
};


#endif