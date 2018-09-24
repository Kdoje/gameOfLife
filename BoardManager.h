/*
 * BoardManager.h
 *
 *  Created on: Sep 23, 2018
 *      Author: kl
 */

#ifndef BOARDMANAGER_H_
#define BOARDMANAGER_H_
#include<stdio.h>
#include<stdlib.h>
#include <pthread.h>
#include "Mailbox.h"

#define FILE_ERROR -1
#define NO_ERROR 0

class BoardManager {;
	char **board0;
	char **board1;
	int rows, cols;
	int errorCode;
	int boardCount, boardToPlay;
private:
	void updatePlayBoard();
public:
	BoardManager(int rows, int cols, FILE *input);
	int GetErrorCode();
	void PrintBoard();
	void ClearBoards();
	void PlayRange(int start, int end);
	virtual ~BoardManager();
};

#endif /* BOARDMANAGER_H_ */
