#include <iostream>
#include <fstream>
#include <unistd.h>
#include <string>
#include <sstream>
#include <cstring>
#include "data.h"
#include <cstdlib>
#include <cstdio>
#include <csignal>


class ratio_index
{
public:
    ratio_index(double r, int i) : item_ratio{r}, index{i} {}

    double item_ratio;
    int index;
};

double computeValueWeightRatio(int value, int* weights, int nrOfResources)
{
    double sum = 0;
    for(int i = 0; i < nrOfResources; i++)
        sum += weights[i];
    return value/sum;
}


char* getOption(int argc, char* argv[], const char* option)
{
  for( int i = 0; i < argc; ++i)
    if(strcmp(argv[i], option) == 0 )
      return argv[i+1];
  return nullptr;
}

inline bool exists (const std::string& name) {
    return ( access( name.c_str(), F_OK ) != -1 );
}

void signalHandler( int signum ) {
   std::cout << "Running finalizing code. Interrupt signal (" << signum << ") received.\n";


   exit(signum);  
}

int main(int argc, char *argv[]) {

    // register signal SIGINT and signal handler  
    signal(SIGINT, signalHandler); 

    char* timelimit = getOption(argc, argv, std::string("-t").c_str());
    char* input = getOption(argc, argv, std::string("-i").c_str());

    if (timelimit == nullptr || input == nullptr){
        std::cout << "Parameters are not correctly specified!\n";
        std::exit(1);
    }

    int inttimelimit = atoi(timelimit);
    std::cout << "Instance name:" << input << "\n" ;
    std::cout << "Time limit:" << inttimelimit << "\n" ;
    std::string output(input);
    output.append(".out");
    std::cout << "Output name:" << output << "\n";

    data instance;
    instance.read_input(input);
    

    /* ******************** */
    /* Write your code here */
    /* ******************** */

    /*int value = 0;

    //valueWeightRatio[nrClasses][nrItems]
    //calculate ratio for each item
    ratio_index* valueWeightRatio[instance.nclasses][instance.nitems[0]];
    for (int i = 0; i < instance.nclasses; i++) {
        for (int j = 0; j < instance.nitems[i]; j++) {
            valueWeightRatio[i][j] = new ratio_index(computeValueWeightRatio(instance.values[i][j], &instance.weights[i][j * instance.nresources], instance.nresources), j);
        }
    }

    //sort items in each class by ratio
    for (int i = 0; i < instance.nclasses; i++) {
        std::sort(valueWeightRatio[i], valueWeightRatio[i] + instance.nitems[i], [](ratio_index* a, ratio_index* b) { return a->item_ratio > b->item_ratio; });
    }

    int sumOfWeightOfEachResource[instance.nresources];
    for (int i = 0; i < instance.nresources; i++) {
        sumOfWeightOfEachResource[i] = 0;
    }

    //calculate multiple kanpsack problem greedy
    for (int i = 0; i < instance.nclasses; i++) {
        for (int j = 0; j < instance.nitems[i]; j++) {
            int index = valueWeightRatio[i][j]->index;
            bool canBeAdded = true;
            for (int k = 0; k < instance.nresources; k++) {
                if (sumOfWeightOfEachResource[k] + instance.weights[i][index * instance.nresources + k] > instance.capacities[k]) {
                    canBeAdded = false;
                    break;
                }
            }
            if (canBeAdded) {
                instance.solution[i] = index;
                for (int k = 0; k < instance.nresources; k++) {
                    sumOfWeightOfEachResource[k] += instance.weights[i][index * instance.nresources + k];
                }
                value += instance.values[i][index];
                break;
            }
        }
    }

    //print sumOfWeightOfEachResource
    for (int i = 0; i < instance.nresources; i++) {
        std::cout << sumOfWeightOfEachResource[i] << " ";
    }

    //print value
    std::cout << "\nValue:" << value << "\n";*/

    int value = 0;

    // Calculate the value-to-weight ratio for each item
    ratio_index* valueWeightRatio[instance.nclasses][instance.nitems[0]];
    for (int i = 0; i < instance.nclasses; i++) {
        for (int j = 0; j < instance.nitems[i]; j++) {
            valueWeightRatio[i][j] = new ratio_index(computeValueWeightRatio(instance.values[i][j], &instance.weights[i][j * instance.nresources], instance.nresources), j);
        }
    }

    // Sort items in each class by ratio
    for (int i = 0; i < instance.nclasses; i++) {
        std::sort(valueWeightRatio[i], valueWeightRatio[i] + instance.nitems[i], [](ratio_index* a, ratio_index* b) { return a->item_ratio > b->item_ratio; });
    }

    // Initialize the sum of weights for each resource
    int sumOfWeightOfEachResource[instance.nresources];
    for (int i = 0; i < instance.nresources; i++) {
        sumOfWeightOfEachResource[i] = 0;
    }

    // Calculate the multiple knapsack problem using a greedy approach
    for (int i = 0; i < instance.nclasses; i++) {
        for (int j = 0; j < instance.nitems[i]; j++) {
            int index = valueWeightRatio[i][j]->index;
            bool canBeAdded = true;

            // Check if the item can be added to the knapsack without exceeding capacity
            // Check if the item can be added to the knapsack without exceeding capacity
            for (int k = 0; k < instance.nresources; k++) {
                if (sumOfWeightOfEachResource[k] + instance.weights[i][index * instance.nresources + k] > instance.capacities[k]) {
                    canBeAdded = false;
                    break;
                }
            }

            // If the item can be added, update the solution and the sum of weights
            if (canBeAdded) {
                instance.solution[i] = index;
                for (int k = 0; k < instance.nresources; k++) {
                    sumOfWeightOfEachResource[k] += instance.weights[i][index * instance.nresources + k];
                }
                value += instance.values[i][index];
                break;
            }
        }
    }

    for (int i = 0; i < instance.nresources; i++) {
        std::cout << sumOfWeightOfEachResource[i] << " ";
    }

    //print value
    std::cout << "\nValue:" << value << "\n";









    std::ofstream outfile;
    outfile.open(output, std::ios_base::out);
    for (auto i = 0; i<instance.nclasses; i++){
            outfile << instance.solution[i] << " ";
    }
    outfile.close();
}
