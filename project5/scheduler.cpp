#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <queue>
#include <vector>
#include <math.h>
#include "scheduler.hpp"
#include "read.hpp"
#include "open.hpp"
#include "print.hpp"
#include "compute.hpp"
#include "method.hpp"
using namespace std;
int main(int argc, char *argv[]) {
  char *file_name;
  FILE *fp;
  queue<task> task_list;
  queue<task> completed_tasks;
  if (argc <= 2 || argc >= 5) {
    printf("Usage: command file_name [FCFS|RR [time_quantum]|SRTF]\n");
    return 0;
  }
  int time_quantum = -1;
  if (strcmp(argv[2], "RR") == 0) {
    if (argc == 4) {
      sscanf(argv[3], "%d", &time_quantum);
      if (time_quantum < 1) {
        printf("Invalid time_quantum input. Please enter a value greater than or equal to 1.\n");
        return EXIT_FAILURE;
      }
      printf("time_quantum is set to %s\n", argv[3]);
    }
    else {
      printf("Please enter a value for time_quantum for the RR Scheduling Policy.\n");
      return EXIT_FAILURE;
    }
  }
  file_name = argv[1];
  fp = open(file_name);
  if (fp == NULL) {
    printf("File is invalid.\n");
    return EXIT_FAILURE;
  }
  printf("The name of the file is %s\n", argv[1]);
  if (!(strcmp(argv[2], "RR") == 0 || strcmp(argv[2], "FCFS") == 0 || strcmp(argv[2], "SRTF") == 0)) {
    printf("Invalid Scheduling Policy.\n");
    return EXIT_FAILURE;
  }
  printf("The selected Scheduling Policy is %s\n", argv[2]);
  task_list = readFile(fp);
  if (task_list.size() == 0) {
    printf("Invalid number of contents in file.\n");
    return EXIT_FAILURE;
  }
  printf("There are %u tasks loaded from %s. Press any key to continue ...", (unsigned int)task_list.size(), file_name);
  getchar();
  printf("==================================================================\n");
  fclose(fp);
  if (strcmp(argv[2], "RR") == 0) {
    completed_tasks = RoundRobin(task_list, time_quantum);
  }
  else if (strcmp(argv[2], "FCFS") == 0) {
    completed_tasks = FirstComeFirstServe(task_list);
  }
  else if(strcmp(argv[2], "SRTF") == 0) {
    completed_tasks = ShortestJobRemainingFirst(task_list);
  }
  else {
    printf("Usage: command file_name [FCFS|RR [time_quantum]|SRTF]\n");
  }
  vector<double> statistics = ComputeStatistics(completed_tasks);
  printf("==================================================================");
  DisplayStatistics(statistics);
  printf("==================================================================\n");
  return EXIT_SUCCESS;
}
