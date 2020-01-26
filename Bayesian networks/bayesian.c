#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<math.h>

struct bayesianNetwork
{
	char *variables;
	char **parents;
	double **probability;
	int *numberOfParents;
};

void initializeBNet(struct bayesianNetwork *bnet);
double findProbability(char **knownC,int *values,int count,struct bayesianNetwork *bnet);
double findProbabilityValue(char **knownC,int *values,struct bayesianNetwork *bnet);
int getDecimalValue(int *numbers,int number);
int *extend(int *values,int position,int value);
int argumentValidation(int number,char **arg);

int main(int numberOfArguments,char **arguments)
{
	int i,j;
	if(numberOfArguments<2 || numberOfArguments>6)
	{
		printf("Invalid Input");
		return 0;
	}
	if(!argumentValidation(numberOfArguments,arguments))
	{
		printf("Invalid Input");
		return 0;
	}
	char **knownC1 = (char**)malloc(5*5);
	char **knownC2 = (char**)malloc(5*5);
	int *values1 = (int*)malloc(sizeof(int)*5);
	int *values2 = (int*)malloc(sizeof(int)*5);
	if(knownC1 && knownC2)
	{
		for(i=0;i<5;i++)
		{
			knownC1[i] = (char*)malloc(5);
			knownC2[i] = (char*)malloc(5);
		}
	}
	int count1=0,count2=0;
	for(i=1;i<numberOfArguments;i++)
	{
		if(!strcmp(arguments[i],"given"))
		{
			for(i=i+1;i<numberOfArguments;i++)
			{
				strcpy(knownC2[count2],arguments[i]);
				strcpy(knownC1[count1],arguments[i]);
				if(arguments[i][1]=='t')
				{
					values2[count2] = 1;
					values1[count1] = 1;
				}
				else
				{
					values2[count2] = 0;
					values1[count1] = 0;
				}
				count2++;
				count1++;
			}
			break;
		}
		else
		{
			strcpy(knownC1[count1],arguments[i]);
			if(arguments[i][1]=='t')
			{
				values1[count1] = 1;
			}
			else
			{
				values1[count1] = 0;
			}
			count1++;
		}
	}
	
	int initialUnknownIndex1 = count1;
	int initialUnknownIndex2 = count2;
	struct bayesianNetwork bnet;
	initializeBNet(&bnet);
	double p2 = 1.0;
	if(count1<=4)
	{
		for(i=0;i<5;i++)
		{
			int foundCheck = 0;
			for(j=0;j<count1;j++)
			{
				if(bnet.variables[i]==knownC1[j][0])
				{
					foundCheck = 1;
					break;
				}
			}
			if(foundCheck==0)
			{
				knownC1[count1][0] = bnet.variables[i];
				knownC1[count1][1] = '\0';
				values1[count1] = -1;
				count1++;
			}
			foundCheck = 0;
			if(count2>0)
			{
				for(j=0;j<count2;j++)
				{
					if(bnet.variables[i]==knownC2[j][0])
					{
						foundCheck = 1;
						break;
					}
				}
				if(foundCheck==0)
				{
					knownC2[count2][0] = bnet.variables[i];
					knownC2[count2][1] = '\0';
					values2[count2] = -1;
					count2++;
					foundCheck = 0;
				}
			}
		}
	}
	
	double p1 = findProbability(knownC1,values1,initialUnknownIndex1,&bnet);
	double finalP;
	if(count2>0)
	{
		p2 = findProbability(knownC2,values2,initialUnknownIndex2,&bnet);
	}
	finalP = p1/p2;
	printf("Probability = %0.11f\n",finalP);
	return 0;
}

double findProbability(char **knownC,int *values,int count,struct bayesianNetwork *bnet)
{
	int c;
	if(count>4)
	{
		return findProbabilityValue(knownC,values,bnet);
	}
	else
	{	
		c = count;
		count = count + 1;
		return findProbability(knownC,extend(values,c,1),count,bnet) + 
			   findProbability(knownC,extend(values,c,0),count,bnet);
	}
}

double findProbabilityValue(char **knownC,int *values,struct bayesianNetwork *bnet)
{
	int i,p,q,j;
	double value = 1.0,truthValue;
	for(i=0;i<5;i++)
	{
		for(j=0;j<5;j++)
		{
			if(bnet->variables[j]==knownC[i][0])
			break;
		}
		if(bnet->numberOfParents[j]>0)
		{
			int number = bnet->numberOfParents[j],r,t;
			int* numbers = (int*)malloc(sizeof(int)*number);
			for(r=0;r<number;r++)
			{
				for(t=0;t<5;t++)
				{
					if(bnet->parents[j][r]==knownC[t][0])
					break;
				}
				if(values[t]==1)
					numbers[r] = 0;
				else
					numbers[r] = 1;
			}
		    q = getDecimalValue(numbers,number);
			p=j;
			truthValue = bnet->probability[p][q];
			if(values[i]==1)
			{
				value = value * truthValue;
			}
			else
				value = value * (1-truthValue);
		}
		else
		{
			truthValue = bnet->probability[j][0];
			if(values[i]==1)
			{
				value = value * truthValue;
			}
			else
				value = value * (1-truthValue);
		}
	}
	return value;
}

int getDecimalValue(int *numbers,int number)
{
	int num = number-1,i,value=0;
	for(i=0;i<number;i++)
	{
		if(numbers[i]==1)
		{
			value = value + pow(2,num);
			num--;
		}
		else
		{
			num--;
		}
	}
	return value;
}
int *extend(int *values,int position,int value)
{
	values[position] = value;
	return values;
}

void initializeBNet(struct bayesianNetwork *bnet)
{
	int i;
	bnet->variables = "BEAJM";
	bnet->parents = (char**)malloc(5*5);
	if(bnet->parents)
	{
		for(i=0;i<5;i++)
		{
			bnet->parents[i] = (char*)malloc(5);
		}
	}
	
	//strcpy(bnet->parents[0],"0");
	//strcpy(bnet->parents[1],"0");
	strcpy(bnet->parents[2],"BE");
	strcpy(bnet->parents[3],"A");
	strcpy(bnet->parents[4],"A");
	
	bnet->probability = (double**)malloc(sizeof(*bnet->probability)*5);
	if(bnet->probability)
	{
		for(i=0;i<5;i++)
		{
			bnet->probability[i] = (double*)malloc(sizeof(double)*4);
		}
	}
	
	bnet->probability[0][0] = 0.001;
	bnet->probability[1][0] = 0.002;
	bnet->probability[2][0] = 0.95;
	bnet->probability[2][1] = 0.94;
	bnet->probability[2][2] = 0.29;
	bnet->probability[2][3] = 0.001;
	bnet->probability[3][0] = 0.90;
	bnet->probability[3][1] = 0.05;
	bnet->probability[4][0] = 0.70;
	bnet->probability[4][1] = 0.01;
	
	bnet->numberOfParents = (int*)malloc(sizeof(int)*5);
	bnet->numberOfParents[0] = 0;
	bnet->numberOfParents[1] = 0;
	bnet->numberOfParents[2] = 2;
	bnet->numberOfParents[3] = 1;
	bnet->numberOfParents[4] = 1;
}

int argumentValidation(int number,char **arg)
{
	int i,j;
	for(i=1;i<number;i++)
	{
		if(!strcmp(arg[i],"given"))
			continue;
		for(j=i+1;j<number;j++)
		{
			if(!strcmp(arg[j],"given"))
				continue;
			if(arg[i][0]==arg[j][0])
				return 0;
			if(!strcmp(arg[i],arg[j]))
				return 0;
		}
	}
	return 1;
}