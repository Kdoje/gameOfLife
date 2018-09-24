/*
 * BoardManager.cpp
 *
 *  Created on: Sep 23, 2018
 *      Author: kl
 */

#include "BoardManager.h"

Mailbox *BoardManager::mailbox;

BoardManager::BoardManager(int rows, int cols, FILE* input, int threadNum) {
	this->threadNum=threadNum;
	threads=(pthread_t*)malloc(sizeof(pthread_t)*threadNum);
	mailbox=(Mailbox*)malloc(sizeof(Mailbox)*threadNum+1);
	boardCount=2;
	boardToPlay=0;
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
				board0[boardRow][boardCol] = ' ';
			}
			nextChar = fgetc(input);
		}
	}

	contents test;
	test.val1=0; test.val2=0;
}

void BoardManager::PrintBoard(){
	//figures out which board to print then
	//prints it out and increments the board to print
	switch (boardToPlay) {
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
			board0[i][j]='0';
			board1[i][j]='0';
		}
	}
}

int BoardManager::GetErrorCode(){
	return errorCode;
}

void BoardManager::PlayGeneration(){
	int threadError;
	for (int i = 0; i < threadNum; i++) {
		//printf("main() creating thread %d\n", i + 1);
		threadError = pthread_create(&threads[i], NULL, playRange,
				(void *)( i + 1)); //this sets the thread id's to be 1-> range
		if (threadError) {
			printf("couldn't make thread ayy lmao\n");
			exit(-1);
		}
	}

	//sends the values to add to the threads
	int final=100;
	int messagesNeeded=threadNum;
	printf("thread num %d, to range %d\n", threadNum, final);
	int prevVal = 0;
	int step = final / threadNum;
	for (int i = 1; i <= threadNum; i++) {
		contents toSend;
		toSend.val1 = prevVal + 1;
		toSend.val2 = prevVal += step;
		//printf("val1: %d, val2: %d\n", toSend.val1, toSend.val2);
		mailbox[i].SendMsg(toSend, RANGE, 0);
	}

	//set up sum
	int sum = 0;
	//check for messages
	while (messagesNeeded > 0) {
		contents msgContents;
		msgContents = mailbox[0].RecvMsg(1);
		//printf("read message\n");
		sum += msgContents.val1;
		messagesNeeded--;
	}

	//print sum
	printf("sum is %d\n", sum);
	pthread_exit(NULL);
}

void BoardManager::updatePlayBoard(){
	boardToPlay+=1;
	boardToPlay=boardToPlay%boardCount;
}

void *BoardManager::playRange(void *threadid){
	//get the thread id to an int
		long tidInit;
		tidInit = (long) threadid;
		int tid = (int) tidInit;

		//read then send the message
		contents recieved;
		recieved = BoardManager::mailbox[tid].RecvMsg(0);
		contents toSend;
		for (int i = recieved.val1; i <= recieved.val2; i++) {
			toSend.val1 += i;
		}
		printf("sending from %d\n", tid);
		BoardManager::mailbox[0].SendMsg(toSend, ALLDONE, tid);
		printf("message sent\n");
		pthread_exit(NULL);
}

BoardManager::~BoardManager() {
	for(int i=0; i<rows; i++){
		free(board0[i]);
		free(board1[i]);
	}
	free(board0);
	free(board1);
	free(threads);
	free(mailbox);
}



