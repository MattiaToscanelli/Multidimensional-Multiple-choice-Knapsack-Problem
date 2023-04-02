#include <iostream>
#include <fstream>
#include <unistd.h>
#include <string>
#include <sstream>
#include <cstring>
#include "data.h"
#include <cstdlib>
#include <csignal>

int findBestFit(const data& instance, const std::vector<std::vector<int>>& remaining_capacity, const std::vector<int>& item, int nclass) {
    int bestFit = -1;
    for (int r = 0; r < instance.nresources; r++) {
        bool feasible = true;
        for (int c = 0; c < instance.nclasses; c++) {
            if (c != nclass && remaining_capacity[r][c] < instance.weights[c][item[c]]) {
                feasible = false;
                break;
            }
        }
        if (feasible && remaining_capacity[r][nclass] >= instance.weights[nclass][item[nclass]]) {
            if (bestFit == -1 || remaining_capacity[r][nclass] < remaining_capacity[bestFit][nclass]) {
                bestFit = r;
            }
        }
    }
    return bestFit;
}

// Funzione che risolve il problema dello zaino multidimensionale con assegnamento simultaneo a tutti i knapsack
void solveMultidimensionalKnapsack(data& instance) {
    std::vector<std::vector<int>> remaining_capacity(instance.nresources, std::vector<int>(instance.nclasses, 0));
    for (int r = 0; r < instance.nresources; r++) {
        for (int c = 0; c < instance.nclasses; c++) {
            remaining_capacity[r][c] = instance.capacities[r];
        }
    }
    std::vector<std::pair<int, int>> items;
    for (int c = 0; c < instance.nclasses; c++) {
        for (int i = 0; i < instance.nitems[c]; i++) {
            items.push_back(std::make_pair(instance.values[c][i], c * instance.nitems[c] + i));
        }
    }
    sort(items.begin(), items.end(), std::greater<std::pair<int, int>>());
    instance.solution.assign(items.size(), -1);
    for (int k = 0; k < items.size(); k++) {
        int item_id = items[k].second;
        int c = item_id / instance.nitems[0];
        int i = item_id % instance.nitems[0];
        int r = findBestFit(instance, remaining_capacity, {c, i}, c);
        if (r != -1) {
            instance.solution[k] = item_id;
            for (int c = 0; c < instance.nclasses; c++) {
                remaining_capacity[r][c] -= instance.weights[c][i];
            }
        }
    }
}



class ratio_index
{
public:
    ratio_index(double r, int i) : item_ratio{r}, index{i} {}

    double item_ratio;
    int index;
};

bool checkIfWeightIsExceeded(int* weights, std::vector<int> capacities, int nrOfResources)
{
    for(int i = 0; i < nrOfResources; i++)
        if(weights[i] > capacities[i])
            return true;
    return false;
}

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

    int c = 0;


    //fill sumOfWeightOfEachResource with the max ratio
    for (int i = 0; i < instance.nclasses; i++) {
        int index = valueWeightRatio[i][0]->index;
        instance.solution[i] = index;
        for (int k = 0; k < instance.nresources; k++) {
            sumOfWeightOfEachResource[k] += instance.weights[i][index * instance.nresources + k];
        }
        value += instance.values[i][index];
        c++;
    }

    //try the combinations to not exceed the capacity
    for (int i = 0; i < instance.nclasses; i++) {
        for (int j = 1; j < instance.nitems[i]; j++) {
            int index = valueWeightRatio[i][j]->index;

        }
    }*/

    solveMultidimensionalKnapsack(instance);



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
