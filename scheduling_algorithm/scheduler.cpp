#include "scheduler.h"
#include <algorithm>
#include <queue>
#include <stdexcept>
#include <unordered_set>

std::string toString(Algorithm algo) {
    switch (algo) {
        case Algorithm::FCFS: return "FCFS";
        case Algorithm::SJF: return "SJF";
        case Algorithm::SRTF: return "SRTF";
        case Algorithm::ROUND_ROBIN: return "Round Robin";
        case Algorithm::PRIORITY: return "Priority";
    }
    return "Unknown";
}

namespace {

void validate(const std::vector<Process>& input, Algorithm algo, int quantum) {
    if (input.empty()) throw std::invalid_argument("no processes");
    if (algo == Algorithm::ROUND_ROBIN && quantum <= 0)
        throw std::invalid_argument("quantum must be > 0");
    std::unordered_set<int> ids;
    for (const auto& p : input) {
        if (p.arrival < 0) throw std::invalid_argument("arrival must be >= 0");
        if (p.burst <= 0) throw std::invalid_argument("burst must be > 0");
        if (!ids.insert(p.id).second) throw std::invalid_argument("duplicate process id");
    }
}

void finalize(ScheduleResult& r) {
    double tw = 0, tt = 0, tr = 0;
    for (auto& sp : r.processes) {
        sp.turnaround = sp.completion - sp.p.arrival;
        sp.waiting = sp.turnaround - sp.p.burst;
        sp.response = sp.firstRun - sp.p.arrival;
        tw += sp.waiting; tt += sp.turnaround; tr += sp.response;
        r.makespan = std::max(r.makespan, sp.completion);
    }
    double n = static_cast<double>(r.processes.size());
    r.avgWaiting = tw / n; r.avgTurnaround = tt / n; r.avgResponse = tr / n;
    r.idleTime = static_cast<int>(std::count(r.timeline.begin(), r.timeline.end(), -1));
}

// Indices sorted by (arrival, id) - the order processes enter the ready queue.
std::vector<int> byArrival(const std::vector<Process>& in) {
    std::vector<int> idx(in.size());
    for (size_t i = 0; i < in.size(); ++i) idx[i] = static_cast<int>(i);
    std::sort(idx.begin(), idx.end(), [&](int a, int b) {
        if (in[a].arrival != in[b].arrival) return in[a].arrival < in[b].arrival;
        return in[a].id < in[b].id;
    });
    return idx;
}

void runTicks(ScheduleResult& r, int idx, int start, int ticks,
              std::vector<int>& remaining) {
    auto& sp = r.processes[idx];
    if (sp.firstRun == -1) sp.firstRun = start;
    for (int t = 0; t < ticks; ++t) {
        r.timeline.push_back(sp.p.id);
        if (--remaining[idx] == 0) sp.completion = start + t + 1;
    }
}

void idleUntil(ScheduleResult& r, int from, int to) {
    for (int t = from; t < to; ++t) r.timeline.push_back(-1);
}

// Non-preemptive dispatch: pick best ready index by `better(a,b)`, run to completion.
template <typename Better>
ScheduleResult runNonPreemptive(const std::vector<Process>& in, Better better) {
    ScheduleResult r;
    r.processes.resize(in.size());
    for (size_t i = 0; i < in.size(); ++i) r.processes[i].p = in[i];
    std::vector<int> remaining(in.size());
    for (size_t i = 0; i < in.size(); ++i) remaining[i] = in[i].burst;
    auto order = byArrival(in);
    size_t next = 0;
    int time = 0, done = 0;
    std::vector<int> ready;
    while (done < static_cast<int>(in.size())) {
        while (next < order.size() && in[order[next]].arrival <= time) ready.push_back(order[next++]);
        if (ready.empty()) { // idle: jump to next arrival
            int nxt = in[order[next]].arrival;
            idleUntil(r, time, nxt); time = nxt; continue;
        }
        auto it = std::min_element(ready.begin(), ready.end(),
            [&](int a, int b) { return better(in[a], in[b]); });
        int cur = *it; ready.erase(it);
        runTicks(r, cur, time, remaining[cur], remaining);
        time += in[cur].burst; ++done;
    }
    finalize(r);
    return r;
}

} // namespace

ScheduleResult schedule(const std::vector<Process>& input, Algorithm algo, int quantum) {
    validate(input, algo, quantum);

    // Step 4 - dispatch rule per algorithm.
    if (algo == Algorithm::FCFS) // earliest arrival, tie: lowest id
        return runNonPreemptive(input, [](const Process& a, const Process& b) {
            if (a.arrival != b.arrival) return a.arrival < b.arrival;
            return a.id < b.id;
        });
    if (algo == Algorithm::SJF) // shortest burst, tie: arrival, then id
        return runNonPreemptive(input, [](const Process& a, const Process& b) {
            if (a.burst != b.burst) return a.burst < b.burst;
            if (a.arrival != b.arrival) return a.arrival < b.arrival;
            return a.id < b.id;
        });
    if (algo == Algorithm::PRIORITY) // lowest priority number, tie: arrival, then id
        return runNonPreemptive(input, [](const Process& a, const Process& b) {
            if (a.priority != b.priority) return a.priority < b.priority;
            if (a.arrival != b.arrival) return a.arrival < b.arrival;
            return a.id < b.id;
        });

    // Preemptive algorithms: simulate one tick at a time (Step 5).
    ScheduleResult r;
    r.processes.resize(input.size());
    for (size_t i = 0; i < input.size(); ++i) r.processes[i].p = input[i];
    std::vector<int> remaining(input.size());
    for (size_t i = 0; i < input.size(); ++i) remaining[i] = input[i].burst;
    auto order = byArrival(input);
    size_t next = 0;
    int time = 0, done = 0;

    if (algo == Algorithm::SRTF) {
        std::vector<int> ready;
        while (done < static_cast<int>(input.size())) {
            while (next < order.size() && input[order[next]].arrival <= time) ready.push_back(order[next++]);
            if (ready.empty()) { int nxt = input[order[next]].arrival; idleUntil(r, time, nxt); time = nxt; continue; }
            int cur = *std::min_element(ready.begin(), ready.end(), [&](int a, int b) {
                if (remaining[a] != remaining[b]) return remaining[a] < remaining[b];
                if (input[a].arrival != input[b].arrival) return input[a].arrival < input[b].arrival;
                return input[a].id < input[b].id;
            });
            runTicks(r, cur, time, 1, remaining);
            if (remaining[cur] == 0) { ready.erase(std::remove(ready.begin(), ready.end(), cur), ready.end()); ++done; }
            ++time;
        }
    } else { // ROUND_ROBIN
        std::queue<int> q;
        int current = -1, sliceLeft = 0;
        while (done < static_cast<int>(input.size())) {
            // Arrivals at `time` join the queue before a preempted process is requeued.
            while (next < order.size() && input[order[next]].arrival <= time) q.push(order[next++]);
            if (current != -1 && sliceLeft == 0) { q.push(current); current = -1; }
            if (q.empty()) {
                if (current == -1) { int nxt = input[order[next]].arrival; idleUntil(r, time, nxt); time = nxt; continue; }
            } else if (current == -1) { current = q.front(); q.pop(); sliceLeft = quantum; }
            runTicks(r, current, time, 1, remaining);
            if (remaining[current] == 0) { ++done; current = -1; }
            else --sliceLeft;
            ++time;
        }
    }
    finalize(r);
    return r;
}
