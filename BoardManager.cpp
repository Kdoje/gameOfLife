/*
 * BoardManager.cpp
 *
 *  Created on: Sep 23, 2018
 *      Author: kl
 */

#include "BoardManager.h"

BoardManager::BoardManager(int rows, int cols, FILE* input) {
	boardCount=2;
	boardToPrint=0;
	char nextChar= fgetc(input);
	int fileRows, fileCols;
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
	fileCols++;
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
				board0[boardRow][boardCol] = ' ';
			}
			nextChar = fgetc(input);
		}
	}

}
void BoardManager::PrintBoard(){
	//figures out which board to print then
	//prints it out and increments the board to print
	switch (boardToPrint) {
	case 0:
		for (int i = 0; i < rows; i++) {
			for (int j = 0; j < cols; j++) {
				printf("%c", board0[i][j]);
			}
			printf("\n");
		}
		break;
	case 1:
		for (int i = 0; i < rows; i++) {
			for (int j = 0; j < cols; j++) {
				printf("%c", board1[i][j]);
			}
			printf("\n");
		}
		break;
	}
	boardToPrint+=1;
	boardToPrint=boardToPrint%boardCount;
}
void BoardManager::ClearBoards(){
	for(int i=0; i<rows; i++){
		for(int j=0; j<cols; j++){
			board0[i][j]=' ';
			board1[i][j]=' ';
		}
	}
}
int BoardManager::GetErrorCode(){
	return errorCode;
}
BoardManager::~BoardManager() {
	for(int i=0; i<rows; i++){
		free(board0[i]);
		free(board1[i]);
	}
	free(board0);
	free(board1);
}



