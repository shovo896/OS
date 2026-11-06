# CPU Scheduling Algorithm — Step-by-Step Design

A complete, runnable design + implementation of classic single-CPU scheduling
algorithms. Files: `scheduler.h` (model/API), `scheduler.cpp` (simulation),
`main.cpp` (worked-example demo), `test_scheduler.cpp` (assert-based tests).

## Step 1 — Define the process model

Each job is `{id, arrival, burst, priority}`:

| Field | Meaning | Constraint |
|---|---|---|
| `id` | unique identifier | no duplicates |
| `arrival` | time it enters the ready queue | `>= 0` |
| `burst` | total CPU time it needs | `> 0` |
| `priority` | lower number = higher priority (1 beats 3) | only used by Priority |

Invalid input (empty list, `burst <= 0`, `arrival < 0`, duplicate id,
Round-Robin `quantum <= 0`) is rejected with `std::invalid_argument`.

## Step 2 — Define what "good" means (metrics)

For every process, once scheduled:

```
turnaround = completion - arrival          // total time in system
waiting    = turnaround - burst            // time spent NOT running
response   = firstRun   - arrival          // time to first CPU
```

The scheduler also reports the averages of the three, total `idleTime`
(CPU with nothing ready), and `makespan` (when the last process finishes).
A `timeline[t]` trace (process id at tick `t`, `-1` = idle) makes every
decision auditable — this is what the tests check.

## Step 3 — Choose the data structures

- **Ready queue**: processes that have arrived but not finished.
- **Arrival order**: input sorted once by `(arrival, id)` so "who arrived"
  is never re-sorted inside the loop.
- **Remaining time** per process (needed by preemptive algorithms).
- Selection is a linear scan for the best ready process — simple and
  obviously correct; a heap (see `../lab_4.cpp`) is the O(log n) upgrade.

## Step 4 — Define the dispatch rule (the algorithm)

At each dispatch, pick among *ready* processes by:

| Algorithm | Rule | Preemptive? |
|---|---|---|
| FCFS | earliest `arrival`, tie → lowest `id` | No |
| SJF | shortest `burst`, tie → arrival, id | No |
| SRTF | shortest *remaining* time, re-evaluated every tick | Yes |
| Round Robin | head of FIFO queue, max `quantum` ticks, then requeue | Yes |
| Priority | lowest `priority` number, tie → arrival, id | No |

Tie-breaking is fixed (arrival, then id) so results are deterministic.

## Step 5 — The simulation loop

```
time = 0;  add all processes with arrival <= time to ready queue
while unfinished processes remain:
    if ready queue is empty:
        record idle ticks; jump time to the next arrival   // CPU idle
    else:
        pick one process by the Step-4 rule
        non-preemptive: run it for its whole burst
        preemptive:     run it for 1 tick (SRTF) or up to `quantum` ticks (RR),
                        then re-evaluate the rule
        record firstRun / completion; advance time
compute metrics (Step 2) from the recorded times
```

Round-Robin detail: processes arriving during a quantum join the queue
*before* the preempted process is requeued at the back.

## Step 6 — Complexity

With `n` processes and total burst `B`: selection scan is O(n) per dispatch,
so O(n²) worst case for non-preemptive and O(B·n) for tick-by-tick
preemptive simulation. Memory O(n + B) for the timeline. Fine for teaching
and lab sizes; swap the scan for a priority queue for production scale.

## Step 7 — Worked example (trace it by hand, then run it)

```
P1: arrival 0, burst 5, priority 3
P2: arrival 1, burst 3, priority 1
P3: arrival 2, burst 1, priority 2
```

| Algorithm | Order | Completions (P1,P2,P3) | Avg waiting |
|---|---|---|---|
| FCFS | P1 P2 P3 | 5, 8, 9 | 3.33 |
| SJF | P1 P3 P2 | 5, 9, 6 | 2.67 |
| SRTF | P1 P2 P3 P2 P1 | 8, 5, 3 | 1.33 |
| Round Robin (q=2) | P1 P2 P3 P1 P2 P1 | 9, 8, 5 | 3.33 |
| Priority | P1 P2 P3 | 5, 8, 9 | 3.33 |

SRTF trace: t0 P1 (only job) → t1 P2 (3 left < P1's 4) → t2 P3 (1 left)
→ t3–4 P2 → t5–7 P1. SJF cannot preempt P1, so P3 waits until t5.

## Step 8 — Implementation map

- `scheduler.h` — `Process`, `ScheduledProcess`, `ScheduleResult`,
  `Algorithm`, and `schedule(input, algo, quantum)`.
- `scheduler.cpp` — validation, one shared non-preemptive engine
  (FCFS/SJF/Priority differ only in the comparison), tick loop for
  SRTF/Round Robin, metric finalisation.
- `main.cpp` — prints the Step-7 example for all five algorithms.

## Step 9 — Build, run, test

```sh
cd scheduling_algorithm
c++ -std=c++17 -Wall -Wextra -O2 -o scheduler_demo main.cpp scheduler.cpp && ./scheduler_demo
c++ -std=c++17 -Wall -Wextra -O2 -o test_scheduler test_scheduler.cpp scheduler.cpp && ./test_scheduler
```

Tests cover: exact completions/averages for all five algorithms on the
Step-7 example, idle-time handling (first arrival at t=2), and rejection
of invalid input. Expected output of the test binary: `All scheduler tests passed`.

## Step 10 — Trade-offs (choosing in practice)

- **FCFS**: fair, simple; suffers convoy effect (short jobs stuck behind long ones).
- **SJF/SRTF**: optimal average waiting, but can starve long jobs and need
  burst estimates.
- **Round Robin**: responsive and fair; quantum too small → context-switch
  overhead, too large → degenerates to FCFS.
- **Priority**: matches business importance; needs aging to avoid starvation.
