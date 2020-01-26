#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>
#include<math.h>

#include "arrays.h"
#include "find_route.h"

struct struct_graph
{
	int numberOfVertices;
	int **edges;
};

struct struct_node_pair
{
	int currentNode;
	int parentNode;
	int pathCost;
	int actualPathCost;
};

struct struct_closed_node_pair
{
	int currentNode;
	int parentNode;
};


graph createEmptyGraph(int num)
{
	graph result = (graph)malloc(sizeof(*result));
	result->numberOfVertices = num;
	result->edges = create2dArray(num, num);
	
	int i,j;
	for(i=0;i<num;i++)
	{
		for(j=0;j<num;j++)
		{
			result->edges[i][j] = 0;
		}
	}
	
	return result;
}

node **createEmptyNodePairs(int num)
{
	node **result1 = (node**)malloc((num) * sizeof(*result1));
	int i;
	if(result1)
	{
		for(i=0;i<=num;i++)
		{
		  result1[i] = (node*)malloc(sizeof(*result1));
		  result1[i]->currentNode = -1;
		  result1[i]->parentNode = -1;
		  result1[i]->pathCost = -1;
		  result1[i]->actualPathCost = -1;
		}
	}
	return result1;
}

closedNode **createEmptyClosedNodePairs(int num)
{
	closedNode **result = (closedNode**)malloc(sizeof(*result)*num);
	int i;
	if(result)
	{
		for(i=0;i<num;i++)
		{
		  result[i] = (closedNode*)malloc(sizeof(*result));
		  result[i]->currentNode = -1;
		  result[i]->parentNode = -1;
		}
	}
	return result;
}
graph createLinks(graph g,int v1,int v2,int pathCost)
{
	g->edges[v1][v2] = pathCost;
	g->edges[v2][v1] = pathCost;
	return g;
}

graph getInfoToCreateLinks(char *fileName,char **finalArray,graph g)
{
  FILE *fp = fopen(fileName,"r");
  if (fp == NULL)
  {
	printf("error opening the file");
  }
  else	
  {
  int v1,v2;
  
  char *city1 = (char*)malloc(sizeof(*city1));
  char *city2 = (char*)malloc(sizeof(*city2));

  int pathCost,i;
  int arrayLength = getLengthOfArrayOfStrings(finalArray);
  while(fscanf(fp,"%s%s%d\n",city1,city2,&pathCost) != EOF)
  {
	if(!strcmp("END",city1))
	break;
    
	for(i=0;i<arrayLength;i++)
	{
		if(!strcmp(finalArray[i],city1))
		{	
			v1=i;
			break;
		}
	}
	for(i=0;i<arrayLength;i++)
	{
		if(!strcmp(finalArray[i],city2))
		{
			v2=i;
			break;
		}
	}
	g = createLinks(g,v1,v2,pathCost);
  }
 }
    fclose(fp);	
	return g;
}
int getNumberOfLinesInTheFile(char *fileName)
{
  FILE *fp = fopen(fileName,"r");
  int result,i=0;
  char line[100];
  if (fp == NULL)
  {
	printf("error opening the file");
	return 0;
  }
  else
	while(fgets(line,100,fp) && !feof(fp))
	{
		i = i+1;
	}
	result = i - 1;
	fclose(fp);
	return result;
}

char **getArrayOfNamesInTheFile(char *fileName,int numberOfLinesInFile,int *len)
{
  FILE *fp = fopen(fileName,"r");
  if (fp == NULL)
  {
	printf("error opening the file");
  }
  else	
  {
  int arrayLength = numberOfLinesInFile*2,foundCheck=0;
  //printf("L:%d\n",arrayLength);
  
  char **arrayOfNames = createArrayOfStrings(arrayLength); 
  
  char *name1 = (char*)malloc(sizeof(*name1));
  char *name2 = (char*)malloc(sizeof(*name2));

  int pathCost,j=0,i;
  while(fscanf(fp,"%s%s%d\n",name1,name2,&pathCost) != EOF)
  {
	if(!strcmp("END",name1))
	break;
       //printf("A1:%s\n",arrayOfNames[0]);
      //  printf("S1:%s\n",name1);
	for(i=0;i<arrayLength;i++)
	{
		if(!strcmp(arrayOfNames[i],name1))
		{
          //              printf("S2:%s,i:%s\n",name1,arrayOfNames[i]);	
			foundCheck = 1;
			break;
		}
		foundCheck = 0;
	}
	if(!foundCheck)
	{
        // printf("j:%d\n",j);
	 arrayOfNames[j++]= strdup(name1);
        // printf("S3:%s,j:%s\n",name1,arrayOfNames[j]);
         foundCheck=0;
	}
	for(i=0;i<arrayLength;i++)
	{
		if(!strcmp(arrayOfNames[i],name2))
		{
          //              printf("S2:%s,i:%s\n",name1,arrayOfNames[i]);	
			foundCheck = 1;
			break;
		}
		foundCheck = 0;
	}
	if(!foundCheck)
	{
        // printf("j:%d\n",j);
	 arrayOfNames[j++]= strdup(name2);
        // printf("S3:%s,j:%s\n",name1,arrayOfNames[j]);
         foundCheck=0;
	}
	
   }
   
    fclose(fp);
    for(i=0;i<arrayLength;i++)
	{
	 // printf("%s\n",arrayOfNames[i]);
	  if(!strcmp("empty",arrayOfNames[i]))
      break;
	}
	int actualLength = i;
    char **finalArray = createArrayOfStrings(actualLength);
    for (i = 0; i < actualLength; i++)
	{
        finalArray[i] = arrayOfNames[i];
        //printf("i:%d-%s\n",i,finalArray[i]);
    }
    *len = actualLength;
    return finalArray;
   }	
}

