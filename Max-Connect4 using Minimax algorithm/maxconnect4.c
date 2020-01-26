#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <math.h>

#define ROWS 6
#define COLUMNS 7

int maxPlayer,pieceCount,finalDepth,currentDepth,minPlayer,mode,internalPC;

void printCurrentStateAndScore(int **);
void printGameBoardToFile(int **,char *,int);
int computerPlays(int **);
int humanPlays(int**);
int chooseNextMove(int **);
int maxValue(int **,int,int);
int minValue(int **,int,int);
int getSucessor(int **,int,int);
int getEval(int **);
void freeGameBoard(int **);
int getNumberOfPieces(int **);

int main(int nargs,char **args)
{
	if (nargs != 5)
	{
		printf("Four command-line arguments are needed:\n");
		printf("Usage: %s interactive [input_file] [computer-next/human-next] [depth]\n", args[0]);
		printf("or:  %s one-move [input_file] [output_file] [depth]\n", args[0]);
		return 0;
	}
	int i,j=0,humanPlayer,computerPlayer,nextPlayer,depth,countPieces=0,result;
	FILE *inputFile;
	mode = !strcmp("interactive",args[1])?0:1;
	inputFile = fopen(args[2],"r");
	char *string = (char*)malloc(sizeof(string));
	int **gameBoard = (int**)malloc((sizeof(int*)*ROWS));
	if(gameBoard)
	{
		for(i=0;i<ROWS;i++)
		{
			gameBoard[i] = (int*)malloc(sizeof(int)*COLUMNS);
		}
	}
	if(inputFile != NULL)
	{
		for(i=0;i<ROWS;i++)
		{
			fgets(string,10,inputFile);
			while(string[j] != 13)
			{
				gameBoard[i][j] = string[j] - 48;
				if(gameBoard[i][j]!=0)
				countPieces = countPieces + 1;
				j++;
			}
			j=0;
		}
		fgets(string,3,inputFile);
		computerPlayer = string[0] - 48;
		maxPlayer = computerPlayer;
		humanPlayer = (computerPlayer==1)?2:1;
		minPlayer = humanPlayer;
	}
	fclose(inputFile);
	pieceCount=countPieces;
	if(mode==0)
	{
		nextPlayer = !strcmp("computer-next",args[3])?computerPlayer:humanPlayer;
	}
	else
	{
		nextPlayer = computerPlayer;
	}
	depth = atoi(args[4]);
	finalDepth = depth;
	if(mode==0)
	{
		if(nextPlayer==computerPlayer)
		{
			result = computerPlays(gameBoard);
		}
		else
		{
			result =  humanPlays(gameBoard);
		}
	}
	else
	{
		result = computerPlays(gameBoard);
		if(result)
		{
			printCurrentStateAndScore(gameBoard);
			printGameBoardToFile(gameBoard,args[3],maxPlayer);
		}
	}
	freeGameBoard(gameBoard);
	free(string);
	return 0;
}
void freeGameBoard(int **gameBoard)
{
	int i;
	for(i=0;i<ROWS;i++)
	{
		free(gameBoard[i]);
	}
	free(gameBoard);
}
int getNumberOfPieces(int **gameBoard)
{
	int count = 0,i,j;
	for(i=0;i<ROWS;i++)
	{
		for(j=0;j<COLUMNS;j++)
		{
			if(gameBoard[i][j]!=0)
				count = count + 1;
		}
	}
	return count;
}

int computerPlays(int **gameBoard)
{
	printCurrentStateAndScore(gameBoard);
	if(pieceCount == 42)
	{
		printf("Game Over.");
		return 0;
	}
	int i,j;
	int **actualGameBoard = (int**)malloc(sizeof(int*)*ROWS);
	if(actualGameBoard)
	{
		for(i=0;i<ROWS;i++)
		{
			actualGameBoard[i] = (int*)malloc(sizeof(int)*COLUMNS);
			for(j=0;j<COLUMNS;j++)
				actualGameBoard[i][j] = gameBoard[i][j];
		}
	}
	int result = chooseNextMove(actualGameBoard);
	if(result != -1)
	{
		result = getSucessor(gameBoard,result,maxPlayer);
		pieceCount = getNumberOfPieces(gameBoard);
	//	printf("PCC:%d\n",pieceCount);
		if(mode == 0)
		{
			printGameBoardToFile(gameBoard,"computer.txt",maxPlayer);
			freeGameBoard(actualGameBoard);
			return humanPlays(gameBoard);
		}
		else
		{
			freeGameBoard(actualGameBoard);
			return 1;
		}
	}
	else
	{
		if(mode==0)
		{
			freeGameBoard(actualGameBoard);
			return humanPlays(gameBoard);
		}
		else
		{
			freeGameBoard(actualGameBoard);
			return 1;
		}
	}
}
int humanPlays(int** gameBoard)
{
	printCurrentStateAndScore(gameBoard);
	if(pieceCount == 42)
	{
		printf("Game Over.");
		return 0;
	}
	int columnNumber,result,i,j;
	int **actualGameBoard = (int**)malloc(sizeof(int*)*ROWS);
	if(actualGameBoard)
	{
		for(i=0;i<ROWS;i++)
		{
			actualGameBoard[i] = (int*)malloc(sizeof(int)*COLUMNS);
			for(j=0;j<COLUMNS;j++)
				actualGameBoard[i][j] = gameBoard[i][j];
		}
	}
	while(1)
	{
		printf("Please enter a valid move(Column number between 1 and 7, which is not full):");
		scanf("%d",&columnNumber);
		columnNumber = columnNumber - 1;
		if(columnNumber<0 || columnNumber>6)
		continue;
		result = getSucessor(actualGameBoard,columnNumber,minPlayer);
		if(!result)
		continue;
		else
		break;
	}
	result = getSucessor(gameBoard,columnNumber,minPlayer);
	pieceCount = getNumberOfPieces(gameBoard);
//	printf("PC:%d\n",pieceCount);
	printGameBoardToFile(gameBoard,"human.txt",minPlayer);
	freeGameBoard(actualGameBoard);
	return computerPlays(gameBoard);
}
// Output current game status to file
void printGameBoardToFile(int **gameBoard,char *fileName,int player)
{
	FILE *outputFile = fopen(fileName,"w");
	if(outputFile)
	{
		int i = 0;
		int j = 0;
		for(i = 0; i < 6; i++)
		{	
			for(j = 0; j < 7; j++)
			{
				fprintf(outputFile,"%d",gameBoard[i][j]);
			}
			fprintf(outputFile,"\r\n");
		}
		fprintf(outputFile,"%d\r\n",player);
		fclose(outputFile);
	}
	else
	{
		printf("error opening file");
	}
}

