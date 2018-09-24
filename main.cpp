/*
 * main.cpp
 *
 *  Created on: Sep 23, 2018
 *      Author: kl
 */
#include <stdio.h>
#include <stdlib.h>
#include "BoardManager.h"
const int MAXGRID=10;

BoardManager *manager;

int main(int argc, char* argv[]){
	if(argc>=3&&argc<=6){
		//sets up the file and parameters for boardManager
		FILE *inputBoard;
		inputBoard = fopen(argv[2], "r");

//		int fileRows=0;
//		int fileCols=0;
		int threadCount=atoi(argv[1])-1;
		bool printEachGen=false;
		bool waitForInput=false;

		manager = new BoardManager(MAXGRID, MAXGRID, inputBoard, 5);
		if(!manager->GetErrorCode()){
			manager->PrintBoard();
		}
		else{
			printf("the file exceeds the grid size\n");
			return -1;
		}
		printf("Print gens: %d, Wait for input: %d\n", printEachGen, waitForInput);
		manager->PlayGeneration();
	}
	else{
		printf("invalid argument count\n");
	}
	printf("ending program\n");
	return 0;
	//how to make this all work:
	/*
	 * read in the command line arguments and figure out when to pause and such
	 * create the right number of threads using the math.round, then having the last thread be the difference remaining
	 * Have thread 0 populate the board from the text file
	 * tell the threads to start with a starting and ending row to use
	 * Move the new generation into the board
	 * Once this is finished send all done and say whether there was a change or not
	 * If there was no change finish otherwise repeat
	 */
}



