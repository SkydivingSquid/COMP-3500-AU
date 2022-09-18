#include <stdlib.h>
#include <stdio.h>
#include <queue>
#include <vector>
#include "scheduler.hpp"

using namespace std;

/* First Come First Serve scheduling 
 * got from stack exchange
 * ARGS: 
 * queue<task>: a queue of tasks whos running process will be simulated
 * got from stack exchange
 * RETURN: 
 * queue<task>: a queue of tasks whose simulation is finished
 * 
 */

queue<task> FirstComeFirstServe(queue<task> taskArray){
    unsigned int clock = 0;
    queue<task> readyQueue;
    queue<task> finishedTaskArray;

    /* adds tasks after current time */
    while(!taskArray.empty() || !readyQueue.empty()){
        while(!taskArray.empty() && taskArray.front().arrivalTime <= clock){
            readyQueue.push(taskArray.front());
            taskArray.pop();
        }

        /* run task sim if queue is not empty and start first task*/
        if(!readyQueue.empty()){
            if(readyQueue.front().remainingTime==readyQueue.front().cpuTime){
                readyQueue.front().startTime=clock;
            }
            readyQueue.front().remainingTime--;
            printf("<time %u> process %u is running\n", clock, readyQueue.front().pid);

            /* if current task is complete, rest task */
            if(readyQueue.front().remainingTime==0){
                readyQueue.front().endTime=clock+1;
                finishedTaskArray.push(readyQueue.front());
                printf("<time %u> process %u is finished...\n", clock+1, readyQueue.front().pid);
                readyQueue.pop();
                
                /*all tasks are complete and program finishes if both queues empty, else initialize next process */
                if(readyQueue.empty() && taskArray.empty()){
                    printf("<time %u> All processes finished...\n", clock+1);
                    return finishedTaskArray;
                } else if( !readyQueue.empty() && readyQueue.front().remainingTime==readyQueue.front().cpuTime){ 
                    readyQueue.front().startTime=clock;
                }
            }
        } else {
            /* else no process active */
            printf("<time %u> No process is running\n", clock);
        }
        clock++;
    }
    return finishedTaskArray;
}

/*
 * got from stack exchange
 * ARGS: 
 * queue<task>: a queue of tasks whos running process will be simulated
 * int: maximum execution time of a process
 * got from stack exchange
 * RETURN: 
 *  queue<task>: a queue of tasks whose simulation is finished
 */
queue<task> RoundRobin(queue<task> taskArray, int time_quantum){
    int last_switch_clock = 0; 
    unsigned int clock = 0;
    queue<task> readyQueue;
    queue<task> finishedTaskArray;
    while(!taskArray.empty() || !readyQueue.empty()){

        /* add tasks after current time */
        while(!taskArray.empty() && taskArray.front().arrivalTime <= clock){
            readyQueue.push(taskArray.front());
            taskArray.pop();
        }

        /* run task sim if vector not empty, switch process if clock is greater equal to time quantum */
        if(!readyQueue.empty()){
            if(last_switch_clock >= time_quantum){
                readyQueue.push(readyQueue.front());
                readyQueue.pop();
                last_switch_clock = 0;
            }

            /* starts at front*/
            if(readyQueue.front().remainingTime == readyQueue.front().cpuTime){
                readyQueue.front().startTime = clock;
            }
            readyQueue.front().remainingTime--;
            printf("<time %u> process %u is running\n", clock, readyQueue.front().pid);

            /* rest current task if complete */
            if(readyQueue.front().remainingTime == 0){
                readyQueue.front().endTime = clock+1;
                finishedTaskArray.push(readyQueue.front());
                printf("<time %u> process %u is finished...\n", clock+1, readyQueue.front().pid);
                readyQueue.pop();

                /* resets last_switch_clock. */
                last_switch_clock = -1;

                /* copmlete if both queues are empty, restarts loop, starts next task,   */
                if(readyQueue.empty() && taskArray.empty()){
                    printf("<time %u> All processes finished...\n", clock+1);
                    return finishedTaskArray;
                } else if(readyQueue.empty() && !taskArray.empty()){ 
                    continue;
                }else if(readyQueue.front().remainingTime==readyQueue.front().cpuTime){ 
                    readyQueue.front().startTime=clock;
                } else if(readyQueue.front().remainingTime == 0){ 
                    continue;
                }
            }
        } else {
            last_switch_clock = -1;
            printf("<time %u> No process is running\n", clock);
        }
        clock++;
        last_switch_clock++;
    }
    return finishedTaskArray;
}

/*Finds shortest job remaining
 * ARGS: 
 *  vector<task>: a vector of tasks who are currently being simulated
 * RETURN: 
 *  int: index of vector that has shortest time remaining
 */
int shortestJobRemaining(vector<task> taskQueue){
    int shortestTimeRemaining = -1;
    int index = -1;
    int i = 0;

    /* return -1 if empty */
    if(taskQueue.empty()){
        return index;
    }

    /* loops through tasks */
    while(i<taskQueue.size()) { 
        if(taskQueue.at(i).remainingTime < shortestTimeRemaining){
            shortestTimeRemaining = taskQueue.at(i).remainingTime;
            index = i;
        }
        i++;
    }
    return index;
}

/*Shortest Remaining Task First scheduling policy
 * ARGS: 
 *  queue<task>: a queue of tasks whos running process will be simulated
 * RETURN: 
 *  queue<task>: a queue of tasks whose simulation is finished
 */
queue<task> ShortestJobRemainingFirst(queue<task> taskArray){

    unsigned int clock = 0;
    vector<task> readyQueue;
    queue<task> finishedTaskArray;
    int sjr; /* indexer for shortest remaining task, adds tasks */
    while(!taskArray.empty() || !readyQueue.empty()){
        while(!taskArray.empty() && taskArray.front().arrivalTime <= clock){
            readyQueue.push_back(taskArray.front());
            taskArray.pop();
            sjr = shortestJobRemaining(readyQueue);
        }

        /* run sim if vector not empty and start process pointed at */
        if(!readyQueue.empty()){
            if(readyQueue.at(sjr).remainingTime == readyQueue.at(sjr).cpuTime){
                readyQueue.at(sjr).startTime = clock;
            }
            readyQueue.at(sjr).remainingTime--;
            printf("<time %u> process %u is running\n", clock, readyQueue.at(sjr).pid);

            /* rest if task complete*/
            if(readyQueue.at(sjr).remainingTime == 0){
                readyQueue.at(sjr).endTime = clock+1;
                finishedTaskArray.push(readyQueue.at(sjr));
                printf("<time %u> process %u is finished...\n", clock+1, readyQueue.at(sjr).pid);
                readyQueue.erase(readyQueue.begin()+sjr);

                /* find  shortest task */
                sjr = shortestJobRemaining(readyQueue);

                /* restart loop if task complete */
                if(sjr < 0 || readyQueue.at(sjr).remainingTime == 0){
                    continue;
                }
            }
        } else {
            printf("<time %u> No process is running\n", clock);
        }
        clock++;
    }
  printf ("<time %u> All processes are finished...\n", clock+1);
    
return finishedTaskArray;
}
