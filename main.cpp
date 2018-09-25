/*
 * main.cpp
 *
 *  Created on: Sep 23, 2018
 *      Author: kl
 */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "BoardManager.h"
#include "main.h"
const int MAXGRID=10;

BoardManager *manager;
const int MAX_THREADS=10;
Mailbox mailbox[MAX_THREADS+1];

void *FindSum(void *threadid) {

	//get the thread id to an int
	long tidInit;
	tidInit = (long) threadid;
	int tid = (int) tidInit;

	//read then send the message
	contents recieved;
	recieved.type=RANGE;
	while (recieved.type != ALLDONE) {
		recieved = mailbox[tid].RecvMsg(0);
		printf("playing from %d to %d\n", recieved.val1, recieved.val2);
		manager->PlayRange(recieved.val1, recieved.val2);
		contents toSend;
		toSend.iSender = tid;
		toSend.type = NOTDONE;
		toSend.val1 = tid;
		//printf("sending from %d\n", tid);
		mailbox[0].SendMsg(toSend);
		//printf("message sent from %d\n", tid);
	}
	printf("thread %d is done\n", tid);
	pthread_exit(NULL);
}

int main(int argc, char* argv[]){
	if(argc>=3&&argc<=6){
		//sets up the file and parameters for boardManager
		FILE *inputBoard;
		inputBoard = fopen(argv[2], "r");
		int genCount=atoi(argv[3]);
		int threadCount=atoi(argv[1]);
		bool printEachGen=false;
		bool waitForInput = false;
		if (argc== 5) {
			printEachGen=(argv[4][0]=='y');
		} else if (argc == 6) {
			waitForInput=(argv[4][0]=='y');
			printEachGen=(argv[5][0]=='y');
		}

		//checks that the board manager can be created
		manager = new BoardManager(MAXGRID, MAXGRID, inputBoard);
		if (!manager->GetErrorCode()) {
			printf("good to go\n");
		} else {
			printf("the file exceeds the grid size\n");
			return -1;
		}

		//gets the threads set up
		int threadError;
		pthread_t threads[threadCount];
		printf("threads: %d, Print gens: %d, Wait for input: %d\n",threadCount, printEachGen,
				waitForInput);
		for (int i = 0; i < threadCount; i++) {
			//printf("main() creating thread %d\n", i + 1);
			threadError = pthread_create(&threads[i], NULL, FindSum,
					(void *) i + 1); //this sets the thread id's to be 1-> range
			if (threadError) {
				printf("couldn't make thread\n");
				exit(-1);
			}
		}

		//reads the messages
		int messagesNeeded=threadCount;
		contents toSend;
		bool gensDone=false;

		//play through each of the generations
		//(or until a finish condition has been fired)
		for (int gen = 0; (gen < genCount && !gensDone); gen++) {
			//figure out the range to write for here
			float stepFloat=(float)MAXGRID/threadCount;
			//figure out what the step needs to be and
			//write it to the thread that needs to play that sector
			int step=(int)(roundf(stepFloat));
			int start=0;
			int final=step;
			for (int thread = 1; thread <= threadCount-1; thread++) {
				//goes from i=start, i<final
				toSend.val1 = start;
				toSend.val2 = final;
				toSend.type = RANGE;
				toSend.iSender = 0;
				//printf("val1: %d, val2: %d\n", toSend.val1, toSend.val2);
				mailbox[thread].SendMsg(toSend);
				start=final;
				final+=step;
			}
			//have the last thread pick up the difference in case
			//the number of assigned columns is different
			toSend.val1 = start;
			toSend.val2 = MAXGRID;
			toSend.type = RANGE;
			toSend.iSender = 0;
			mailbox[threadCount].SendMsg(toSend);
			gensDone=waitForMessages(messagesNeeded);
			manager->PrintBoard();
			printf("--------GEN %d---------\n", gen);
		}

		//tell the threads to finish and wait for them to close up
		for (int i = 1; i <= threadCount; i++) {
			toSend.val1 = i;
			toSend.val2 = 5;
			toSend.type = ALLDONE;
			toSend.iSender = 0;
			//printf("val1: %d, val2: %d\n", toSend.val1, toSend.val2);
			mailbox[i].SendMsg(toSend);
		}
		waitForMessages(messagesNeeded);
		printf("board finished with gensDone: %d \n", (int)gensDone);
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

bool waitForMessages(int messagesNeeded){
	int messages=messagesNeeded;
	bool isDone=true;
	while (messages > 0) {
		contents msgContents;
		msgContents = mailbox[0].RecvMsg(1);
		//if at least one thread isn't done then make isDone false
		isDone=!(msgContents.type==NOTDONE);
		printf("	read message from %d\n", msgContents.iSender);
		messages--;
	}
	return isDone;
}

void SendMessages(){

}

