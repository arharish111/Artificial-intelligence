#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>

#include "find_route.h"
#include "arrays.h"

int main(int nargs,char **args)
{
  int length; 
  char **finalArray = getArrayOfNamesInTheFile(args[1],getNumberOfLinesInTheFile(args[1]),&length);
  int lengthofNP = ((length*length)-length)+1;
  graph g = createEmptyGraph(length);
  g = getInfoToCreateLinks(args[1],finalArray,g);
  node **nodePairs = createEmptyNodePairs(lengthofNP);
  int *hueristicDistance = createEmptyArray(length);
  if(nargs == 5)
	hueristicDistance = parseHueristicDistance(finalArray,args[4],length);
  uniformCostSearch(nodePairs,getPositionOfStringInAnArray(finalArray,args[2]),g,getPositionOfStringInAnArray(finalArray,args[3]),finalArray,hueristicDistance,length);
  return 0;
}
