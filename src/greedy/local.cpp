#include <iostream>
#include <fstream>
#include <unistd.h>
#include <string>
#include <cstring>
#include "data.h"
#include <cstdlib>
#include <csignal>

// weight_index class to store the sum of weights for each item in each class and its index
class weight_index {
public:
    weight_index(double w, int i) : weight_sum(w), index(i) {}

    double weight_sum;
    int index;
};

// function to check if the weight limit of a resource is exceeded
bool checkIfWeightIsExceeded(int* weights, int* addedWeights, int nrOfResources, std::vector<int> originalCapacities) {
    for(int i = 0; i < nrOfResources; i++)
        if(weights[i] + addedWeights[i] > originalCapacities[i])
            return true;
    return false;
}

// function to compute the sum of weights for a resource
double computeValueWeight(int* weights, int nrOfResources) {
    double sum = 0;
    for(int i = 0; i < nrOfResources; i++)
        sum += weights[i];
    return sum;
}

bool* getExeededValues(int* weights, int* addedWeights, int nrOfResources, std::vector<int> originalCapacities) {
    bool* result = new bool[nrOfResources];
    for(int i = 0; i < nrOfResources; i++)
        result[i] = weights[i] + addedWeights[i] > originalCapacities[i];
    return result;
}

int getNrOfExceededValues(int* weights, int* addedWeights, int nrOfResources, std::vector<int> originalCapacities) {
    int result = 0;
    for(int i = 0; i < nrOfResources; i++)
        if(weights[i] + addedWeights[i] > originalCapacities[i])
            result++;
    return result;
}

// function to get the value of an option passed in as a command line argument
char* getOption(int argc, char* argv[], const char* option) {
    for( int i = 0; i < argc; ++i)
        if(strcmp(argv[i], option) == 0 )
            return argv[i+1];
    return nullptr;
}

// function to check if a file exists
inline bool exists (const std::string& name) {
    return ( access( name.c_str(), F_OK ) != -1 );
}

// signal handler for interrupt signal
void signalHandler( int signum ) {
    std::cout << "Running finalizing code. Interrupt signal (" << signum << ") received.\n";
    exit(signum);
}

