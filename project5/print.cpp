#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include "scheduler.hpp"

using namespace std;
/*
 * Print a task list. To display the task list, the length of which is specified by size.
 */
void DisplayStatistics(vector<double> stats) {
    printf("\n");
    printf("average wait time: %.2f\n", stats.at(0));
    printf("average response time: %.2f\n", stats.at(1));
    printf("average turnaround time: %.2f\n", stats.at(2));
    printf("total cpu usage: %.2f % \n", stats.at(3)*100);
}