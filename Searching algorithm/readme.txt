Name: Harish Harish

PROGRAMMING LANGUAGE : C

INSTRUCTIONS TO RUN THE PROGRAM:

1.Compile: gcc -o find_route search_main.c find_route.c arrays.c

2.Run: find_route input.txt Bremen Kassel

STRUCTURE OF THE CODE:

1.search_main.c : This file consists of the "main" function, which is also the driver function. 
                  Main function takes input from command line and parses it and then makes
				  necessary function calls to find the optimal path between start and final.
				 
2.find_route.c : This file consists of functions which implements state space search and also 
				 helper functions for state space search.
				 
3.arrays.c : Consists of helper functions to initialize arrays.

4.find_route.h : header file for find_route.c

5.arrays.h : header file for arrays.c