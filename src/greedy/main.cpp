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

bool checkIfWeightIsExceeded(int* weights, std::vector<int> capacities, int nrOfResources)
{
    for(int i = 0; i < nrOfResources; i++)
        if(weights[i] > capacities[i])
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

    weight_index* weightSum[instance.nclasses][instance.nitems[0]];
    for (int i = 0; i < instance.nclasses; i++) {
        for (int j = 0; j < instance.nitems[i]; j++) {
            weightSum[i][j] = new weight_index(computeValueWeight(&instance.weights[i][j * instance.nresources], instance.nresources), j);
        }
    }

    // Sort items in each class by weight ascending
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
            if(!checkIfWeightIsExceeded(&instance.weights[i][weightSum[i][j]->index * instance.nresources], instance.capacities, instance.nresources)){
                value += instance.values[i][weightSum[i][j]->index];
                for (int k = 0; k < instance.nresources; k++) {
                    sumOfWeightOfEachResource[k] += instance.weights[i][weightSum[i][j]->index * instance.nresources + k];
                }
                c++;
                break;
            }
        }
    }

    //print all weights
    for (int i = 0; i < instance.nresources; i++) {
        std::cout << "Weight: " << sumOfWeightOfEachResource[i] << "\n";
    }

    std::cout << "Value: " << value << " c:" << c << "\n";


    for (int i = 0; i < instance.nresources; i++) {
        std::cout << instance.solution[i] << " ";
    }

    //print value


    std::ofstream outfile;
    outfile.open(output, std::ios_base::out);
    for (auto i = 0; i<instance.nclasses; i++){
            outfile << instance.solution[i] << " ";
    }
    outfile.close();
}
