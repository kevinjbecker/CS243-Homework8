///
/// File: pt-cruisers.c
///
/// Description: Builds an instance of racers and sets them off.
///
/// @author: kjb2503 : Kevin Becker
///
// // // // // // // // // // // // // // // // // // // // // // // // // // //

#define _POSIX_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include "display.h"
#include "racer.h"

#define GENERIC_USAGE "Usage: %s [delay] racer1 racer2 [racer3...]\n"
#define NAME_TOO_LONG "Usage: racer names must not exceed length 9.\n"
#define INVALID_DELAY "Delay %li is invalid.\n"

///
/// Function: padName
///
/// Description: Pads the name so that it is centerd and surrounded by
///              underscores.
///
/// @param curName  The name as it was entered by the user.
/// @param paddedName  The location to store the padded name after it is padded.
///
static void padName(char *curName, char *paddedName)
{
    // the length remaining to make
    size_t nameLen = strlen(curName);

    // if the name is max already, we can copy and exit (waste of time otherwise)
    if(nameLen == MAX_NAME_LEN)
    {
        strcpy(paddedName, curName);
        return;
    }

    // the name we will return (MAX_NAME_LEN + 1 for the terminating NUL)
    char retName [MAX_NAME_LEN + 1];
    retName[MAX_NAME_LEN] = '\0';

    // the number of underscores before and after
    size_t before = (MAX_NAME_LEN - nameLen) / 2;

    // sets the first underscores in place
    for(size_t i = 0; i < before; ++i)
        retName[i] = '_';

    // copies in the name
    strcpy(retName+((MAX_NAME_LEN - nameLen)/2), curName);

    // sets the last underscores in place
    for(size_t i = before + nameLen; i < MAX_NAME_LEN; ++i)
        retName[i] = '_';

    // finally we put our padded name into place
    strcpy(paddedName, retName);
}

///
/// Function: main
///
/// Description: Runs an instance of pt-cruisers based off of the way
///              the program was run
///
/// @param argc  The number of arguments used to run the program.
/// @param **argv  Each argument as a string.
///
int main(int argc, char **argv)
{
    /* there should at least be 3 arguments (program, racer1, racer2)
       if argc is less than 3 we *definitely* have a problem */
    if(argc < 3)
    {
        // print usage message to
        fprintf(stderr, GENERIC_USAGE, argv[0]);
        return EXIT_FAILURE;
    }

    // the numer of millis a racer will wait
    long waitTime = DEFAULT_WAIT;
    // the argv index where the racer names begin
    int racerStartIndex = 1;

    // if the first character in argv[1] is numeric we get the wait
    if(argv[1][0] >= '0' && argv[1][0] <= '9')
    {
        waitTime = strtol(argv[1], NULL, 10);
        if(waitTime < 1)
        {
            fprintf(stderr, INVALID_DELAY, waitTime);
            fprintf(stderr, GENERIC_USAGE, argv[0]);
            return EXIT_FAILURE;
        }
        // the racers now start at argv[2], update that offset here
        ++racerStartIndex;
    }

    // last minute check to make sure we have enough racers
    if(argc-racerStartIndex < 2)
    {
       fprintf(stderr, GENERIC_USAGE, argv[0]);
        return EXIT_FAILURE;
    }

    // last minute check to make sure all names are small enough 9 characters
    for(int i = racerStartIndex; i < argc; ++i)
        if(strlen(argv[i]) > MAX_NAME_LEN)
        {
            fputs(NAME_TOO_LONG, stderr);
            return EXIT_FAILURE;
        }

    // BEGIN BUILDING OUR RUN ==================================================
    // creates an array to house our threads in
    pthread_t racerThreads[argc-racerStartIndex];
    // creates an array to house our racer pointers
    Racer *racers[argc-racerStartIndex];

    // seeds the random generator
    srand(10);
    // initializes the racer wait
    initRacers(waitTime);

    // loop to create all the racers
    for(int i = racerStartIndex; i < argc; ++i)
    {
        char paddedName[MAX_NAME_LEN + 1];
        padName(argv[i], paddedName);
        racers[i-racerStartIndex] = makeRacer(paddedName, i-racerStartIndex+1);
    }
    // clears the screen right before we go
    clear();

    // create a thread for each racer
    for(int i = 0; i < argc-racerStartIndex; ++i)
        pthread_create(&racerThreads[i], NULL, run, (void *)racers[i]);

    // joins all of the threads (we don't want to exit until they're done)
    for(int i = 0; i < argc-racerStartIndex; ++i)
    {
        // we don't care about the retval, we can set that to NULL
        pthread_join(racerThreads[i], NULL);
        // destroys the racer (FUCKING SEGFAULT?????)
        destroyRacer(racers[i]);
    }

    // sets our cursor to the bottom row
    set_cur_pos(argc-racerStartIndex+1, 0);
    // attempt to read in the first argument as number
    return EXIT_SUCCESS;
}
