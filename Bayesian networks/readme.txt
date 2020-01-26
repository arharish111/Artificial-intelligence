Student Name:Harish Harish

Programming language: C

compile : gcc -o bnetwork bayesian.c -lm
run: bnetwork At Jf
-----------------------------------------------------------------------------
Code structure:

void initializeBNet(struct bayesianNetwork *bnet);
---To initialize bayesian network with known nodes and probability.


double findProbability(char **knownC,int *values,int count,struct bayesianNetwork *bnet);
----Recursive function to initialize values for hidden variables and call findProbabilityValue in base case.


double findProbabilityValue(char **knownC,int *values,struct bayesianNetwork *bnet);
----to get probability value of the input sequence by semantics of the bayesian network.


int getDecimalValue(int *numbers,int number);
----to convert binary into decimal value.


int *extend(int *values,int position,int value);
----Helper function to assign values for the hidden variables.


int argumentValidation(int number,char **arg);
----to verify whether arguments passed through command line are correct.
