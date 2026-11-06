#include "scheduler.h"
#include <iomanip>
#include <iostream>

static void printResult(const ScheduleResult& r, Algorithm algo) {
    std::cout << "\n== " << toString(algo) << " ==\nTimeline: ";
    for (int id : r.timeline) std::cout << (id == -1 ? "idle" : "P" + std::to_string(id)) << " ";
    std::cout << "\nID  Arr  Burst  Prio  First  Done  Turn  Wait  Resp\n";
    for (const auto& sp : r.processes)
        std::cout << std::setw(2) << sp.p.id << std::setw(5) << sp.p.arrival << std::setw(7) << sp.p.burst
                  << std::setw(6) << sp.p.priority << std::setw(7) << sp.firstRun << std::setw(6) << sp.completion
                  << std::setw(6) << sp.turnaround << std::setw(6) << sp.waiting << std::setw(6) << sp.response << "\n";
    std::cout << std::fixed << std::setprecision(2)
              << "Avg waiting=" << r.avgWaiting << "  Avg turnaround=" << r.avgTurnaround
              << "  Avg response=" << r.avgResponse << "  Idle=" << r.idleTime << "  Makespan=" << r.makespan << "\n";
}

int main() {
    // Worked example from README Step 7.
    std::vector<Process> jobs = {{1, 0, 5, 3}, {2, 1, 3, 1}, {3, 2, 1, 2}};
    printResult(schedule(jobs, Algorithm::FCFS), Algorithm::FCFS);
    printResult(schedule(jobs, Algorithm::SJF), Algorithm::SJF);
    printResult(schedule(jobs, Algorithm::SRTF), Algorithm::SRTF);
    printResult(schedule(jobs, Algorithm::ROUND_ROBIN, 2), Algorithm::ROUND_ROBIN);
    printResult(schedule(jobs, Algorithm::PRIORITY), Algorithm::PRIORITY);
    return 0;
}