int main(int argc, char *argv[]) {

    // register signal SIGINT and signal handler
    signal(SIGINT, signalHandler);

    // get the time limit and input file name from command line arguments
    char *timelimit = getOption(argc, argv, std::string("-t").c_str());
    char *input = getOption(argc, argv, std::string("-i").c_str());

    // check if the options are specified
    if (timelimit == nullptr || input == nullptr) {
        std::cout << "Parameters are not correctly specified!\n";
        std::exit(1);
    }

    // convert time limit to integer
    int inttimelimit = atoi(timelimit);
    std::cout << "Instance name:" << input << "\n";
    std::cout << "Time limit:" << inttimelimit << "\n";

    // set the output file name
    std::string output(input);
    output.append(".out");
    std::cout << "Output name:" << output << "\n";

    // read the input data
    data instance;
    instance.read_input(input);

    // initialize the total value of the solution to 0
    int value = 0;

    // initialize the sum of weights for each item in each class
    weight_index **weightSum[instance.nclasses];
    for (int i = 0; i < instance.nclasses; i++) {
        weightSum[i] = new weight_index *[instance.nitems[i]];
    }

    // compute the sum of weights for each item in each class
    for (int i = 0; i < instance.nclasses; i++) {
        for (int j = 0; j < instance.nitems[i]; j++) {
            weightSum[i][j] = new weight_index(
                    computeValueWeight(&instance.weights[i][j * instance.nresources], instance.nresources), j);
        }
    }

    // sort the sum of weights for each item in each class
    for (int i = 0; i < instance.nclasses; i++) {
        //std::sort(weightSum[i], weightSum[i] + instance.nitems[i], [](weight_index* a, weight_index* b) { return a->weight_sum < b->weight_sum; });
        for (int j = 0; j < instance.nitems[i]; j++) {
            for (int k = j + 1; k < instance.nitems[i]; k++) {
                if (weightSum[i][j]->weight_sum > weightSum[i][k]->weight_sum) {
                    weight_index* temp = weightSum[i][j];
                    weightSum[i][j] = weightSum[i][k];
                    weightSum[i][k] = temp;
                }
            }
        }
    }

    // initialize the sum of weights for each resource to 0
    int sumOfWeightOfEachResource[instance.nresources];
    for (int i = 0; i < instance.nresources; i++) {
        sumOfWeightOfEachResource[i] = 0;
    }

    int remainingWeight[instance.nresources];
    for (int i = 0; i < instance.nresources; i++) {
        remainingWeight[i] = instance.capacities[i];
    }

    bool feasible = true;
    int c = 0;
    // resolve the multi-choice knapsack problem by taking the first element with the lowest weight
    for (int i = 0; i < instance.nclasses; i++) {
        for (int j = 0; j < instance.nitems[i]; j++) {
            if(!checkIfWeightIsExceeded(&instance.weights[i][weightSum[i][j]->index * instance.nresources], sumOfWeightOfEachResource, instance.nresources, instance.capacities)) {
                value += instance.values[i][weightSum[i][j]->index];
                instance.solution[i] = weightSum[i][j]->index;
                for (int k = 0; k < instance.nresources; k++) {
                    sumOfWeightOfEachResource[k] += instance.weights[i][weightSum[i][j]->index * instance.nresources + k];
                    remainingWeight[k] -= instance.weights[i][weightSum[i][j]->index * instance.nresources + k];
                }
                c++;
                break;
            }
            if (j == instance.nitems[i] - 1) {
                feasible = false;
                //Add the first element with the lowest weight
                value += instance.values[i][weightSum[i][0]->index];
                instance.solution[i] = weightSum[i][0]->index;
                for (int k = 0; k < instance.nresources; k++) {
                    sumOfWeightOfEachResource[k] += instance.weights[i][weightSum[i][0]->index * instance.nresources + k];
                    remainingWeight[k] -= instance.weights[i][weightSum[i][0]->index * instance.nresources + k];
                }
                c++;
            }
        }
    }

    /// UPON HERE SOLUTION SHOULD BE FEASIBLE, NOW WE TRY TO MAKE IT BETTER
    bool changedItems;
    do {
        changedItems = false;
        for (int i = 0; i < instance.nclasses; i++) {
            // remove actual weight
            for (int k = 0; k < instance.nresources; k++) {
                sumOfWeightOfEachResource[k] -= instance.weights[i][instance.solution[i] * instance.nresources + k];
                remainingWeight[k] += instance.weights[i][instance.solution[i] * instance.nresources + k];
            }
            for (int j = 0; j < instance.nitems[i]; j++) {
                if (!checkIfWeightIsExceeded(&instance.weights[i][weightSum[i][j]->index * instance.nresources],
                                             sumOfWeightOfEachResource, instance.nresources, instance.capacities)) {
                    if (instance.values[i][weightSum[i][j]->index] > instance.values[i][instance.solution[i]] || !feasible) {
                        value -= instance.values[i][instance.solution[i]];
                        value += instance.values[i][weightSum[i][j]->index];
                        instance.solution[i] = weightSum[i][j]->index;
                        changedItems = true;
                        feasible = true;
                        break;
                    }
                }
            }
            // if no item is feasible
            if (!feasible) {


                int oldSum = INT32_MAX;
                int sumOfNehativeWeights;
                int bestFitIndex = 0;


                //select a new item that has the lowest weight for the resource with the lowest remaining weight withouth making the other remaining weights negative
                for (int j = 0; j < instance.nitems[i]; j++) {
                    sumOfNehativeWeights = 0;
                    //add item to solution
                    for (int k = 0; k < instance.nresources; k++) {
                        remainingWeight[k] -= instance.weights[i][weightSum[i][j]->index * instance.nresources + k];
                        if (remainingWeight[k] < 0) {
                            sumOfNehativeWeights += abs(remainingWeight[k]);
                        }
                    }

                    if (sumOfNehativeWeights < oldSum) {
                        oldSum = sumOfNehativeWeights;
                        bestFitIndex = j;
                    }
                    
                    //remove item from solution
                    for (int k = 0; k < instance.nresources; k++) {
                        remainingWeight[k] += instance.weights[i][weightSum[i][j]->index * instance.nresources + k];
                    }
                }

                value -= instance.values[i][instance.solution[i]];
                value += instance.values[i][weightSum[i][bestFitIndex]->index];
                instance.solution[i] = weightSum[i][bestFitIndex]->index;
                changedItems = true;
            }

            // re-add actual weight
            for (int k = 0; k < instance.nresources; k++) {
                sumOfWeightOfEachResource[k] += instance.weights[i][instance.solution[i] * instance.nresources + k];
                remainingWeight[k] -= instance.weights[i][instance.solution[i] * instance.nresources + k];
            }
        }

    } while(changedItems);


    std::cout << "\n";

    // check if the solution is feasible
    if(feasible)
        std::cout << "Solution is feasible!\n";
    else {
        std::cout << "Solution is not feasible!\n";
        for (int i = 0; i < instance.nresources; i++) {
            std::cout << "Sum of weight of resource " << i << ": " << sumOfWeightOfEachResource[i] << " --> Remaining weight of resource " << i << ": " << remainingWeight[i] <<"\n";
        }
    }
    std::cout << "Value of solution: " << value << "\n";

    // write the solution to the output file
    std::ofstream outfile;
    outfile.open(output, std::ios_base::out);
    for (auto i = 0; i<instance.nclasses; i++){
        outfile << instance.solution[i] << " ";
    }
    outfile.close();
}