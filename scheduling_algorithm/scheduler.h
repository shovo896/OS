#pragma once
#include <string>
#include <vector>

// Step 1 - Process model (input)
// priority: LOWER number = HIGHER priority (1 beats 3). Ignored by non-priority algorithms.
struct Process {
    int id;
    int arrival;   // time unit when process becomes ready, >= 0
    int burst;     // total CPU time needed, > 0
    int priority = 0;
};

// Step 2 - Metrics (output, filled in by the scheduler)
struct ScheduledProcess {
    Process p;
    int firstRun = -1;
    int completion = -1;
    int turnaround = -1; // completion - arrival
    int waiting = -1;    // turnaround - burst
    int response = -1;   // firstRun - arrival
};

struct ScheduleResult {
    std::vector<ScheduledProcess> processes; // same order as input
    std::vector<int> timeline;               // timeline[t] = id running at t, -1 = idle
    double avgWaiting = 0.0;
    double avgTurnaround = 0.0;
    double avgResponse = 0.0;
    int idleTime = 0;
    int makespan = 0; // time when last process completes
};

enum class Algorithm { FCFS, SJF, SRTF, ROUND_ROBIN, PRIORITY };

// quantum is only used by ROUND_ROBIN and must be > 0 there.
// Throws std::invalid_argument on empty input, burst <= 0, arrival < 0,
// duplicate ids, or quantum <= 0 for Round Robin.
ScheduleResult schedule(const std::vector<Process>& input, Algorithm algo, int quantum = 1);

std::string toString(Algorithm algo);
