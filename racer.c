#define _BSD_SOURCE
#include <pthread.h>
#include <string.h>
#include <unistd.h>
#include "display.h"
#include "racer.h"

// A converter from milliseconds to microseconds
#define MILLI_SEC_TO_MICRO_SEC 1000

/// global max wait time between each movement
static long waitTime;

/// sets the wait time each racer should make.
void initRacers( long milliseconds )
{
    // sets the wait to the milliseconds specified
    waitTime = milliseconds;
}

/// creates a heap allocated racer with name and position.
Racer *makeRacer(char *name, int position)
{
    // the racer we will eventually return
    Racer * racer = NULL;
    // allocates a new racer
    racer = malloc(sizeof(Racer));
    // if malloc hit an error, we report it and return null
    if(racer == NULL)
    {
        perror("Error allocating racer");
        return NULL;
    }
    // if we make it here, malloc was successful
    racer->dist = 1;
    racer->row = position;
    // allocate space for the name + NUL
    racer->graphic = malloc(MAX_NAME_LEN + 1);
    // make sure the allocation succeeded
    if(racer->graphic == NULL)
    {
        perror("Error allocating racer name");
        return NULL;
    }
    // copy in the name
    strcpy(racer->graphic, name);
    // we've finished creating the racer, we can return it now
    return racer;
}

/// destroys the heap usage of the racer.
void destroyRacer(Racer *racer)
{
    // nothing we need to do if the racer is alread null
    if(racer == NULL)
        return;
    // frees the graphic (since it was allocated on the heap)
    free(racer->graphic);
    racer->graphic = NULL;
    // free the racer itself; racer destroyed
    free(racer);
}

/// thread run function which is used to simulate the race.
void *run( void *racer )
{
    // casts our racer to something we know
    Racer * rcr = (Racer *)racer;

    // makes a mutex which is static to all threads
    static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

    // keeps going until we're at the finish line
    while(rcr->dist < FINISH_LINE)
    {
        // locks the mutex so we can print ourself out
        pthread_mutex_lock(&mutex);
        // BEGIN CRITIAL REGION
        set_cur_pos(rcr->row, rcr->dist++);
        /* the space before is to remove the trailing underscore (makes sure
           that the only printed characters are the 9 wanted */
        printf(" %s", rcr->graphic);
        fflush(stdout);
        // END CRITIAL REGION
        // unlocks the critical region for a racer to go in
        pthread_mutex_unlock(&mutex);
        // sleeps for a random amount of time (max wait)
        usleep((rand() % waitTime) * MILLI_SEC_TO_MICRO_SEC);
    }
    return NULL;
}
