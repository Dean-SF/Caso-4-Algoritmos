#ifndef _GENETICBASE_
#define _GENETICBASE_ 1
#include <unistd.h>
#include <map>
#include <ctime>
#include <cmath>
#include <vector>
#include <random>
#include <limits.h>
#include <float.h>
#include "lib/Individual.h"
#include "lib/Cromodistribution.h"
#include "../socket/socketclient.h"

#define PIXEL_SEPARATION BOX_SIZE/3
#define PIXEL_IDEAL_NEIGHBOURS 8

using namespace std;

/*
pseudo random number generator, generates numbers from 0 to 4294967295
*/
uniform_int_distribution<unsigned int> distributor(0,CROMO_MAX_VALUE-1);
typedef mt19937 randomType;

class GeneticBase {
    private:
        int middleYAxis;
        int middleXAxis;
        randomType random;
        socketclient client;
        int targetGenerations;
        int populationQuantity;
        vector<Individual*> *population;
        vector<Individual*> *fitnessPopulation;
        vector<Individual*> *unfitnessPopulation;
        map<unsigned int,Cromodistribution> *representation;
        
        /*
        Function to evaluate fitness, first it calculates the fitness of every individual
        and gets the sum of every individual, the worst individual fitness and the best individual fitness,
        next we get the number in between the worst individual and the best individual, then we get the number
        in between that middle number and the best individual. The individuals that are better than that
        number, are our fitness population.
        */
        void evaluateFitness() {
            fitnessPopulation->clear();
            unfitnessPopulation->clear();
            
            double totalFitness = 0;
            double worstIndividualFitness = 0;
            double bestIndividualFitness = DBL_MAX;
            for(Individual *actual : *population) {
                actual->setFitnessValue(fitness(actual));
                totalFitness += actual->getFitnessValue();
                if(actual->getFitnessValue() > worstIndividualFitness) {
                    worstIndividualFitness = actual->getFitnessValue();
                }

                if(actual->getFitnessValue() < bestIndividualFitness) {
                    bestIndividualFitness = actual->getFitnessValue();
                }
            }

            double fitnessCandidateValue = (bestIndividualFitness+worstIndividualFitness)/2.0;
            fitnessCandidateValue = (bestIndividualFitness+fitnessCandidateValue)/2.0;

            for(Individual *actual : *population) {
                
                if(actual->getFitnessValue() < fitnessCandidateValue) {
                    fitnessPopulation->emplace_back(actual);
                } else {
                    unfitnessPopulation->emplace_back(actual);
                }
            }
        }

        // function to get the distance between two points
        double getDistance(int pFirstXAxis, int pFirstYAxis, int pSecondXAxis, int pSecondYAxis) {
            return sqrt(pow(pFirstXAxis-pSecondXAxis,2)+pow(pFirstYAxis-pSecondYAxis,2));
        }

        /*
        The fitness function, we look for individuals nearby the individual we are calculating the fitness
        the ideal is to get a number close to "PIXEL_IDEAL_NEIGHBOURS", so we calculate the distance between the
        amount of neighbours and the ideal amount of neighbours, then we calculate the distance to the center, and
        multiplicate both numbers to get our fitness value. the less is this number, the better.
        */
        double fitness(Individual *pIndividual) {
            Cromodistribution actualIndividual = representation->lower_bound(pIndividual->getCromosoma())->second;
            int nearbyIndividuals = 0;
            int minXAxis = actualIndividual.xAxis - (PIXEL_SEPARATION*2);
            int minYAxis = actualIndividual.yAxis - (PIXEL_SEPARATION*2);
            int maxXAxis = actualIndividual.xAxis + (PIXEL_SEPARATION*2);
            int maxYAxis = actualIndividual.yAxis + (PIXEL_SEPARATION*2);

            for(Individual *actual : *population) {
                Cromodistribution compareIndividual = representation->lower_bound(actual->getCromosoma())->second;

                if(!(minXAxis <= compareIndividual.xAxis && compareIndividual.xAxis <= maxXAxis)) 
                    continue;

                if(!(minYAxis <= compareIndividual.yAxis && compareIndividual.yAxis <= maxYAxis))
                    continue;

                if(compareIndividual.xAxis == actualIndividual.xAxis && compareIndividual.yAxis == actualIndividual.yAxis) {
                    nearbyIndividuals += 3;
                    continue;
                }
                    
                nearbyIndividuals += 1;
            }
            double result = abs(PIXEL_IDEAL_NEIGHBOURS-nearbyIndividuals);
            result = getDistance(actualIndividual.xAxis,actualIndividual.yAxis,middleXAxis,middleYAxis)*result;
            return result;
        }

