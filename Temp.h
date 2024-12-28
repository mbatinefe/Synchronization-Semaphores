#ifndef TOUR_H
#define TOUR_H

#include <semaphore.h>
#include <pthread.h>
#include <stdexcept>
#include <unistd.h>
#include <cstdio>

using namespace std;

class Tour {
public:
    Tour(int visitors, int guide);
    ~Tour();
    void arrive();
    void start(); // PASSING SOME TIME IS SIMULATED BY PROF
    void leave();

private:
    struct my_sem_t {
        int val;                // Tracks semaphore value
        int waiting_count;      // Number of waiting threads
        bool terminate;         // Termination flag
        pthread_mutex_t lock;
        pthread_cond_t cond;

        my_sem_t() : val(1), waiting_count(0), terminate(false) {
            pthread_mutex_init(&lock, NULL);
            pthread_cond_init(&cond, NULL);
        }

        ~my_sem_t() {
            pthread_mutex_destroy(&lock);
            pthread_cond_destroy(&cond);
        }

        void wait() {
            pthread_mutex_lock(&lock);
            while (val <= 0 && !terminate) {
                waiting_count++;
                pthread_cond_wait(&cond, &lock);
                waiting_count--;
            }
            if (!terminate) {
                val--;
            }
            pthread_mutex_unlock(&lock);
        }

        void post() {
            pthread_mutex_lock(&lock);
            if (waiting_count > 0) {
                pthread_cond_signal(&cond);
            } else {
                val++;
            }
            pthread_mutex_unlock(&lock);
        }

        void broadcast_terminate() {
            pthread_mutex_lock(&lock);
            terminate = true;
            pthread_cond_broadcast(&cond);
            pthread_mutex_unlock(&lock);
        }

        int get_waiting() {
            pthread_mutex_lock(&lock);
            int result = waiting_count;
            pthread_mutex_unlock(&lock);
            return result;
        }
    };

    // Class members
    int visitor_count;
    int group_guide_count;
    pthread_t guide_ID;
    bool is_tour_active;
    bool is_there_guide;

    my_sem_t tour_start_semaphore;  // Custom semaphore with termination
    sem_t tour_guide_semaphore;
    sem_t barrier1;
    pthread_mutex_t mutex_visit;
    pthread_mutex_t mutex_end;
};

Tour::Tour(int group_size, int guide) {
    if (group_size <= 0 || (guide != 0 && guide != 1)) {
        throw invalid_argument("Invalid group size or guide flag.");
    }

    visitor_count = 0;
    group_guide_count = group_size + guide;
    is_tour_active = false;
    is_there_guide = guide;
    guide_ID = -1;

    pthread_mutex_init(&mutex_visit, nullptr);
    pthread_mutex_init(&mutex_end, nullptr);
    sem_init(&barrier1, 0, 0);
    sem_init(&tour_guide_semaphore, 0, 0);
}

void Tour::arrive() {
    tour_start_semaphore.wait();

    if (tour_start_semaphore.terminate) {
        printf("Thread ID: %lu | Status: Terminated. Leaving without a tour.\n", pthread_self());
        return;
    }

    printf("Thread ID: %lu | Status: Arrived at the location.\n", pthread_self());

    pthread_mutex_lock(&mutex_visit);
    visitor_count++;
    pthread_mutex_unlock(&mutex_visit);

    if (visitor_count == group_guide_count) {
        is_tour_active = true;
        guide_ID = pthread_self();
        printf("Thread ID: %lu | Status: There are enough visitors, the tour is starting.\n", pthread_self());
    } else {
        printf("Thread ID: %lu | Status: Only %d visitors inside, starting solo shots.\n", pthread_self(), visitor_count);
        tour_start_semaphore.post();
    }
}

void Tour::leave() {
    pthread_mutex_lock(&mutex_end);

    if (!is_tour_active) {
        printf("Thread ID: %lu | Status: My camera ran out of memory while waiting, I am leaving.\n", pthread_self());
        pthread_mutex_unlock(&mutex_end);
        return;
    }

    if (pthread_equal(pthread_self(), guide_ID) && is_there_guide) {
        printf("Thread ID: %lu | Status: Tour guide speaking, the tour is over.\n", pthread_self());
    } else {
        printf("Thread ID: %lu | Status: I am a visitor and I am leaving.\n", pthread_self());
    }

    visitor_count--;

    if (visitor_count == 0) {
        is_tour_active = false;
        printf("Thread ID: %lu | Status: All visitors have left, the new visitors can come.\n", pthread_self());
        tour_start_semaphore.post(); // Release the semaphore for new arrivals
    }

    pthread_mutex_unlock(&mutex_end);
}

#endif // TOUR_H
