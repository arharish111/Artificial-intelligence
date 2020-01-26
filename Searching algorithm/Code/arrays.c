#include <stdio.h>
#include <string.h>
#include <stdlib.h>

char **createArrayOfStrings(int lengthOfArray)
{
	int i;
	char **array = (char**)malloc(sizeof(char*)*lengthOfArray);
	if(array)
	{
		for(i=0;i<lengthOfArray;i++)
		{
			array[i] = (char*)malloc(sizeof(char*));
			array[i] = "empty";
		}
	}
	return array;
}

void freeArrayOfStrings(char **array,int lengthOfArray)
{
	int i;
	for(i=0;i<lengthOfArray;i++)
	{
		free(array[i]);
	}
	free(array);
}

int getLengthOfArrayOfStrings(char **array)
{
	int length = 0,i=0;
	while(array[i++]!=NULL)
		length = length + 1;
	return length;
}

int getPositionOfStringInAnArray(char **array,char *string)
{
	int length = getLengthOfArrayOfStrings(array);
	int i,position=-1;
	for(i=0;i<length;i++)
	{
		if(!strcmp(array[i],string))
		{
			position = i;
			break;
		}
	}
	return position;
}

int **create2dArray(int num1, int num2)
{
	int **result = (int **)malloc(sizeof(int*)* num1);
	int i;
	if(result)
	{
		for(i=0;i<num1;i++)
			result[i] = (int*)malloc(sizeof(int)* num2);	
	}
	return result;
}

int *createEmptyArray(int num)
{
	int *result = (int*)malloc(sizeof(int)*num);
	int i;
	if(result)
	{
	    for(i=0;i<num;i++)
		{
			result[i] = 0;
		}
	}
	return result;
}
