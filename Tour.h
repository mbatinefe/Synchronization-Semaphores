#ifndef TOUR_H
#define TOUR_H

#include <semaphore.h>
#include <pthread.h>
#include <stdexcept>
#include <unistd.h>

#include <cstdio>
bool using_time_allowed = true;

using namespace std;

class Tour {
public:
    Tour(int visitors, int guide);
    ~Tour();
    void arrive();
    void start(); // PASSING SOME TIME IS SIMULATED BY PROF
    void leave();

private:
    int visitor_count;
    int group_guide_count;
    pthread_t guide_ID;
    bool is_tour_active;
    bool is_there_guide;
    
    sem_t tour_start_semaphore; // To control rounds
    sem_t tour_guide_semaphore; // To control guide finish last
    sem_t barrier1; // To keep visitors on arrive
    pthread_mutex_t mutex_visit; // Mutex is mutex
};


Tour::Tour(int group_size, int guide) {
    // Check if group_size is lower than 0 or guide has different params
    if (group_size <= 0 || (guide != 0 && guide != 1)) {
        throw invalid_argument("An error occurred.");
    }

    visitor_count = 0;
    group_guide_count = group_size + guide; // It is both excluding and including
        // I used last visitor as guided as well. So, if guide is 0 -> no problem even if its not included.
    is_tour_active = false;
    is_there_guide = guide;
    guide_ID = -1;

    pthread_mutex_init(&mutex_visit, nullptr);
    sem_init(&barrier1, 0, 0);
    sem_init(&tour_start_semaphore, 0, 1); // To give access to first comer
    sem_init(&tour_guide_semaphore, 0, 0);
}

Tour::~Tour() {
    sem_destroy(&tour_start_semaphore);
    sem_destroy(&tour_guide_semaphore);
    sem_destroy(&barrier1);
    pthread_mutex_destroy(&mutex_visit);
}

void Tour::arrive() {

    printf("Thread ID: %lu | Status: Arrived at the location.\n", pthread_self());
    sem_wait(&tour_start_semaphore);
    // I used lecture notes here to come up with solution
    // First visitor passes, rest waits here until visitor gets inside.
    pthread_mutex_lock(&mutex_visit);

    visitor_count++;

    if(visitor_count == group_guide_count){
        is_tour_active = true; // Activate the tour
        guide_ID = pthread_self(); // Assign last visitor as guide, no matter if they have guide=1 or 0
        printf("Thread ID: %lu | Status: There are enough visitors, the tour is starting.\n", pthread_self());
        
        visitor_count--; // Decrease because it is not visitor anymore
    } else {
        printf("Thread ID: %lu | Status: Only %d visitors inside, starting solo shots.\n", pthread_self(), visitor_count);
        if(!is_tour_active){
            // Visitor got inside, it can release the next one
            sem_post(&tour_start_semaphore);
        }
    }

    pthread_mutex_unlock(&mutex_visit);

    // Option 1: Activate using_time_allowed
    //      --- It gets the time and waits 6s 
    //      ---     if no other visitor coming at that time, it releases that visitor also its friend if any (they remove each other)
    //      ---     if yes, it blocks on barrier1, and wait for guide to release them
    // Option 2: Deactivate using_time_allowed
    //      --- It waits on barrier until guide releases them
    if(!pthread_equal(pthread_self(), guide_ID)) {
        
        if(using_time_allowed){
            // Option 1
            struct timespec ts;
            clock_gettime(CLOCK_REALTIME, &ts);
            ts.tv_sec += 7; // Set the clock
            int ret = sem_timedwait(&barrier1, &ts);
            // Please see report, I explained further
            if (ret == -1 && errno == ETIMEDOUT) { // This error is timed out, which is what we wanted
                pthread_mutex_lock(&mutex_visit);
                if(!is_tour_active) {
                    // Unlock it and release the visitor
                    pthread_mutex_unlock(&mutex_visit);
                    return;
                }
                pthread_mutex_unlock(&mutex_visit);
                // Rest as option 2
                sem_wait(&barrier1);
            }
            sem_post(&barrier1);
        } else {
            // Option 2
            sem_post(&barrier1);
            sem_post(&barrier1);
        }

    }
}

void Tour::leave() {

    // If tour is not active, and trying to leave
    if (!is_tour_active) {
        printf("Thread ID: %lu | Status: My camera ran out of memory while waiting, I am leaving.\n", pthread_self());
        return;
    } 

    pthread_mutex_lock(&mutex_visit);
    
    if (pthread_equal(pthread_self(), guide_ID)) {
        ///////---------------G U I D E _ P A T H -----------//////////
        if(is_there_guide){ // If there is a guide meaning guide=1 in constructor
            printf("Thread ID: %lu | Status: Tour guide speaking, the tour is over.\n", pthread_self());
        } 
        sem_post(&barrier1); // We release the barrier now, so that they can leave

        // We wait here until all of visitor leaves
        pthread_mutex_unlock(&mutex_visit);
        sem_wait(&tour_guide_semaphore);
        pthread_mutex_lock(&mutex_visit);

        if(!is_there_guide){ // If there is not, remember we also assigned last visitor as guide (in case guide=0 in constructor)
            printf("Thread ID: %lu | Status: I am a visitor and I am leaving.\n", pthread_self());
        }

        // All of them left
        printf("Thread ID: %lu | Status: All visitors have left, the new visitors can come.\n", pthread_self());
        is_tour_active = false; // Tour is not active anymore
        visitor_count = 0; // Reset visitor count
        guide_ID = -1; // Reset guide id
        sem_destroy(&barrier1); // Destroy the semaphore for other rounds
        sem_init(&barrier1, 0, 0); // Set again

        // Lets other round start if any...
        sem_post(&tour_start_semaphore);
    } else {
        ///////---------------V I S I T O R _ P A T H -----------//////////
        printf("Thread ID: %lu | Status: I am a visitor and I am leaving.\n", pthread_self());
        if(visitor_count == 1){
            // Last visitor left, let guide know that we are done
            sem_post(&tour_guide_semaphore);
        }
        visitor_count--; // Decrease since they left
    }
    pthread_mutex_unlock(&mutex_visit);
}


#endif