int chooseNextMove(int **gameBoard)
{
	//printf("Here1\n");
	currentDepth = 0;
	int result,i,column,value,r,s,key;
	int alpha = -1000,beta = 1000;
	// result = maxValue(gameBoard,-1000,1000);
	//do finalDepth = 0 check
	if(finalDepth==0)
		return -1;
	int *actionsValue = (int*)malloc(sizeof(int)*COLUMNS);
	if(actionsValue)
	{
		for(i=0;i<COLUMNS;i++)
			actionsValue[i] = -1000;
	}
	int **gameBoardCopy = (int**)malloc(sizeof(int*)*ROWS);
	if(gameBoardCopy)
	{
		for(i=0;i<ROWS;i++)
		{
			gameBoardCopy[i] = (int*)malloc(sizeof(int)*COLUMNS);
		}
	}
	for(i=0;i<COLUMNS;i++)
	{
		//printf("AV:%d",actionsValue[i]);
		currentDepth = 1;
		for(r=0;r<ROWS;r++)
		{
			for(s=0;s<COLUMNS;s++)
			{
				gameBoardCopy[r][s] = gameBoard[r][s];
			}
		}
		result = getSucessor(gameBoardCopy,i,maxPlayer);
		if(!result)
		continue;
		else
		{
			internalPC = getNumberOfPieces(gameBoardCopy);
			value = minValue(gameBoardCopy,alpha,beta);
			alpha = (value>alpha)?value:alpha;
			actionsValue[i] = value;
		//	printf("AV---------------------:%d,%d\n",actionsValue[i],i);
		}
		// if(actionsValue[i]==result)
		// {
			// column = i;
			// printf("Here:%d,%d\n",column,i);
			// break;
		// }
	}
	// for(i=0;i<COLUMNS;i++)
	// {
		// if(actionsValue[i]==alpha)
		// {
			// column = i;
			// break;
		// }
	// }
	currentDepth = currentDepth - 1;
	key = -1000;
	for(s=0;s<COLUMNS;s++)
	{
		if(actionsValue[s]>key)
		{
			key = actionsValue[s];
			column = s;
		}

	}
	freeGameBoard(gameBoardCopy);
	//freeGameBoard(gameBoard);
	free(actionsValue);
	return column;
}
int maxValue(int **gameBoard,int alpha,int beta)
{
	//printf("InMax\n");
	if(finalDepth==currentDepth || internalPC == 42)
	{
		currentDepth = currentDepth - 1;
		return getEval(gameBoard);
	}
	int i,result,v = -1000,r,s;
	//printf("CDM:%d\n",currentDepth);
	int **gameBoardCopy = (int**)malloc(sizeof(int*)*ROWS);
	if(gameBoardCopy)
	{
		for(i=0;i<ROWS;i++)
		{
			gameBoardCopy[i] = (int*)malloc(sizeof(int)*COLUMNS);
		}
	}
	for(i=0;i<COLUMNS;i++)
	{
		for(r=0;r<ROWS;r++)
		{
			for(s=0;s<COLUMNS;s++)
			{
				gameBoardCopy[r][s] = gameBoard[r][s];
			}
		}
		result = getSucessor(gameBoardCopy,i,maxPlayer);
		if(!result)
		continue;
		else
		{
			internalPC = getNumberOfPieces(gameBoardCopy);
			currentDepth = currentDepth + 1;
			result = minValue(gameBoardCopy,alpha,beta);
			v = (v>result)?v:result;
			if(v>=beta)
			{
				freeGameBoard(gameBoardCopy);
				currentDepth = currentDepth - 1;
				return v;
			}
			alpha = (alpha>v)?alpha:v;
			// if(finalDepth1==1)
			// {
				// actionsValue[i] = alpha;
				// finalDepth1 = finalDepth;
			// }
		}
	}
	//finalDepth1 = finalDepth1 - 1;
	currentDepth = currentDepth - 1;
	freeGameBoard(gameBoardCopy);
	return v;
	
}
int minValue(int **gameBoard,int alpha,int beta)
{
	//printf("InMin\n");
	if(finalDepth==currentDepth || internalPC == 42)
	{
		currentDepth = currentDepth - 1;
		return getEval(gameBoard);
	}
	int i,result,v = 1000,r,s;
	//printf("CDMin:%d\n",currentDepth);
	int **gameBoardCopy = (int**)malloc(sizeof(int*)*ROWS);
	if(gameBoardCopy)
	{
		for(i=0;i<ROWS;i++)
		{
			gameBoardCopy[i] = (int*)malloc(sizeof(int)*COLUMNS);
		}
	}
	for(i=0;i<COLUMNS;i++)
	{
		for(r=0;r<ROWS;r++)
		{
			for(s=0;s<COLUMNS;s++)
			{
				gameBoardCopy[r][s] = gameBoard[r][s];
			}
		}
		result = getSucessor(gameBoardCopy,i,minPlayer);
		if(!result)
		continue;
		else
		{
			internalPC = getNumberOfPieces(gameBoardCopy);
			currentDepth = currentDepth + 1;
			result = maxValue(gameBoardCopy,alpha,beta);
			v = (v<result)?v:result;
			if(v<=alpha)
			{
				freeGameBoard(gameBoardCopy);
				currentDepth = currentDepth - 1;
				return v;
			}
			beta = 	(beta<v)?beta:v;
		}
	}
	//finalDepth1 = finalDepth1 - 1;
	currentDepth = currentDepth - 1;
	freeGameBoard(gameBoardCopy);
	return v;
}
int getSucessor(int **gameBoard,int columnNumber,int player)
{
	// if column full, return 1
	if(gameBoard[0][columnNumber] != 0)
	{
		return 0;
	}
	
	int i;
	// starting at the bottom of the board, place the piece into the
	// first empty spot
	for(i = 5; i >= 0; i--)
	{
		if(gameBoard[i][columnNumber] == 0)
		{
			gameBoard[i][columnNumber] = player;
			return 1;
		}
	}
	return 0;
}
int getEval(int **gameBoard)
{
	//printf("Here4\n");
	int i,j,maxPlayerCount=0,minPlayerCount=0,numberOfMaxPlayerPieces=0,numberOfMinPlayerPieces=0;
	int numberOfRowsBenefittingMax=0, numberOfRowsBenefittingMin=0,start=0,current=0,foundCheck=0;
	int evalValue = 0;
	//horizontal count
	for(i=0;i<ROWS;i++)
	{
		maxPlayerCount=0;
		minPlayerCount=0;
		numberOfMaxPlayerPieces=0;
		numberOfMinPlayerPieces=0;
		start =0;current=0;foundCheck=0;
		for(j=0;j<COLUMNS;j++)
		{
			if(gameBoard[i][j]==0)
			{
				numberOfMaxPlayerPieces = 0;
				numberOfMinPlayerPieces = 0;
				maxPlayerCount = maxPlayerCount + 1;
				minPlayerCount = minPlayerCount + 1;
			}
			else if(gameBoard[i][j]== maxPlayer)
			{
				numberOfMaxPlayerPieces = numberOfMaxPlayerPieces + 1;
				numberOfMinPlayerPieces = 0;
				maxPlayerCount = maxPlayerCount + 1;
				minPlayerCount = 0;
			}
			else
			{
				numberOfMinPlayerPieces = numberOfMinPlayerPieces + 1;
				numberOfMaxPlayerPieces = 0;
				minPlayerCount = minPlayerCount + 1;
				maxPlayerCount = 0;
			}
			if(maxPlayerCount==4)
			{
				numberOfRowsBenefittingMax = numberOfRowsBenefittingMax + 1;
			}
			if(minPlayerCount==4)
			{
				numberOfRowsBenefittingMin = numberOfRowsBenefittingMin + 1;
			}
			if(numberOfMaxPlayerPieces==4)
			{
				start = j-3;
				current = j;
				foundCheck = 0;
				while(start>0)
				{
					start = start -1;
					if(gameBoard[i][start]==0)
					{
						foundCheck = 1;
						break;
					}
					else if(gameBoard[i][start]==maxPlayer)
					{
						foundCheck = 0;
					}
					else
					{
						foundCheck = 0;
						break;
					}
				}
				if(!foundCheck)
				{
					while(current<6)
					{
						current = current + 1;
						if(gameBoard[i][start]==0)
						{
							foundCheck = 1;
							break;
						}
						else if(gameBoard[i][start]==maxPlayer)
						{
							foundCheck = 0;
						}
						else
						{
							foundCheck = 0;
							break;
						}
					}
				}
				if(!foundCheck)
				{
					numberOfRowsBenefittingMax = numberOfRowsBenefittingMax - 1;
					break;
				}
				else
				{
					//numberOfRowsBenefittingMax = numberOfRowsBenefittingMax + 4;
					//numberOfRowsBenefittingMin = numberOfRowsBenefittingMin + 5;
					break;
				}
			}
			if(numberOfMinPlayerPieces==4)
			{
				start = j-3;
				current = j;
				foundCheck = 0;
				while(start>0)
				{
					start = start -1;
					if(gameBoard[i][start]==0)
					{
						foundCheck = 1;
						break;
					}
					else if(gameBoard[i][start]!=maxPlayer)
					{
						foundCheck = 0;
					}
					else
					{
						foundCheck = 0;
						break;
					}
				}
				if(!foundCheck)
				{
					while(current<6)
					{
						current = current + 1;
						if(gameBoard[i][start]==0)
						{
							foundCheck = 1;
							break;
						}
						else if(gameBoard[i][start]!=maxPlayer)
						{
							foundCheck = 0;
						}
						else
						{
							foundCheck = 0;
							break;
						}
					}
				}
				if(!foundCheck)
				{
					numberOfRowsBenefittingMin = numberOfRowsBenefittingMin - 1;
					break;
				}
				else
				{
					//numberOfRowsBenefittingMin = numberOfRowsBenefittingMin + 4;
					//numberOfRowsBenefittingMax = numberOfRowsBenefittingMax + 5;
					break;
				}
			}
		}
		// if(numberOfMaxPlayerPieces == 7)
		// {
			// numberOfRowsBenefittingMax = numberOfRowsBenefittingMax - 1;
		// }
		// if(numberOfMinPlayerPieces == 7)
		// {
			// numberOfRowsBenefittingMin = numberOfRowsBenefittingMin - 1;
		// }
	}
	//vertical check
	for(j=0;j<COLUMNS;j++)
	{
		maxPlayerCount=0;
		minPlayerCount=0;
		numberOfMaxPlayerPieces=0;
		numberOfMinPlayerPieces=0;
		start =0;current=0;foundCheck=0;
		for(i=0;i<ROWS;i++)
		{
			if(gameBoard[i][j]==0)
			{
				numberOfMaxPlayerPieces = 0;
				numberOfMinPlayerPieces = 0;
				maxPlayerCount = maxPlayerCount + 1;
				minPlayerCount = minPlayerCount + 1;
			}
			else if(gameBoard[i][j]== maxPlayer)
			{
				numberOfMaxPlayerPieces = numberOfMaxPlayerPieces + 1;
				numberOfMinPlayerPieces = 0;
				maxPlayerCount = maxPlayerCount + 1;
				minPlayerCount = 0;
			}
			else
			{
				numberOfMinPlayerPieces = numberOfMinPlayerPieces + 1;
				numberOfMaxPlayerPieces = 0;
				minPlayerCount = minPlayerCount + 1;
				maxPlayerCount = 0;
			}
			if(maxPlayerCount==4)
			{
				numberOfRowsBenefittingMax = numberOfRowsBenefittingMax + 1;
			}
			if(minPlayerCount==4)
			{
				numberOfRowsBenefittingMin = numberOfRowsBenefittingMin + 1;
			}
			if(numberOfMaxPlayerPieces==4)
			{
				start = i-3;
				current = i;
				foundCheck = 0;
				while(start>0)
				{
					start = start -1;
					if(gameBoard[start][j]==0)
					{
						foundCheck = 1;
						break;
					}
					else if(gameBoard[start][j]==maxPlayer)
					{
						foundCheck = 0;
					}
					else
					{
						foundCheck = 0;
						break;
					}
				}
				if(!foundCheck)
				{
					while(current<5)
					{
						current = current + 1;
						if(gameBoard[start][j]==0)
						{
							foundCheck = 1;
							break;
						}
						else if(gameBoard[start][j]==maxPlayer)
						{
							foundCheck = 0;
						}
						else
						{
							foundCheck = 0;
							break;
						}
					}
				}
				if(!foundCheck)
				{
					numberOfRowsBenefittingMax = numberOfRowsBenefittingMax - 1;
					break;
				}
				else
				{
					//numberOfRowsBenefittingMax = numberOfRowsBenefittingMax + 4;
					//numberOfRowsBenefittingMin = numberOfRowsBenefittingMin + 5;
					break;
				}
			}
			if(numberOfMinPlayerPieces==4)
			{
				start = i-3;
				current = i;
				foundCheck = 0;
				while(start>0)
				{
					start = start -1;
					if(gameBoard[start][j]==0)
					{
						foundCheck = 1;
						break;
					}
					else if(gameBoard[start][j]!=maxPlayer)
					{
						foundCheck = 0;
					}
					else
					{
						foundCheck = 0;
						break;
					}
				}
				if(!foundCheck)
				{
					while(current<5)
					{
						current = current + 1;
						if(gameBoard[start][j]==0)
						{
							foundCheck = 1;
							break;
						}
						else if(gameBoard[start][j]!=maxPlayer)
						{
							foundCheck = 0;
						}
						else
						{
							foundCheck = 0;
							break;
						}
					}
				}
				if(!foundCheck)
				{
					numberOfRowsBenefittingMin = numberOfRowsBenefittingMin - 1;
					break;
				}
				else
				{
					//numberOfRowsBenefittingMin = numberOfRowsBenefittingMin + 4;
					//numberOfRowsBenefittingMax = numberOfRowsBenefittingMax + 5;
					break;
				}
			}
		}
	}
	//diagonal check
	maxPlayerCount=0;
	minPlayerCount=0;
	numberOfMaxPlayerPieces=0;
	numberOfMinPlayerPieces=0;
	for(i=3,j=0;i>=0,j<=3;i--,j++)
	{
		if(gameBoard[i][j]==0)
		{
			numberOfMaxPlayerPieces = 0;
			numberOfMinPlayerPieces = 0;
			maxPlayerCount = maxPlayerCount + 1;
			minPlayerCount = minPlayerCount + 1;
		}
		else if(gameBoard[i][j]== maxPlayer)
		{
			numberOfMaxPlayerPieces = numberOfMaxPlayerPieces + 1;
			numberOfMinPlayerPieces = 0;
			maxPlayerCount = maxPlayerCount + 1;
			minPlayerCount = 0;
		}
		else
		{
			numberOfMinPlayerPieces = numberOfMinPlayerPieces + 1;
			numberOfMaxPlayerPieces = 0;
			minPlayerCount = minPlayerCount + 1;
			maxPlayerCount = 0;
		}
		if(maxPlayerCount==4)
		{
			numberOfRowsBenefittingMax = numberOfRowsBenefittingMax + 1;
		}
		if(minPlayerCount==4)
		{
			numberOfRowsBenefittingMin = numberOfRowsBenefittingMin + 1;
		}
		if(numberOfMaxPlayerPieces==4)
		{
			numberOfRowsBenefittingMax = numberOfRowsBenefittingMax - 1;
		}
		if(numberOfMinPlayerPieces==4)
		{
			numberOfRowsBenefittingMin = numberOfRowsBenefittingMin - 1;
		}
	}
	maxPlayerCount=0;
	minPlayerCount=0;
	numberOfMaxPlayerPieces=0;
	numberOfMinPlayerPieces=0;
	for(i=4,j=0;i>=0,j<=4;i--,j++)
	{
		if(gameBoard[i][j]==0)
		{
			numberOfMaxPlayerPieces = 0;
			numberOfMinPlayerPieces = 0;
			maxPlayerCount = maxPlayerCount + 1;
			minPlayerCount = minPlayerCount + 1;
		}
		else if(gameBoard[i][j]== maxPlayer)
		{
			numberOfMaxPlayerPieces = numberOfMaxPlayerPieces + 1;
			numberOfMinPlayerPieces = 0;
			maxPlayerCount = maxPlayerCount + 1;
			minPlayerCount = 0;
		}
		else
		{
			numberOfMinPlayerPieces = numberOfMinPlayerPieces + 1;
			numberOfMaxPlayerPieces = 0;
			minPlayerCount = minPlayerCount + 1;
			maxPlayerCount = 0;
		}
		if(maxPlayerCount==4)
		{
			numberOfRowsBenefittingMax = numberOfRowsBenefittingMax + 1;
		}
		if(minPlayerCount==4)
		{
			numberOfRowsBenefittingMin = numberOfRowsBenefittingMin + 1;
		}
		if(numberOfMaxPlayerPieces==4)
		{
			if(gameBoard[4][0]==0 || gameBoard[0][4]==0)
			{
				//numberOfRowsBenefittingMax = numberOfRowsBenefittingMax + 1;
				//numberOfRowsBenefittingMin = numberOfRowsBenefittingMin + 5;
				break;
			}
			else
			{
				numberOfRowsBenefittingMax = numberOfRowsBenefittingMax - 1;
				break;
			}
		}
		if(numberOfMinPlayerPieces==4)
		{
			if(gameBoard[4][0]==0 || gameBoard[0][4]==0)
			{
				//numberOfRowsBenefittingMin = numberOfRowsBenefittingMin + 1;
				//numberOfRowsBenefittingMax = numberOfRowsBenefittingMax + 5;
				break;
			}
			else
			{
				numberOfRowsBenefittingMin = numberOfRowsBenefittingMin - 1;
				break;
			}
		}
	}
	maxPlayerCount=0;
	minPlayerCount=0;
	numberOfMaxPlayerPieces=0;
	numberOfMinPlayerPieces=0;
	for(i=5,j=0;i>=0,j<=5;i--,j++)
	{
		if(gameBoard[i][j]==0)
		{
			numberOfMaxPlayerPieces = 0;
			numberOfMinPlayerPieces = 0;
			maxPlayerCount = maxPlayerCount + 1;
			minPlayerCount = minPlayerCount + 1;
		}
		else if(gameBoard[i][j]== maxPlayer)
		{
			numberOfMaxPlayerPieces = numberOfMaxPlayerPieces + 1;
			numberOfMinPlayerPieces = 0;
			maxPlayerCount = maxPlayerCount + 1;
			minPlayerCount = 0;
		}
		else
		{
			numberOfMinPlayerPieces = numberOfMinPlayerPieces + 1;
			numberOfMaxPlayerPieces = 0;
			minPlayerCount = minPlayerCount + 1;
			maxPlayerCount = 0;
		}
		if(maxPlayerCount==4)
		{
			numberOfRowsBenefittingMax = numberOfRowsBenefittingMax + 1;
		}
		if(minPlayerCount==4)
		{
			numberOfRowsBenefittingMin = numberOfRowsBenefittingMin + 1;
		}
		if(numberOfMaxPlayerPieces==4)
		{
			if(gameBoard[1][4]==0 || gameBoard[4][1]==0 || gameBoard[5][0]==0 || gameBoard[0][5]==0)
			{
				//numberOfRowsBenefittingMax = numberOfRowsBenefittingMax + 1;
				//numberOfRowsBenefittingMin = numberOfRowsBenefittingMin + 5;
				break;
			}
			else
			{
				numberOfRowsBenefittingMax = numberOfRowsBenefittingMax - 1;
				break;
			}
		}
		if(numberOfMinPlayerPieces==4)
		{
			if(gameBoard[1][4]==0 || gameBoard[4][1]==0 || gameBoard[5][0]==0 || gameBoard[0][5]==0)
			{
				//numberOfRowsBenefittingMin = numberOfRowsBenefittingMin + 1;
				//numberOfRowsBenefittingMax = numberOfRowsBenefittingMax + 5;
				break;
			}
			else
			{
				numberOfRowsBenefittingMin = numberOfRowsBenefittingMin - 1;
				break;
			}
		}
	}
	maxPlayerCount=0;
	minPlayerCount=0;
	numberOfMaxPlayerPieces=0;
	numberOfMinPlayerPieces=0;
	for(i=5,j=1;i>=0,j<=6;i--,j++)
	{
		if(gameBoard[i][j]==0)
		{
			numberOfMaxPlayerPieces = 0;
			numberOfMinPlayerPieces = 0;
			maxPlayerCount = maxPlayerCount + 1;
			minPlayerCount = minPlayerCount + 1;
		}
		else if(gameBoard[i][j]== maxPlayer)
		{
			numberOfMaxPlayerPieces = numberOfMaxPlayerPieces + 1;
			numberOfMinPlayerPieces = 0;
			maxPlayerCount = maxPlayerCount + 1;
			minPlayerCount = 0;
		}
		else
		{
			numberOfMinPlayerPieces = numberOfMinPlayerPieces + 1;
			numberOfMaxPlayerPieces = 0;
			minPlayerCount = minPlayerCount + 1;
			maxPlayerCount = 0;
		}
		if(maxPlayerCount==4)
		{
			numberOfRowsBenefittingMax = numberOfRowsBenefittingMax + 1;
		}
		if(minPlayerCount==4)
		{
			numberOfRowsBenefittingMin = numberOfRowsBenefittingMin + 1;
		}
		if(numberOfMaxPlayerPieces==4)
		{
			if(gameBoard[1][5]==0 || gameBoard[4][2]==0 || gameBoard[5][1]==0 || gameBoard[0][6]==0)
			{
				//numberOfRowsBenefittingMax = numberOfRowsBenefittingMax + 1;
				//numberOfRowsBenefittingMin = numberOfRowsBenefittingMin + 5;
				break;
			}
			else
			{
				numberOfRowsBenefittingMax = numberOfRowsBenefittingMax - 1;
				break;
			}
		}
		if(numberOfMinPlayerPieces==4)
		{
			if(gameBoard[1][5]==0 || gameBoard[4][2]==0 || gameBoard[5][1]==0 || gameBoard[0][6]==0)
			{
				//numberOfRowsBenefittingMin = numberOfRowsBenefittingMin + 1;
				//numberOfRowsBenefittingMax = numberOfRowsBenefittingMax + 5;
				break;
			}
			else
			{
				numberOfRowsBenefittingMin = numberOfRowsBenefittingMin - 1;
				break;
			}
		}
	}
	maxPlayerCount=0;
	minPlayerCount=0;
	numberOfMaxPlayerPieces=0;
	numberOfMinPlayerPieces=0;
	for(i=5,j=2;i>=1,j<=6;i--,j++)
	{
		if(gameBoard[i][j]==0)
		{
			numberOfMaxPlayerPieces = 0;
			numberOfMinPlayerPieces = 0;
			maxPlayerCount = maxPlayerCount + 1;
			minPlayerCount = minPlayerCount + 1;
		}
		else if(gameBoard[i][j]== maxPlayer)
		{
			numberOfMaxPlayerPieces = numberOfMaxPlayerPieces + 1;
			numberOfMinPlayerPieces = 0;
			maxPlayerCount = maxPlayerCount + 1;
			minPlayerCount = 0;
		}
		else
		{
			numberOfMinPlayerPieces = numberOfMinPlayerPieces + 1;
			numberOfMaxPlayerPieces = 0;
			minPlayerCount = minPlayerCount + 1;
			maxPlayerCount = 0;
		}
		if(maxPlayerCount==4)
		{
			numberOfRowsBenefittingMax = numberOfRowsBenefittingMax + 1;
		}
		if(minPlayerCount==4)
		{
			numberOfRowsBenefittingMin = numberOfRowsBenefittingMin + 1;
		}
		if(numberOfMaxPlayerPieces==4)
		{
			if(gameBoard[5][2]==0 || gameBoard[1][6]==0)
			{
				//numberOfRowsBenefittingMax = numberOfRowsBenefittingMax + 1;
				//numberOfRowsBenefittingMin = numberOfRowsBenefittingMin + 5;
				break;
			}
			else
			{
				numberOfRowsBenefittingMax = numberOfRowsBenefittingMax - 1;
				break;
			}
		}
		if(numberOfMinPlayerPieces==4)
		{
			if(gameBoard[5][2]==0 || gameBoard[1][6]==0)
			{
				//numberOfRowsBenefittingMin = numberOfRowsBenefittingMin + 1;
				//numberOfRowsBenefittingMax = numberOfRowsBenefittingMax + 5;
				break;
			}
			else
			{
				numberOfRowsBenefittingMin = numberOfRowsBenefittingMin - 1;
				break;
			}
		}
		
	}
	maxPlayerCount=0;
	minPlayerCount=0;
	numberOfMaxPlayerPieces=0;
	numberOfMinPlayerPieces=0;
	for(i=5,j=3;i>=2,j<=6;i--,j++)
	{
		if(gameBoard[i][j]==0)
		{
			numberOfMaxPlayerPieces = 0;
			numberOfMinPlayerPieces = 0;
			maxPlayerCount = maxPlayerCount + 1;
			minPlayerCount = minPlayerCount + 1;
		}
		else if(gameBoard[i][j]== maxPlayer)
		{
			numberOfMaxPlayerPieces = numberOfMaxPlayerPieces + 1;
			numberOfMinPlayerPieces = 0;
			maxPlayerCount = maxPlayerCount + 1;
			minPlayerCount = 0;
		}
		else
		{
			numberOfMinPlayerPieces = numberOfMinPlayerPieces + 1;
			numberOfMaxPlayerPieces = 0;
			minPlayerCount = minPlayerCount + 1;
			maxPlayerCount = 0;
		}
		if(maxPlayerCount==4)
		{
			numberOfRowsBenefittingMax = numberOfRowsBenefittingMax + 1;
		}
		if(minPlayerCount==4)
		{
			numberOfRowsBenefittingMin = numberOfRowsBenefittingMin + 1;
		}
		if(numberOfMaxPlayerPieces==4)
		{
			numberOfRowsBenefittingMax = numberOfRowsBenefittingMax - 1;
		}
		if(numberOfMinPlayerPieces==4)
		{
			numberOfRowsBenefittingMin = numberOfRowsBenefittingMin - 1;
		}
	}
	//diagonal - 2
	maxPlayerCount=0;
	minPlayerCount=0;
	numberOfMaxPlayerPieces=0;
	numberOfMinPlayerPieces=0;
	for(i=2,j=0;i<=5,j<=3;i++,j++)
	{
		if(gameBoard[i][j]==0)
		{
			numberOfMaxPlayerPieces = 0;
			numberOfMinPlayerPieces = 0;
			maxPlayerCount = maxPlayerCount + 1;
			minPlayerCount = minPlayerCount + 1;
		}
		else if(gameBoard[i][j]== maxPlayer)
		{
			numberOfMaxPlayerPieces = numberOfMaxPlayerPieces + 1;
			numberOfMinPlayerPieces = 0;
			maxPlayerCount = maxPlayerCount + 1;
			minPlayerCount = 0;
		}
		else
		{
			numberOfMinPlayerPieces = numberOfMinPlayerPieces + 1;
			numberOfMaxPlayerPieces = 0;
			minPlayerCount = minPlayerCount + 1;
			maxPlayerCount = 0;
		}
		if(maxPlayerCount==4)
		{
			numberOfRowsBenefittingMax = numberOfRowsBenefittingMax + 1;
		}
		if(minPlayerCount==4)
		{
			numberOfRowsBenefittingMin = numberOfRowsBenefittingMin + 1;
		}
		if(numberOfMaxPlayerPieces==4)
		{
			numberOfRowsBenefittingMax = numberOfRowsBenefittingMax - 1;
		}
		if(numberOfMinPlayerPieces==4)
		{
			numberOfRowsBenefittingMin = numberOfRowsBenefittingMin - 1;
		}
	}
	maxPlayerCount=0;
	minPlayerCount=0;
	numberOfMaxPlayerPieces=0;
	numberOfMinPlayerPieces=0;
	for(i=1,j=0;i<=5,j<=4;i++,j++)
	{
		if(gameBoard[i][j]==0)
		{
			numberOfMaxPlayerPieces = 0;
			numberOfMinPlayerPieces = 0;
			maxPlayerCount = maxPlayerCount + 1;
			minPlayerCount = minPlayerCount + 1;
		}
		else if(gameBoard[i][j]== maxPlayer)
		{
			numberOfMaxPlayerPieces = numberOfMaxPlayerPieces + 1;
			numberOfMinPlayerPieces = 0;
			maxPlayerCount = maxPlayerCount + 1;
			minPlayerCount = 0;
		}
		else
		{
			numberOfMinPlayerPieces = numberOfMinPlayerPieces + 1;
			numberOfMaxPlayerPieces = 0;
			minPlayerCount = minPlayerCount + 1;
			maxPlayerCount = 0;
		}
		if(maxPlayerCount==4)
		{
			numberOfRowsBenefittingMax = numberOfRowsBenefittingMax + 1;
		}
		if(minPlayerCount==4)
		{
			numberOfRowsBenefittingMin = numberOfRowsBenefittingMin + 1;
		}
		if(numberOfMaxPlayerPieces==4)
		{
			if(gameBoard[1][0]==0 || gameBoard[5][4]==0)
			{
				//numberOfRowsBenefittingMax = numberOfRowsBenefittingMax + 1;
				//numberOfRowsBenefittingMin = numberOfRowsBenefittingMin + 5;
				break;
			}
			else
			{
				numberOfRowsBenefittingMax = numberOfRowsBenefittingMax - 1;
				break;
			}
		}
		if(numberOfMinPlayerPieces==4)
		{
			if(gameBoard[1][0]==0 || gameBoard[5][4]==0)
			{
				//numberOfRowsBenefittingMin = numberOfRowsBenefittingMin + 1;
				//numberOfRowsBenefittingMax = numberOfRowsBenefittingMax + 5;
				break;
			}
			else
			{
				numberOfRowsBenefittingMin = numberOfRowsBenefittingMin - 1;
				break;
			}
		}
	}
	maxPlayerCount=0;
	minPlayerCount=0;
	numberOfMaxPlayerPieces=0;
	numberOfMinPlayerPieces=0;
	for(i=0,j=0;i<=5,j<=5;i++,j++)
	{
		if(gameBoard[i][j]==0)
		{
			numberOfMaxPlayerPieces = 0;
			numberOfMinPlayerPieces = 0;
			maxPlayerCount = maxPlayerCount + 1;
			minPlayerCount = minPlayerCount + 1;
		}
		else if(gameBoard[i][j]== maxPlayer)
		{
			numberOfMaxPlayerPieces = numberOfMaxPlayerPieces + 1;
			numberOfMinPlayerPieces = 0;
			maxPlayerCount = maxPlayerCount + 1;
			minPlayerCount = 0;
		}
		else
		{
			numberOfMinPlayerPieces = numberOfMinPlayerPieces + 1;
			numberOfMaxPlayerPieces = 0;
			minPlayerCount = minPlayerCount + 1;
			maxPlayerCount = 0;
		}
		if(maxPlayerCount==4)
		{
			numberOfRowsBenefittingMax = numberOfRowsBenefittingMax + 1;
		}
		if(minPlayerCount==4)
		{
			numberOfRowsBenefittingMin = numberOfRowsBenefittingMin + 1;
		}
		if(numberOfMaxPlayerPieces==4)
		{
			if(gameBoard[1][1]==0 || gameBoard[4][4]==0 || gameBoard[0][0]==0 || gameBoard[5][5]==0)
			{
				//numberOfRowsBenefittingMax = numberOfRowsBenefittingMax + 1;
				//numberOfRowsBenefittingMin = numberOfRowsBenefittingMin + 5;
				break;
			}
			else
			{
				numberOfRowsBenefittingMax = numberOfRowsBenefittingMax - 1;
				break;
			}
		}
		if(numberOfMinPlayerPieces==4)
		{
			if(gameBoard[1][1]==0 || gameBoard[4][4]==0 || gameBoard[0][0]==0 || gameBoard[5][5]==0)
			{
				//numberOfRowsBenefittingMin = numberOfRowsBenefittingMin + 1;
				//numberOfRowsBenefittingMax = numberOfRowsBenefittingMax + 5;
				break;
			}
			else
			{
				numberOfRowsBenefittingMin = numberOfRowsBenefittingMin - 1;
				break;
			}
		}
	}
	maxPlayerCount=0;
	minPlayerCount=0;
	numberOfMaxPlayerPieces=0;
	numberOfMinPlayerPieces=0;
	for(i=0,j=1;i<=5,j<=6;i++,j++)
	{
		if(gameBoard[i][j]==0)
		{
			numberOfMaxPlayerPieces = 0;
			numberOfMinPlayerPieces = 0;
			maxPlayerCount = maxPlayerCount + 1;
			minPlayerCount = minPlayerCount + 1;
		}
		else if(gameBoard[i][j]== maxPlayer)
		{
			numberOfMaxPlayerPieces = numberOfMaxPlayerPieces + 1;
			numberOfMinPlayerPieces = 0;
			maxPlayerCount = maxPlayerCount + 1;
			minPlayerCount = 0;
		}
		else
		{
			numberOfMinPlayerPieces = numberOfMinPlayerPieces + 1;
			numberOfMaxPlayerPieces = 0;
			minPlayerCount = minPlayerCount + 1;
			maxPlayerCount = 0;
		}
		if(maxPlayerCount==4)
		{
			numberOfRowsBenefittingMax = numberOfRowsBenefittingMax + 1;
		}
		if(minPlayerCount==4)
		{
			numberOfRowsBenefittingMin = numberOfRowsBenefittingMin + 1;
		}
		if(numberOfMaxPlayerPieces==4)
		{
			if(gameBoard[1][2]==0 || gameBoard[4][5]==0 || gameBoard[0][1]==0 || gameBoard[5][6]==0)
			{
				//numberOfRowsBenefittingMax = numberOfRowsBenefittingMax + 1;
				//numberOfRowsBenefittingMin = numberOfRowsBenefittingMin + 5;
				break;
			}
			else
			{
				numberOfRowsBenefittingMax = numberOfRowsBenefittingMax - 1;
				break;
			}
		}
		if(numberOfMinPlayerPieces==4)
		{
			if(gameBoard[1][2]==0 || gameBoard[4][5]==0 || gameBoard[0][1]==0 || gameBoard[5][6]==0)
			{
				//numberOfRowsBenefittingMin = numberOfRowsBenefittingMin + 1;
				//numberOfRowsBenefittingMax = numberOfRowsBenefittingMax + 5;
				break;
			}
			else
			{
				numberOfRowsBenefittingMin = numberOfRowsBenefittingMin - 1;
				break;
			}
		}
	}
	maxPlayerCount=0;
	minPlayerCount=0;
	numberOfMaxPlayerPieces=0;
	numberOfMinPlayerPieces=0;
	for(i=0,j=2;i<=4,j<=6;i++,j++)
	{
		if(gameBoard[i][j]==0)
		{
			numberOfMaxPlayerPieces = 0;
			numberOfMinPlayerPieces = 0;
			maxPlayerCount = maxPlayerCount + 1;
			minPlayerCount = minPlayerCount + 1;
		}
		else if(gameBoard[i][j]== maxPlayer)
		{
			numberOfMaxPlayerPieces = numberOfMaxPlayerPieces + 1;
			numberOfMinPlayerPieces = 0;
			maxPlayerCount = maxPlayerCount + 1;
			minPlayerCount = 0;
		}
		else
		{
			numberOfMinPlayerPieces = numberOfMinPlayerPieces + 1;
			numberOfMaxPlayerPieces = 0;
			minPlayerCount = minPlayerCount + 1;
			maxPlayerCount = 0;
		}
		if(maxPlayerCount==4)
		{
			numberOfRowsBenefittingMax = numberOfRowsBenefittingMax + 1;
		}
		if(minPlayerCount==4)
		{
			numberOfRowsBenefittingMin = numberOfRowsBenefittingMin + 1;
		}
		if(numberOfMaxPlayerPieces==4)
		{
			if(gameBoard[0][2]==0 || gameBoard[4][6]==0)
			{
				//numberOfRowsBenefittingMax = numberOfRowsBenefittingMax + 1;
				//numberOfRowsBenefittingMin = numberOfRowsBenefittingMin + 5;
				break;
			}
			else
			{
				numberOfRowsBenefittingMax = numberOfRowsBenefittingMax - 1;
				break;
			}
		}
		if(numberOfMinPlayerPieces==4)
		{
			if(gameBoard[0][2]==0 || gameBoard[4][6]==0)
			{
				//numberOfRowsBenefittingMin = numberOfRowsBenefittingMin + 1;
				//numberOfRowsBenefittingMax = numberOfRowsBenefittingMax + 5;
				break;
			}
			else
			{
				numberOfRowsBenefittingMin = numberOfRowsBenefittingMin - 1;
				break;
			}
		}
		
	}
	maxPlayerCount=0;
	minPlayerCount=0;
	numberOfMaxPlayerPieces=0;
	numberOfMinPlayerPieces=0;
	for(i=0,j=3;i<=3,j<=6;i++,j++)
	{
		if(gameBoard[i][j]==0)
		{
			numberOfMaxPlayerPieces = 0;
			numberOfMinPlayerPieces = 0;
			maxPlayerCount = maxPlayerCount + 1;
			minPlayerCount = minPlayerCount + 1;
		}
		else if(gameBoard[i][j]== maxPlayer)
		{
			numberOfMaxPlayerPieces = numberOfMaxPlayerPieces + 1;
			numberOfMinPlayerPieces = 0;
			maxPlayerCount = maxPlayerCount + 1;
			minPlayerCount = 0;
		}
		else
		{
			numberOfMinPlayerPieces = numberOfMinPlayerPieces + 1;
			numberOfMaxPlayerPieces = 0;
			minPlayerCount = minPlayerCount + 1;
			maxPlayerCount = 0;
		}
		if(maxPlayerCount==4)
		{
			numberOfRowsBenefittingMax = numberOfRowsBenefittingMax + 1;
		}
		if(minPlayerCount==4)
		{
			numberOfRowsBenefittingMin = numberOfRowsBenefittingMin + 1;
		}
		if(numberOfMaxPlayerPieces==4)
		{
			numberOfRowsBenefittingMax = numberOfRowsBenefittingMax - 1;
		}
		if(numberOfMinPlayerPieces==4)
		{
			numberOfRowsBenefittingMin = numberOfRowsBenefittingMin - 1;
		}
	}
	evalValue = numberOfRowsBenefittingMax - numberOfRowsBenefittingMin;
	//printf("%d%d\n",numberOfRowsBenefittingMax,numberOfRowsBenefittingMin);
	return evalValue;
}
void printCurrentStateAndScore(int **gameBoard)
{
	int i,j,player1Score=0,player2Score=0;
	printf(" -----------------\n");
	for(i=0;i<ROWS;i++)
	{
		printf(" | ");
		for(j=0;j<COLUMNS;j++)
			printf("%d ",gameBoard[i][j]);
		printf("| \n");
	}
	printf(" -----------------\n");
	//printf("%d%d%d%d\n",gameBoard[3][0],gameBoard[2][1],gameBoard[1][2],gameBoard[0][3]);
	for(i = 0; i < 6; i++)
	{
		//check player 1
		if(gameBoard[i][0] == 1 && gameBoard[i][1] == 1 
			&& gameBoard[i][2] == 1 && gameBoard[i][3] == 1)
			{player1Score++;}
		if(gameBoard[i][1] == 1 && gameBoard[i][2] == 1 
			&& gameBoard[i][3] == 1 && gameBoard[i][4] == 1)
			{player1Score++;}
		if(gameBoard[i][2] == 1 && gameBoard[i][3] == 1 
			&& gameBoard[i][4] == 1 && gameBoard[i][5] == 1)
			{player1Score++;}
		if(gameBoard[i][3] == 1 && gameBoard[i][4] == 1 
			&& gameBoard[i][5] == 1 && gameBoard[i][6] == 1)
			{player1Score++;}
		//check player 2
		if(gameBoard[i][0] == 2 && gameBoard[i][1] == 2 
			&& gameBoard[i][2] == 2 && gameBoard[i][3] == 2)
			{player2Score++;}
		if(gameBoard[i][1] == 2 && gameBoard[i][2] == 2 
			&& gameBoard[i][3] == 2 && gameBoard[i][4] == 2)
			{player2Score++;}
		if(gameBoard[i][2] == 2 && gameBoard[i][3] == 2 
			&& gameBoard[i][4] == 2 && gameBoard[i][5] == 2)
			{player2Score++;}
		if(gameBoard[i][3] == 2 && gameBoard[i][4] == 2 
			&& gameBoard[i][5] == 2 && gameBoard[i][6] == 2)
			{player2Score++;}
	}

	//check vertically
	for(j = 0; j < 7; j++)
	{
		//check player 1
		if(gameBoard[0][j] == 1 && gameBoard[1][j] == 1 
			&& gameBoard[2][j] == 1 && gameBoard[3][j] == 1)
			{player1Score++;}
		if(gameBoard[1][j] == 1 && gameBoard[2][j] == 1 
			&& gameBoard[3][j] == 1 && gameBoard[4][j] == 1)
			{player1Score++;}
		if(gameBoard[2][j] == 1 && gameBoard[3][j] == 1 
			&& gameBoard[4][j] == 1 && gameBoard[5][j] == 1)
			{player1Score++;}
		//check player 2
		if(gameBoard[0][j] == 2 && gameBoard[1][j] == 2 
			&& gameBoard[2][j] == 2 && gameBoard[3][j] == 2)
			{player2Score++;}
		if(gameBoard[1][j] == 2 && gameBoard[2][j] == 2 
			&& gameBoard[3][j] == 2 && gameBoard[4][j] == 2)
			{player2Score++;}
		if(gameBoard[2][j] == 2 && gameBoard[3][j] == 2 
			&& gameBoard[4][j] == 2 && gameBoard[5][j] == 2)
			{player2Score++;}
	}

	//check diagonally
	
	//check player 1
	if(gameBoard[2][0] == 1 && gameBoard[3][1] == 1 
		&& gameBoard[4][2] == 1 && gameBoard[5][3] == 1)
			{player1Score++;}
	if(gameBoard[1][0] == 1 && gameBoard[2][1] == 1 
		&& gameBoard[3][2] == 1 && gameBoard[4][3] == 1)
			{player1Score++;}
	if(gameBoard[2][1] == 1 && gameBoard[3][2] == 1 
		&& gameBoard[4][3] == 1 && gameBoard[5][4] == 1)
			{player1Score++;}
	if(gameBoard[0][0] == 1 && gameBoard[1][1] == 1 
		&& gameBoard[2][2] == 1 && gameBoard[3][3] == 1)
			{player1Score++;}
	if(gameBoard[1][1] == 1 && gameBoard[2][2] == 1 
		&& gameBoard[3][3] == 1 && gameBoard[4][4] == 1)
			{player1Score++;}
	if(gameBoard[2][2] == 1 && gameBoard[3][3] == 1 
		&& gameBoard[4][4] == 1 && gameBoard[5][5] == 1)
			{player1Score++;}
	if(gameBoard[0][1] == 1 && gameBoard[1][2] == 1 
		&& gameBoard[2][3] == 1 && gameBoard[3][4] == 1)
			{player1Score++;}
	if(gameBoard[1][2] == 1 && gameBoard[2][3] == 1 
		&& gameBoard[3][4] == 1 && gameBoard[4][5] == 1)
			{player1Score++;}
	if(gameBoard[2][3] == 1 && gameBoard[3][4] == 1 
		&& gameBoard[4][5] == 1 && gameBoard[5][6] == 1)
			{player1Score++;}
	if(gameBoard[0][2] == 1 && gameBoard[1][3] == 1 
		&& gameBoard[2][4] == 1 && gameBoard[3][5] == 1)
			{player1Score++;}
	if(gameBoard[1][3] == 1 && gameBoard[2][4] == 1 
		&& gameBoard[3][5] == 1 && gameBoard[4][6] == 1)
			{player1Score++;}
	if(gameBoard[0][3] == 1 && gameBoard[1][4] == 1 
		&& gameBoard[2][5] == 1 && gameBoard[3][6] == 1)
			{player1Score++;}

	if(gameBoard[0][3] == 1 && gameBoard[1][2] == 1 
		&& gameBoard[2][1] == 1 && gameBoard[3][0] == 1)
			{player1Score++;}
	if(gameBoard[0][4] == 1 && gameBoard[1][3] == 1 
		&& gameBoard[2][2] == 1 && gameBoard[3][1] == 1)
			{player1Score++;}
	if(gameBoard[1][3] == 1 && gameBoard[2][2] == 1 
		&& gameBoard[3][1] == 1 && gameBoard[4][0] == 1)
			{player1Score++;}
	if(gameBoard[0][5] == 1 && gameBoard[1][4] == 1 
		&& gameBoard[2][3] == 1 && gameBoard[3][2] == 1)
			{player1Score++;}
	if(gameBoard[1][4] == 1 && gameBoard[2][3] == 1 
		&& gameBoard[3][2] == 1 && gameBoard[4][1] == 1)
			{player1Score++;}
	if(gameBoard[2][3] == 1 && gameBoard[3][2] == 1 
		&& gameBoard[4][1] == 1 && gameBoard[5][0] == 1)
			{player1Score++;}
	if(gameBoard[0][6] == 1 && gameBoard[1][5] == 1 
		&& gameBoard[2][4] == 1 && gameBoard[3][3] == 1)
			{player1Score++;}
	if(gameBoard[1][5] == 1 && gameBoard[2][4] == 1 
		&& gameBoard[3][3] == 1 && gameBoard[4][2] == 1)
			{player1Score++;}
	if(gameBoard[2][4] == 1 && gameBoard[3][3] == 1 
		&& gameBoard[4][2] == 1 && gameBoard[5][1] == 1)
			{player1Score++;}
	if(gameBoard[1][6] == 1 && gameBoard[2][5] == 1 
		&& gameBoard[3][4] == 1 && gameBoard[4][3] == 1)
			{player1Score++;}
	if(gameBoard[2][5] == 1 && gameBoard[3][4] == 1 
		&& gameBoard[4][3] == 1 && gameBoard[5][2] == 1)
			{player1Score++;}
	if(gameBoard[2][6] == 1 && gameBoard[3][5] == 1 
		&& gameBoard[4][4] == 1 && gameBoard[5][3] == 1)
			{player1Score++;}
		
	//check player 2
	if(gameBoard[2][0] == 2 && gameBoard[3][1] == 2 
		&& gameBoard[4][2] == 2 && gameBoard[5][3] == 2)
			{player2Score++;}
	if(gameBoard[1][0] == 2 && gameBoard[2][1] == 2 
		&& gameBoard[3][2] == 2 && gameBoard[4][3] == 2)
			{player2Score++;}
	if(gameBoard[2][1] == 2 && gameBoard[3][2] == 2 
		&& gameBoard[4][3] == 2 && gameBoard[5][4] == 2)
			{player2Score++;}
	if(gameBoard[0][0] == 2 && gameBoard[1][1] == 2 
		&& gameBoard[2][2] == 2 && gameBoard[3][3] == 2)
			{player2Score++;}
	if(gameBoard[1][1] == 2 && gameBoard[2][2] == 2 
		&& gameBoard[3][3] == 2 && gameBoard[4][4] == 2)
			{player2Score++;}
	if(gameBoard[2][2] == 2 && gameBoard[3][3] == 2 
		&& gameBoard[4][4] == 2 && gameBoard[5][5] == 2)
			{player2Score++;}
	if(gameBoard[0][1] == 2 && gameBoard[1][2] == 2 
		&& gameBoard[2][3] == 2 && gameBoard[3][4] == 2)
			{player2Score++;}
	if(gameBoard[1][2] == 2 && gameBoard[2][3] == 2 
		&& gameBoard[3][4] == 2 && gameBoard[4][5] == 2)
			{player2Score++;}
	if(gameBoard[2][3] == 2 && gameBoard[3][4] == 2 
		&& gameBoard[4][5] == 2 && gameBoard[5][6] == 2)
			{player2Score++;}
	if(gameBoard[0][2] == 2 && gameBoard[1][3] == 2 
		&& gameBoard[2][4] == 2 && gameBoard[3][5] == 2)
			{player2Score++;}
	if(gameBoard[1][3] == 2 && gameBoard[2][4] == 2 
		&& gameBoard[3][5] == 2 && gameBoard[4][6] == 2)
			{player2Score++;}
	if(gameBoard[0][3] == 2 && gameBoard[1][4] == 2 
		&& gameBoard[2][5] == 2 && gameBoard[3][6] == 2)
			{player2Score++;}

	if(gameBoard[0][3] == 2 && gameBoard[1][2] == 2 && gameBoard[2][1] == 2 && gameBoard[3][0] == 2)
	{player2Score++;}
	if(gameBoard[0][4] == 2 && gameBoard[1][3] == 2 
		&& gameBoard[2][2] == 2 && gameBoard[3][1] == 2)
			{player2Score++;}
	if(gameBoard[1][3] == 2 && gameBoard[2][2] == 2
		&& gameBoard[3][1] == 2 && gameBoard[4][0] == 2)
			{player2Score++;}
	if(gameBoard[0][5] == 2 && gameBoard[1][4] == 2 
		&& gameBoard[2][3] == 2 && gameBoard[3][2] == 2)
			{player2Score++;}
	if(gameBoard[1][4] == 2 && gameBoard[2][3] == 2 
		&& gameBoard[3][2] == 2 && gameBoard[4][1] == 2)
			{player2Score++;}
	if(gameBoard[2][3] == 2 && gameBoard[3][2] == 2 
		&& gameBoard[4][1] == 2 && gameBoard[5][0] == 2)
			{player2Score++;}
	if(gameBoard[0][6] == 2 && gameBoard[1][5] == 2 
		&& gameBoard[2][4] == 2 && gameBoard[3][3] == 2)
			{player2Score++;}
	if(gameBoard[1][5] == 2 && gameBoard[2][4] == 2 
		&& gameBoard[3][3] == 2 && gameBoard[4][2] == 2)
			{player2Score++;}
	if(gameBoard[2][4] == 2 && gameBoard[3][3] == 2 
		&& gameBoard[4][2] == 2 && gameBoard[5][1] == 2)
			{player2Score++;}
	if(gameBoard[1][6] == 2 && gameBoard[2][5] == 2 
		&& gameBoard[3][4] == 2 && gameBoard[4][3] == 2)
			{player2Score++;}
	if(gameBoard[2][5] == 2 && gameBoard[3][4] == 2 
		&& gameBoard[4][3] == 2 && gameBoard[5][2] == 2)
			{player2Score++;}
	if(gameBoard[2][6] == 2 && gameBoard[3][5] == 2 
		&& gameBoard[4][4] == 2 && gameBoard[5][3] == 2)
			{player2Score++;}
	
	
	printf("player1Score:%d\nplayer2Score:%d\n",player1Score,player2Score);
}
