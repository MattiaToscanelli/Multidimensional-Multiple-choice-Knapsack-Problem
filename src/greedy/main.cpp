#include <iostream>
#include <fstream>
#include <unistd.h>
#include <string>
#include <sstream>
#include <cstring>
#include "data.h"
#include <cstdlib>
#include <csignal>

class weight_index
{
public:
    weight_index(double w, int i) : weight_sum{w}, index{i} {}

    double weight_sum;
    int index;
};

bool checkIfWeightIsExceeded(int* weights, int* addedWeights, int nrOfResources, std::vector<int> originalCapacities)
{
    for(int i = 0; i < nrOfResources; i++)
        if(weights[i] + addedWeights[i] > originalCapacities[i])
            return true;
    return false;
}

double computeValueWeight(int* weights, int nrOfResources)
{
    double sum = 0;
    for(int i = 0; i < nrOfResources; i++)
        sum += weights[i];
    return sum;
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

    int value = 0;

    // Initialize the sum of weights for each item in each class
    weight_index* weightSum[instance.nclasses][instance.nitems[0]];
    for (int i = 0; i < instance.nclasses; i++) {
        for (int j = 0; j < instance.nitems[i]; j++) {
            weightSum[i][j] = new weight_index(computeValueWeight(&instance.weights[i][j * instance.nresources], instance.nresources), j);
        }
    }

    // Sort the sum of weights for each item in each class
    for (int i = 0; i < instance.nclasses; i++) {
        std::sort(weightSum[i], weightSum[i] + instance.nitems[i], [](weight_index* a, weight_index* b) { return a->weight_sum < b->weight_sum; });
    }

    // Initialize the sum of weights for each resource
    int sumOfWeightOfEachResource[instance.nresources];
    for (int i = 0; i < instance.nresources; i++) {
        sumOfWeightOfEachResource[i] = 0;
    }

    int c = 0;

    // Resolve multiochoice knapsack problem take the first element with the lowest weight
    for (int i = 0; i < instance.nclasses; i++) {
        for (int j = 0; j < instance.nitems[i]; j++) {
            if(!checkIfWeightIsExceeded(&instance.weights[i][weightSum[i][j]->index * instance.nresources], sumOfWeightOfEachResource, instance.nresources, instance.capacities)) {
                value += instance.values[i][weightSum[i][j]->index];
                instance.solution[i] = weightSum[i][j]->index;
                for (int k = 0; k < instance.nresources; k++) {
                    sumOfWeightOfEachResource[k] += instance.weights[i][weightSum[i][j]->index * instance.nresources + k];
                }
                c++;
                break;
            }
        }
    }

    /// UPON HERE SOLUTION SHOULD BE FEASIBLE, NOW WE TRY TO MAKE IT BETTER

    bool changedItems;
    do {
        changedItems = false;
        for (int i = 0; i < instance.nclasses; i++) {
            for (int j = 0; j < instance.nitems[i]; j++) {
                // remove actual weight
                for (int k = 0; k < instance.nresources; k++) {
                    sumOfWeightOfEachResource[k] -= instance.weights[i][instance.solution[i] * instance.nresources + k];
                }
                if (!checkIfWeightIsExceeded(&instance.weights[i][weightSum[i][j]->index * instance.nresources],
                                             sumOfWeightOfEachResource, instance.nresources, instance.capacities)) {
                    if (instance.values[i][weightSum[i][j]->index] > instance.values[i][instance.solution[i]]) {
                        value -= instance.values[i][instance.solution[i]];
                        value += instance.values[i][weightSum[i][j]->index];
                        for (int k = 0; k < instance.nresources; k++) {
                            sumOfWeightOfEachResource[k] += instance.weights[i][
                                    weightSum[i][j]->index * instance.nresources + k];
                        }
                        instance.solution[i] = weightSum[i][j]->index;
                        changedItems = true;
                        break;
                    }
                }
                // re-add actual weight
                for (int k = 0; k < instance.nresources; k++) {
                    sumOfWeightOfEachResource[k] += instance.weights[i][instance.solution[i] * instance.nresources + k];
                }
            }
        }


    } while(changedItems);


    for (int i = 0; i < instance.nclasses; i++) {
        std::cout << instance.solution[i] << " ";
    }
    std::cout << "\n" << std::endl;
    //print all weights
    for (int i = 0; i < instance.nresources; i++) {
        std::cout << "Weight of resource: " << sumOfWeightOfEachResource[i] << "\n";
    }

    if(c==instance.nclasses)
        std::cout << "Solution is feasible!\n";
    else
        std::cout << "Solution is not feasible!\n" << "Number of classes: " << instance.nclasses << " Number of classes in solution: " << c << "\n";


    std::cout << "Value of solution: " << value << "\n";


    std::ofstream outfile;
    outfile.open(output, std::ios_base::out);
    for (auto i = 0; i<instance.nclasses; i++){
        outfile << instance.solution[i] << " ";
    }
    outfile.close();
}