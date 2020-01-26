#ifndef find_route_h
#define find_route_h

typedef struct struct_graph * graph;
typedef struct struct_node_pair  node;
typedef struct struct_closed_node_pair closedNode;
graph createEmptyGraph(int num);
node **createEmptyNodePairs(int num);
closedNode **createEmptyClosedNodePairs(int num);
graph createLinks(graph g,int v1,int v2,int pathCost);
graph getInfoToCreateLinks(char *fileName,char **finalArray,graph g);
int getNumberOfLinesInTheFile(char *fileName);
char **getArrayOfNamesInTheFile(char *fileName,int numberOfLinesInFile,int *);
void uniformCostSearch(node **NP,int start,graph g,int final,char **finalArr,int *hueristicDistance,int lengthOfGraph);
void getFinalPath(closedNode **finalClosedNP,int final,int lenghtofClosedNP,graph g,char **finalArr);
int *parseHueristicDistance(char **finalArr,char *hueristicFileName,int length);

#endif 