void uniformCostSearch(node **NP,int start,graph g,int final,char **finalArr,int *hueristicDistance,int lengthOfGraph)
{
	// int z,y;
        // for(z=0;z<g->numberOfVertices;z++)
	// {
	  // for(y=0;y<g->numberOfVertices;y++)
	  // {
	     // if(g->edges[z][y] != 0)
             // printf("%d ",g->edges[z][y]);
	  // }
	  // printf("\n");
	// }
	NP[1]->currentNode = start;
	NP[1]->pathCost = 0;
	NP[1]->actualPathCost = NP[1]->pathCost + hueristicDistance[start];
	NP[0]->pathCost = 1;
	closedNode **closedNP = createEmptyClosedNodePairs(lengthOfGraph);
	int i=0,j,p,q=2,r=1,s,foundCheck=0,fc=1;
	node *key;
	int numberOfNodesExpanded = 0;
    while(NP[0]->pathCost!=0)
	{
		numberOfNodesExpanded = numberOfNodesExpanded + 1;
		//printf("start,final,curr,no:%d,%d,%d,%d\n",start,final,NP[r]->currentNode,numberOfNodesExpanded);
		for(p=0;p<lengthOfGraph;p++)
		{
			if(closedNP[p]->currentNode == NP[r]->currentNode)
			{ 
			   foundCheck = 1;
			   break;	
			}
		}
		if(foundCheck == 1)
		{
			r=r+1;
			NP[0]->pathCost = NP[0]->pathCost - 1;
			foundCheck=0;
			continue;
		}
		closedNP[i]->currentNode = NP[r]->currentNode;
		closedNP[i]->parentNode = NP[r]->parentNode;
		//printf("added to close\n");
		if(NP[r]->currentNode == final)
		{
			fc=0;
			printf("Number of nodes expanded:%d\n",numberOfNodesExpanded);
			printf("Distance:%d KM\n",NP[r]->actualPathCost);	
			getFinalPath(closedNP,final,lengthOfGraph,g,finalArr);
			break;
		}
		else
		{
			//printf("Not final:%d\n",i);
			for(j=0;j<lengthOfGraph;j++)
			{
				foundCheck = 0;
				if(g->edges[NP[r]->currentNode][j] != 0)
				{
					//printf("valueofj:%d\n",j);
					// for(p=0;p<lenghtofNP;p++)
					// {
						// if(closedNP[p]->currentNode == j)
						// { 
						   // foundCheck = 1;
						   // break;	
						// }
					// }
                    //printf("valueofjAdd:%dforQ:%d\n",j,q);		
					NP[q]->currentNode = j;
					NP[q]->parentNode = NP[r]->currentNode;
					NP[q]->pathCost = NP[r]->pathCost + g->edges[NP[r]->currentNode][j];
					NP[q]->actualPathCost = NP[q]->pathCost + hueristicDistance[j];
					q=q+1;
					NP[0]->pathCost = NP[0]->pathCost + 1;					
				}
			}
			for(j=r+2;j<q;j++)
			{
				key = NP[j];
				s=j-1;
				while(s>=r+1 && NP[s]->actualPathCost>key->actualPathCost)
				{
					NP[s+1]=NP[s];
					s=s-1;
				}
				NP[s+1]=key;
			}
			// for(j=r;j<lengthOfGraph;j++)
			// printf("%d:%d ",j,NP[j]->actualPathCost);
			r=r+1;
			//printf("r:%d,NP.curr:%d\n",r,NP[r]->currentNode);
			NP[0]->pathCost = NP[0]->pathCost - 1;
		}
        i=i+1;
	}
	if(fc)
	printf("Number of nodes expanded:%d\nDistance:Infinity\nRoute:None\n",numberOfNodesExpanded);
}

void getFinalPath(closedNode **finalClosedNP,int final,int lenghtofClosedNP,graph g,char **finalArr)
{
	int i,length = lenghtofClosedNP;
	int finalPatharray[lenghtofClosedNP];
	if(finalPatharray)
	{
		for(i=0;i<lenghtofClosedNP;i++)
		{
			finalPatharray[i] = -1;
		}
	}
	printf("Route:\n");
	while(1)
	{
		for(i=0;i<lenghtofClosedNP;i++)
		{
			if(finalClosedNP[i]->currentNode == final)
			{
				finalPatharray[--length] = final;
				break;
			}
		}
		if(finalClosedNP[i]->parentNode == -1)
		{	
      		break;
     	}
	    else
		{
			final = finalClosedNP[i]->parentNode;
		}
	}
	for(i=0;i<lenghtofClosedNP;i++)
	{
		if(finalPatharray[i]!=-1 && ((i+1)<lenghtofClosedNP))
		{
			printf("%s to %s,%d KM\n",finalArr[finalPatharray[i]],finalArr[finalPatharray[i+1]],g->edges[finalPatharray[i]][finalPatharray[i+1]]);
		}		
	}
}

int *parseHueristicDistance(char **finalArr,char *hueristicFileName,int length)
{
	FILE *fp = fopen(hueristicFileName,"r");
    int *hueristicDistance = (int*)malloc(sizeof(int)*length);
	if(fp == NULL)
	{
		printf("File can not be opened");
	}
	else
	{
		char *city = (char*)malloc(sizeof(*city));
		int distance,i;
		while(fscanf(fp,"%s%d\n",city,&distance) != EOF)
		{
			if(!strcmp(city,"END"))
			break;
			for(i=0;i<length;i++)
			{
				if(!strcmp(finalArr[i],city))
				{
					hueristicDistance[i] = distance;
					break;
				}
			}
			
		}
	}
	return hueristicDistance;
}	
