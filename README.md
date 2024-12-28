# Synchronization Sightseeing

This project, completed as part of Sabancı University's Operating Systems (CS 307) course, implements a multi-threaded simulation of managing a tourist attraction using semaphores. Visitors and guides are represented as threads, and their synchronization is achieved using semaphore primitives.

## Introduction
This project simulates a tourist attraction where visitors can tour the site under strict synchronization constraints. The simulation ensures that:
- Tours start only when the required number of visitors arrive.
- No visitors leave during a tour.
- If a guide is present, they announce the end of the tour.
- The last visitor to leave notifies waiting visitors for the next round.

The synchronization is implemented using semaphores and barriers to prevent busy-waiting, ensuring efficient multi-threading.

## Problem Description
The task involves implementing the **Tour** class in `C++` to manage the synchronization of threads representing visitors and guides. The project adheres to the following rules:
- Tours require a fixed number of participants, and no more visitors are allowed until the ongoing tour ends.
- A guide is optional and is included as part of the visitor count.
- When a tour ends, threads execute specific behaviors depending on their roles.

## Tour Class
### Constructor
The `Tour` constructor accepts two parameters:
- `visitors`: The number of visitors required for a tour.
- `guide`: A flag indicating if a guide is required (0 or 1).

### Methods
- `arrive()`: Simulates a visitor arriving at the site.
- `start()`: Represents the visitor spending time at the attraction (simulated by provided test cases).
- `leave()`: Simulates a visitor or guide leaving the site after the tour ends.

## Implementation
Key components of the implementation:
- **Semaphores and Barriers**: Used for thread synchronization.
- **Mutexes**: Ensure atomic operations on shared variables.
- **Thread-safe Output**: Ensures console messages are not garbled when printed by multiple threads.

## How to Compile and Run
1. Clone the repository:
   ```bash
   git clone https://github.com/username/project-name.git
   cd project-name
   ```
2. Compile the code using the provided `Makefile`:
   ```bash
   make
   ```
3. Run the program with sample test cases:
   ```bash
   ./tour_test <test parameters>
   ```

## Sample Outputs
Example outputs can be found in the `outputs/` directory, showcasing various configurations of visitors and guides.

## Resources
- [POSIX Threads Documentation](https://man7.org/linux/man-pages/man7/pthreads.7.html)
- [Little Book of Semaphores](https://greenteapress.com/semaphores/)
