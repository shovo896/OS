#include "scheduler.h"
#include <cassert>
#include <cmath>
#include <iostream>

static bool near(double a, double b) { return std::fabs(a - b) < 1e-9; }
static std::vector<int> completions(const ScheduleResult& r) {
    std::vector<int> v;
    for (auto& sp : r.processes) v.push_back(sp.completion);
    return v;
}

int main() {
    const std::vector<Process> jobs = {{1, 0, 5, 3}, {2, 1, 3, 1}, {3, 2, 1, 2}};

    // FCFS: 1,2,3 -> done 5,8,9 ; waiting 0,4,6
    auto fcfs = schedule(jobs, Algorithm::FCFS);
    assert((completions(fcfs) == std::vector<int>{5, 8, 9}));
    assert(near(fcfs.avgWaiting, 10.0 / 3));

    // SJF: 1,3,2 -> done 5,9,6 ; waiting 0,5,3
    auto sjf = schedule(jobs, Algorithm::SJF);
    assert((completions(sjf) == std::vector<int>{5, 9, 6}));
    assert(near(sjf.avgWaiting, 8.0 / 3));

    // SRTF: P1(0),P2(1),P3(2),P2(3-4),P1(5-7) -> done 8,5,3 ; waiting 3,1,0
    auto srtf = schedule(jobs, Algorithm::SRTF);
    assert((completions(srtf) == std::vector<int>{8, 5, 3}));
    assert(near(srtf.avgWaiting, 4.0 / 3));
    assert(srtf.processes[2].response == 0);

    // Round Robin q=2: P1,P2,P3,P1,P2,P1 -> done 9,8,5 ; waiting 4,4,2
    auto rr = schedule(jobs, Algorithm::ROUND_ROBIN, 2);
    assert((completions(rr) == std::vector<int>{9, 8, 5}));
    assert(near(rr.avgWaiting, 10.0 / 3));

    // Priority (lower=higher): 1,2,3 -> done 5,8,9
    auto prio = schedule(jobs, Algorithm::PRIORITY);
    assert((completions(prio) == std::vector<int>{5, 8, 9}));

    // Idle time: first arrival at t=2
    auto idle = schedule({{7, 2, 1, 0}}, Algorithm::FCFS);
    assert(idle.idleTime == 2 && idle.makespan == 3 && idle.processes[0].waiting == 0);
    assert((idle.timeline == std::vector<int>{-1, -1, 7}));

    // Invalid input must throw, not crash.
    bool threw = false;
    try { schedule({}, Algorithm::FCFS); } catch (const std::invalid_argument&) { threw = true; }
    assert(threw);
    threw = false;
    try { schedule(jobs, Algorithm::ROUND_ROBIN, 0); } catch (const std::invalid_argument&) { threw = true; }
    assert(threw);

    std::cout << "All scheduler tests passed\n";
    return 0;
}
