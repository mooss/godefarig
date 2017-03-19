#include "combinaisons.h"

unsigned long factorial(unsigned int nbr)
{
    unsigned long result;
    for(result = 1; nbr > 1; --nbr)
        result *= nbr;
    return result;
}
