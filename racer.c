#include <pthread.h>

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
