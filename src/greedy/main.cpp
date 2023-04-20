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

    int c = 0;
    // resolve the multi-choice knapsack problem by taking the first element with the lowest weight
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

    // check if the solution is feasible
    if(c==instance.nclasses)
        std::cout << "Solution is feasible!\n";
    else
        std::cout << "Solution is not feasible!\n" << "Number of classes: " << instance.nclasses << " Number of classes in solution: " << c << "\n";
    std::cout << "Value of solution: " << value << "\n";

    // write the solution to the output file
    std::ofstream outfile;
    outfile.open(output, std::ios_base::out);
    for (auto i = 0; i<instance.nclasses; i++){
        outfile << instance.solution[i] << " ";
    }
    outfile.close();
}