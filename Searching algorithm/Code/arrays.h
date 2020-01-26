#ifndef arrays_h
#define arrays_h

char **createArrayOfStrings(int lengthOfArray);
void freeArrayOfStrings(char **array,int lengthOfArray);
int getLengthOfArrayOfStrings(char **array);
int **create2dArray(int num1, int num2);
int getPositionOfStringInAnArray(char **array,char *string);
int *createEmptyArray(int num);

#endif 

