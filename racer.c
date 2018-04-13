#define _POSIX_SOURCE
#include <pthread.h>
#include <string.h>

#define MICRO_SEC_TO_MILLIS 1000

static long wait;

void initRacers( long milliseconds )
{
    // sets the wait to the milliseconds specified
    wait = milliseconds;
}

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
    // allocate space for the name
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

void destoryRacer(Racer *racer)
{
    // nothing we need to do if the racer is alread null
    if(racer == NULL)
        return;

    // frees the graphic (since it was allocated on the heap)
    free(racer->graphic);
    // set the graphic to NULL (for keeping things clean)
    racer->graphic = NULL;
    // free the racer itself; racer destroyed
    free(racer);
}

/// run Run one racer in the race.
/// Initialize the display of the racer*:
///   The racer starts at the start position, column 1.
///   The racer's graphic (text name ) is displayed.
/// This action happens repetitively, until its position is at FINISH_LINE:
///   Randomly calculate a waiting period, no more than
///   the value given to initRacers
///   Sleep for that length of time.
///   Change the display position of this racer by +1 column*:
///     Erase the racer's name from the display.
///     Update the racer's dist field by +1.
///     Display the racer's name at the new position.
///
void *run( void *racer )
{
    // casts our racer to something we know
    Racer * rcr = (Racer *)racer;
    
    // makes a mutex which is static to all threads
    static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

    // keeps going until we're at the finish line
    while(rcr->position < FINISH_LINE)
    {
        // locks the mutex so we can print ourself out
        pthread_mutex_lock(&mutex);
        // BEGIN CRITIAL REGION
        set_cur_pos(rcr->row, rcr->dist++);
        printf(" %s", rcr->display);
        fflush(stdout);
        // END CRITIAL REGION
        // unlocks the critical region for a racer to go in
        pthread_mutex_unlock(&mutex)
        // sleeps for a random amount of time (max wait)
        usleep(rand(wait) * MICRO_SEC_TO_MILLIS);
    }
}
