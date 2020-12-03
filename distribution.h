typedef struct gaussian {
    int lowerbound;
    int upperbound;
    double varians;
    double expectedValue;
} gaussian;

int bernoulli(double chanceForTrue);
int rndInt(int max);
int uniformTruncated(int lowerbound, int upperbound);
int gaussianTruncatedDiscrete(gaussian normal);