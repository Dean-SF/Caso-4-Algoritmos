#ifndef _GENETICBASE_
#define _GENETICBASE_ 1
#include <windows.h>
#include <map>
#include <ctime>
#include <cmath>
#include <vector>
#include <random>
#include "lib/Individual.h"
#include "lib/Cromodistribution.h"
#include "../socket/socketclient.h"

#define PIXEL_SEPARATION BOX_SIZE/3
#define PIXEL_IDEAL_NEIGHBOURS 25

using namespace std;

uniform_int_distribution<unsigned int> distributor(0,CROMO_MAX_VALUE-1);
typedef mt19937 randomType;

class GeneticBase {
    private:
        map<unsigned int,Cromodistribution> *representation;
        map<double,Individual*> *rulette;
        vector<Individual*> *population;
        vector<Individual*> *fitnessPopulation;
        vector<Individual*> *unfitnessPopulation;
        int populationQuantity;
        int targetGenerations;
        randomType random;
        socketclient client;


        void evaluateFitness() {
            fitnessPopulation->clear();
            unfitnessPopulation->clear();
            
            double totalFitness = 0;
            double worstIndividualFitness = 0;
            double bestIndividualFitness = 1080;
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

            double fitnessCandidateValue = (totalFitness/population->size());
            fitnessCandidateValue = (bestIndividualFitness+fitnessCandidateValue)/2;

            for(Individual *actual : *population) {
                
                if(actual->getFitnessValue() < fitnessCandidateValue) {
                    fitnessPopulation->emplace_back(actual);
                } else {
                    unfitnessPopulation->emplace_back(actual);
                }
            }
        }

        double getDistance(Cromodistribution pFirstIndiv,Cromodistribution pSecondIndiv) {
            return sqrt(pow(pSecondIndiv.xAxis-pFirstIndiv.xAxis,2)+pow(pSecondIndiv.yAxis-pFirstIndiv.yAxis,2));
        }

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
                /*if(compareIndividual.color != actualIndividual.color)
                    continue;*/ // si colores diferentes
                /*if(compareIndividual.xAxis == actualIndividual.xAxis && compareIndividual.yAxis == actualIndividual.yAxis)
                    continue;*/ //si son iguales
                nearbyIndividuals += 1;
            }

            return abs(PIXEL_IDEAL_NEIGHBOURS-nearbyIndividuals);
        }

        /*
        double fitness(Individual *pIndividual) {
            int totalSizeCluster = 0;
            double distanceSum = 0;
            Cromodistribution actualIndividual = representation->lower_bound(pIndividual->getCromosoma())->second;
            for(Individual *actual : *population) {
                Cromodistribution compareIndividual = representation->lower_bound(actual->getCromosoma())->second;
                if(compareIndividual.color != actualIndividual.color) {
                    continue;
                }
                distanceSum += getDistance(actualIndividual,compareIndividual);
                totalSizeCluster++;
            }
            return distanceSum/totalSizeCluster;
        }*/

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
            //population->insert(population->end(),fitnessPopulation->begin(),fitnessPopulation->end());
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
                    Sleep(10);
                }
                client.closeConnection();
                //Sleep(5000);
        }
        void printPopulation() {
            for(Individual *actual : *population) {
                Cromodistribution IndivRepr = representation->lower_bound(actual->getCromosoma())->second;
                cout << endl << "Posicion: " << IndivRepr.xAxis << ", " << IndivRepr.yAxis << endl;
                cout << "Fitness Value: " << actual->getFitnessValue() << endl;
            }
            cout << endl;
        }

    public:
        GeneticBase() {
            srand(time(0));
            random.seed(time(0));
            this->population = new vector<Individual*>();
            this->fitnessPopulation = new vector<Individual*>();
            this->unfitnessPopulation = new vector<Individual*>();
            this->representation = new map<unsigned int,Cromodistribution>(); 
            this->rulette = new map<double,Individual*>();
            this->populationQuantity = 0;
            this->targetGenerations = 50;
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
            for(int i=0; i<ptargetGenerations; i++) {
                evaluateFitness();
                //cout << endl << fitnessPopulation->size() << endl;
                //cout << unfitnessPopulation->size() << endl;
                if((i+1) >= ptargetGenerations)
                    printPopulation();
                //paintGeneration();                
                reproduce(pChildrensPerGenerations);
            }
            paintGeneration();
            //printPopulation();  
        }

        vector<Individual*> getPopulation() {
            return *this->population;
        }

        map<unsigned int,Cromodistribution> *getRepresentation() {
            return representation;
        }
};

#endif