        /*
        Basic reproduction algorithm
        */
        void reproduce(int pAmountOfChildrens) {
            population->clear();
            for(int i=0; i<pAmountOfChildrens; i++) {
                int parent_a_index = rand()%fitnessPopulation->size();
                Individual* parent_a = fitnessPopulation->at(parent_a_index);

                int parent_b_index = rand()%fitnessPopulation->size();
                Individual* parent_b = fitnessPopulation->at(parent_b_index);

                population->push_back(cross(parent_a, parent_b));
            }
        }

        /*
        Basic cross algorithm, with 3% mutation rate
        */
        Individual* cross(Individual *pParent_a, Individual *pParent_b) {
            int cut_position = (rand() % (NIBBLE_SIZE-MIN_GENOTYPE_SIZE_BY_PARENT*2)) + MIN_GENOTYPE_SIZE_BY_PARENT;

            unsigned int mask_a = CROMO_MAX_VALUE - 1; // 4294967295 -> 11111111111111111111111111111111
            mask_a <<= cut_position;

            unsigned int mask_b = CROMO_MAX_VALUE - 1; // 4294967295 -> 11111111111111111111111111111111
            mask_b >>= NIBBLE_SIZE - cut_position;

            unsigned int kid = (pParent_a->getCromosoma() & mask_a) | (pParent_b->getCromosoma() & mask_b);

            double percentage = (rand() % 1000)/1000;
            if(percentage <=0.03) {
                unsigned int mutationMask = 2147483648;
                int bit = rand() % 31;
                mutationMask >>= bit;
                kid ^= mutationMask;
            }

            Individual *children = new Individual(kid);
            return children;
        }

        /*
        Method used to send to the java painter, the population to draw.
        */
        void paintGeneration() {
            client.init();
                client.clear();
                for(Individual *actual : *population) {
                    //cout << actual->getCromosoma() << endl;
                    Cromodistribution IndivRepr = representation->lower_bound(actual->getCromosoma())->second;
                    if(IndivRepr.shape == line) {
                        int firstXAxis = IndivRepr.xAxis-5;
                        int firstYAxis = IndivRepr.yAxis-5;
                        int secondXAxis = IndivRepr.xAxis+5;
                        int secondYAxis = IndivRepr.yAxis+5;
                        client.paintLineGray(IndivRepr.color,firstXAxis,firstYAxis,secondXAxis,secondYAxis);
                    } else {
                        client.paintDotGray(IndivRepr.color,IndivRepr.xAxis,IndivRepr.yAxis,10);
                    }
                    Sleep(80);
                }
                client.closeConnection();
                Sleep(6000);
        }

    public:
        GeneticBase() {
            srand(time(0));
            random.seed(time(0));
            this->population = new vector<Individual*>();
            this->fitnessPopulation = new vector<Individual*>();
            this->unfitnessPopulation = new vector<Individual*>();
            this->representation = new map<unsigned int,Cromodistribution>(); 
            this->populationQuantity = 0;
            this->targetGenerations = 50;
            this->middleXAxis = 0;
            this->middleYAxis = 0;
        }

        void setMiddleXAxis(int pMiddleXAxis) {
            middleXAxis = pMiddleXAxis;
        }

        void setMiddleYAxis(int pMiddleYAxis) {
            middleYAxis = pMiddleYAxis;
        }

        void addDistribution(Cromodistribution pDistribution, unsigned int pRange) {
            representation->insert({pRange,pDistribution});
        }

        void initPopulation(int pAmountOfIndividuals) {
            population->clear();

            for(int i=0; i<pAmountOfIndividuals; i++) {
                Individual* p = new Individual(distributor(random));
                population->push_back(p);
            }
        }
        
        void produceGenerations(int ptargetGenerations, int pChildrensPerGenerations) {
            paintGeneration(); // initial Population
            for(int i=0; i<ptargetGenerations; i++) {
                evaluateFitness();
                //paintGeneration();                
                reproduce(pChildrensPerGenerations);
            }
            paintGeneration(); // final population
        }

        vector<Individual*> getPopulation() {
            return *this->population;
        }

        map<unsigned int,Cromodistribution> *getRepresentation() {
            return representation;
        }
};

#endif