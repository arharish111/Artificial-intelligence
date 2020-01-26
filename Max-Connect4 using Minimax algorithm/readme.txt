Name: Harish Harish
--------------------------------------------
Programming language:C
----------------------------------------------
Compile : gcc -o maxconnect4 maxconnect4.c
Run: maxconnect4 interactive [input_file] [computer-next/human-next] [depth] OR
	 maxconnect4 one-move [input_file] [output_file] [depth]
----------------------------------------------------------------------------------------	 
Code Structure:

void printCurrentStateAndScore(int **);

This function prints the current state and score.
-----------------------------------------------------------------

void printGameBoardToFile(int **,char *,int);

This function takes game board, file name and current player value as parameters and then prints the content of
game board(2D matrix) and current player to the file name mentioned.
-----------------------------------------------------------------------------------------------------------

int computerPlays(int **);

This function is called when the next move has to be made by the computer. This function prints the current board state and score. 
If the board is full, then exit. If not chooseNextMove function is called to identify the column to place the piece and after
that humanPlays function is called or returned to main.
-----------------------------------------------------------------------------------------------------------------------

int humanPlays(int**);

This function is called when the next move has to be made by human/user. This function prints the current board state and score. 
If the board is full, then exit.If not, a valid move is accepted from the user and then computerPlays function is called.
-----------------------------------------------------------------------------------------------------------------------------

int chooseNextMove(int **);

This function is the driven fuction for depth - limited alpha,beta pruning minimax search.A next valid move is made by calling 
getSucessor function and then minValue function is called.
-----------------------------------------------------------------------------------------------------------------------

int maxValue(int **,int,int);

This function checks whether game has reached terminal or depth limit, if so eval value of the game board is returned.If not,
a valid next move is made and then minValue function is called.
--------------------------------------------------------------------------------------------------------------------------------

int minValue(int **,int,int);

This function checks whether game has reached terminal or depth limit, if so eval value of the game board is returned.If not,
a valid next move is made and then maxValue function is called.
---------------------------------------------------------------------------------------------------------------------------------

int getSucessor(int **,int,int);

This function gives the next valid move.
----------------------------------------------------------------------------------------------------------------------------------

int getEval(int **);

This function returns the eval value of the game board passed as parameter.
---------------------------------------------------------------------------------------------------------------------------

void freeGameBoard(int **);

This is a helper function to free allocated game boards.
-------------------------------------------------------------------------------------------------------------

int getNumberOfPieces(int **);

This function retuns number of pieces present on the game board which is passed as a parameter.