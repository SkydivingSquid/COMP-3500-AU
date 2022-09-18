#ifndef _METHOD_HPP
#define _METHOD_HPP

/* INFO: First Come First Serve scheduling policy
 */
std::queue<task> FirstComeFirstServe(std::queue<task> taskArray);

/* INFO: Round Robin scheduling policy
 */
std::queue<task> RoundRobin(std::queue<task> taskArray, int time_quantum);

/* INFO: Finds shortest job remaining
 */
int shortestJobRemaining(std::vector<task> taskQueue);

/* INFO: Shortest Remaining Task First scheduling policy
 */
std::queue<task> ShortestJobRemainingFirst(std::queue<task> taskArray);
#endif