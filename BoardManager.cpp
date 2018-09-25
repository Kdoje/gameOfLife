/*
 * BoardManager.cpp
 *
 *  Created on: Sep 23, 2018
 *      Author: kl
 */

#include "BoardManager.h"


BoardManager::BoardManager(int rows, int cols, FILE* input) {
	boardCount=2;
	curBoard=0;
	nextBoard=1;
	char nextChar= fgetc(input);
	int fileRows=0, fileCols=0;
	board0=(char**)malloc(sizeof(char*)*rows);
	board1=(char**)malloc(sizeof(char*)*rows);
	for(int i=0; i<rows; i++){
		board0[i]=(char*)malloc(sizeof(char)*cols);
		board1[i]=(char*)malloc(sizeof(char)*cols);
	}
	this->rows=rows;
	this->cols=cols;

	//Gets the number of rows and columns from the file
	int curCol = 0;
	while (nextChar != EOF) {
		if (nextChar == '\n') {
			fileRows++;
			curCol = 0;
		} else if (nextChar != ' ') {
			curCol++;
			if (curCol > fileCols) {
				fileCols = curCol;
			}
		}
		nextChar = fgetc(input);
	}
	//Determine whether or not to return an error code
	if(fileCols>cols||fileRows>rows){
		errorCode=FILE_ERROR;
	}
	else{
		//say there was no error and place the file in board1
		errorCode=NO_ERROR;
		int startRow=(rows-fileRows)/2;
		int startCol=(cols-fileCols)/2;
		rewind(input);
		//clear the boards
		this->ClearBoards();
		//read through the file again and place it where needed
		nextChar = fgetc(input);
		int boardCol=startCol;
		int boardRow=startRow;
		while (nextChar != EOF) {
			if (nextChar == '\n') {
				boardCol=startCol;
				boardRow++;
			} else if (nextChar != ' ') {
				board0[boardRow][boardCol] = nextChar;
				boardCol++;
			} else {
				board0[boardRow][boardCol] = OPEN;
			}
			nextChar = fgetc(input);
		}
	}
}

void BoardManager::PrintBoard(){
	//figures out which board to print then
	//prints it out and increments the board to print
	updatePlayBoard();
	switch (curBoard) {
	case 0:
		for (int i = 0; i < rows; i++) {
			for (int j = 0; j < cols; j++) {
				printf("%c ", board0[i][j]);
			}
			printf("\n");
		}
		break;
	case 1:
		for (int i = 0; i < rows; i++) {
			for (int j = 0; j < cols; j++) {
				printf("%c ", board1[i][j]);
			}
			printf("\n");
		}
		break;
	}
}

void BoardManager::ClearBoards(){
	for(int i=0; i<rows; i++){
		for(int j=0; j<cols; j++){
			board0[i][j]=OPEN;
			board1[i][j]=OPEN;
		}
	}
}

int BoardManager::GetErrorCode(){
	return errorCode;
}

void BoardManager::updatePlayBoard(){
	curBoard=nextBoard;
	nextBoard+=1;
	nextBoard=nextBoard%boardCount;
}

void BoardManager::PlayRange(int start, int end){
	//TODO get this play stuff to work (possibly need function
	//to get active board
	char **curBoard=getCurBoard();
	char **nextBoard=getNextBoard();
	for(int row=start; row<end; row++){
		for(int col=0; col<cols; col++){
			int neighbors=getNumNeighbors(row, col);
			if(curBoard[row][col]==CELL){
				if(neighbors<=1||neighbors>=4){
					nextBoard[row][col]=OPEN;
				}
				else{
					nextBoard[row][col]=CELL;
				}
			}
			else{
				if(neighbors==3){
					nextBoard[row][col]=CELL;
				}
			}
		}
	}
}
int BoardManager::getNumNeighbors(int r, int c){
	//set up search bounds
	char **board=getCurBoard();
	int startRow, endRow, startCol, endCol;
	int numNeighbors=0;
	startRow=(r==0)?0:(r-1);
	endRow=((r+1)>=rows)?(rows-1):r+1;
	startCol=(c==0)?0:(c-1);
	endCol=((c+1)>=cols)?(cols-1):c+1;
	for(int i= startRow; i<=endRow; i++){
		for(int j=startCol; j<=endCol; j++){
			if(board[i][j]==CELL){
				numNeighbors++;
			}
		}
	}
	if(board[r][c]==CELL){
		numNeighbors--;//this will accounts for the fact the cell calling
						//is included in numNeighbors
	}
	if(numNeighbors>0){
		printf("neighbors %d\n", numNeighbors);
	}
	return numNeighbors;
}
//returns the board of the current generation
char **BoardManager::getCurBoard(){
	if(curBoard==1){
		return board1;
	}
	return board0;
}

char **BoardManager::getNextBoard() {
	if (nextBoard == 1) {
		return board1;
	}
	return board0;
}

BoardManager::~BoardManager() {
	for(int i=0; i<rows; i++){
		free(board0[i]);
		free(board1[i]);
	}
	free(board0);
	free(board1);
}



