#include <stdio.h>
#include <stdlib.h>
#include <queue>
#include <vector>
#include "scheduler.hpp"
using namespace std;

/* performs analysis of complete tasks
 * AVG wait time, AVG response time, AVG turnaround time, total CPU usage
 * got from stack exchange
 * ARGS: 
 * queue<task>: a queue of tasks
 * got from stack exchange
 * RETURN: 
 * vector<double>: holds information
 /* number of tasks, the sums of "variable name" 
 */
vector<double> ComputeStatistics(queue<task> finishedTaskArray){
    double waitTime = 0, responseTime = 0, turnaroundTime = 0, totalCpuTime = 0; 
    int finishTime = 0;
    int tasks = finishedTaskArray.size(); 

 /* finds when last process finished, response time = startTime - arrivalTime, calculates cpu usage, 
 turnaround time = endTime - arrivalTime, wait time = endTime - arrivalTime - cpuTime 
 */
    while(!finishedTaskArray.empty()) {
        turnaroundTime += finishedTaskArray.front().endTime - finishedTaskArray.front().arrivalTime; /*  */
        waitTime += finishedTaskArray.front().endTime - finishedTaskArray.front().arrivalTime - finishedTaskArray.front().cpuTime;  
        totalCpuTime += finishedTaskArray.front().cpuTime; 
        responseTime += finishedTaskArray.front().startTime - finishedTaskArray.front().arrivalTime; 
        finishTime = max(finishTime, (int)finishedTaskArray.front().endTime); 
        finishedTaskArray.pop();
    }

 /* cpu usage = total CPU time / time when last task finishes 
 */
    vector<double> stats;
    stats.push_back(waitTime/tasks);
    stats.push_back(responseTime/tasks);
    stats.push_back(turnaroundTime/tasks);
    stats.push_back(totalCpuTime/(double)(finishTime)); 

    return stats;
}
