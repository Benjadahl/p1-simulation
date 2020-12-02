#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include "distribution.h"

double randNumberZeroToOne();
int bernoulli(double chanceForTrue);
int rndInt(int max);
double uniform(double lowerBound, double upperBound);
void expSim(double *arrayExpDistribution, int lenghtOfArray,
             double lambda);
double normal();
double gaussianSpecific(double varians, double expectedValue);
int gaussianTruncatedDiscrete(gaussian normal);

double randNumberZeroToOne()
{
    int n = rand();
    double numberOneToZero = (double) n / RAND_MAX;
    return numberOneToZero;
}

int bernoulli(double chanceForTrue)
{
    if (rndInt(100) <= chanceForTrue - 1) {
        return 1;
    } else {
        return 0;
    }
}

int rndInt(int max)
{
    return rand() % max;
}

double uniform(double lowerBound, double upperBound)
{
    double result;

    result =
        randNumberZeroToOne() * (upperBound - lowerBound) + lowerBound;

    return result;
}

int uniformTruncted(int lowerbound, int upperbound)
{
    double result;

    do
    {
        result = randNumberZeroToOne() * (upperbound - lowerbound) + lowerbound;
        result = round(result);
    }while(result < lowerbound || result > upperbound);

    return result;
}


void expSim(double *arrayExpDistribution, int lenghtOfArray, double lambda)
{
    double numberOneToZero;
    for (int i = 0; i < lenghtOfArray; i++) {
        do {
            numberOneToZero = randNumberZeroToOne();
        } while (numberOneToZero == 0);

        arrayExpDistribution[i] =
            -(1.0 / lambda) * log(1 - numberOneToZero);
    }
}

double normal()
{
    double U, V, *X = NULL, Y[1];
    
    V = uniform(0, 1);

    while(X == NULL)
    {
        expSim(Y, 1, 1 / 1);
        U = uniform(0, 1);

        if(U <= exp(-1 * (pow(Y[0] - 1, 2) / 2)))
        {
            if (Y[0] <= 0)
            {
                X = (Y + 0); 
            }
            else
            {
                X = (Y + 0);
                *X *= -1;
            }
        }
    }

    if(V <= 0.5) 
    {
        return *X;
    } 
    else 
    {
        return -1 * (*X);
    }
}

double gaussianSpecific(double varians, double expectedValue)
{
    double Y;

    Y = normal();

    return Y * sqrt(varians) + expectedValue;
}

int gaussianTruncatedDiscrete(gaussian normal)
{
    double result;

    do
    {
        result = gaussianSpecific(normal.varians, normal.expectedValue);
        result = round(result);
    } while(result < normal.lowerbound || result > normal.upperbound);
    
    return result;
}
