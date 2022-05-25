#ifndef _GENETICBASE_
#define _GENETICBASE_ 1
#include <map>
#include <ctime>
#include <vector>
#include "lib/Individual.h"
#include "lib/Cromodistribution.h"

using namespace std;

class GeneticBase {
    private:
        map<unsigned int,Cromodistribution*> *representation;
        vector<Individual*> *population;
        vector<Individual*> *fitnessPopulation;
        vector<Individual*> *unfitnessPopulation;
        int populationQuantity;
        int targetGenerations;


        void evaluateFitness() {
            fitnessPopulation->clear();
            unfitnessPopulation->clear();

            for(int i=0;i<population->size(); i++) {
               population->at(i)->setFitnessValue(fitness(population->at(i)));

                if (population->at(i)->getFitnessValue()>50) {  // fitness criteria of selection never will be an absolute value always is relative to the population
                    fitnessPopulation->push_back(population->at(i));
                } else {
                    unfitnessPopulation->push_back(population->at(i));
                }
            }
        }

        float fitness(Individual *pIndividual) {
            return rand()%100;
        }

        void reproduce(int pAmountOfChildrens) {
            // previous population will be cleared, full saved, partial saved depending on the problem
            population->clear();

            for(int i=0; i<pAmountOfChildrens; i++) {
                // select any two fitness parents
                int parent_a_index = rand()%fitnessPopulation->size();
                Individual* parent_a = fitnessPopulation->at(parent_a_index);

                int parent_b_index = rand()%fitnessPopulation->size();
                Individual* parent_b = fitnessPopulation->at(parent_b_index);

                population->push_back(cross(parent_a, parent_b));
            }
        }

        Individual* cross(Individual *pParent_a, Individual *pParent_b) {
            int cut_position = (rand() % (NIBBLE_SIZE-MIN_GENOTYPE_SIZE_BY_PARENT*2)) + MIN_GENOTYPE_SIZE_BY_PARENT;

            unsigned int mask_a = CROMO_MAX_VALUE - 1; // 4294967295 -> 11111111111111111111111111111111
            mask_a <<= cut_position;

            unsigned int mask_b = CROMO_MAX_VALUE - 1; // 4294967295 -> 11111111111111111111111111111111
            mask_b >>= NIBBLE_SIZE - cut_position;

            unsigned int kid = (pParent_a->getCromosoma() & mask_a) | (pParent_b->getCromosoma() & mask_b);

            double percentage = (rand() % 1000)/1000;
            if(percentage <=0.07) {
                unsigned int mutationMask = 2147483648;
                int bit = rand() % 31;
                mutationMask >>= bit;
                kid ^= mutationMask;
            }

            Individual *children = new Individual(kid);
            return children;
        }

    public:
        GeneticBase() {
            srand(time(0));
            this->population = new vector<Individual*>();
            this->fitnessPopulation = new vector<Individual*>();
            this->unfitnessPopulation = new vector<Individual*>();
            this->representation = new map<unsigned int,Cromodistribution*>(); 
            this->populationQuantity = 0;
            this->targetGenerations = 20;
        }

        void addDistribution(Cromodistribution* pDistribution, int32_t pRange) {
            representation->insert({pRange,pDistribution});
        }

        void initPopulation(int pAmountOfIndividuals) {
            population->clear();

            for(int i=0; i<pAmountOfIndividuals; i++) {
                Individual* p = new Individual((unsigned char) rand()%CROMO_MAX_VALUE);
                population->push_back(p);
            }
        }
        
        void produceGenerations(int ptargetGenerations, int pChildrensPerGenerations) {
            for(int i=0; i<ptargetGenerations; i++) {
                evaluateFitness();
                reproduce(pChildrensPerGenerations);
            }
        }

        vector<Individual*> getPopulation() {
            return *this->population;
        }
};

#endif