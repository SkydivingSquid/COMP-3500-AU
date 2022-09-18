#ifndef _SCHEDULER_HPP
#define _SCHEDULER_HPP

typedef struct task {
    unsigned int pid;
    unsigned int arrivalTime;
    unsigned int cpuTime;
    unsigned int responseTime;
    unsigned int remainingTime;
    unsigned int startTime;
    unsigned int endTime;
    unsigned int waitTime;
}
task;
#endif
