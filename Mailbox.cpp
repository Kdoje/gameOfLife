/*
 * Message.cpp
 *
 *  Created on: Sep 16, 2018
 *      Author: kl
 */

#include "Mailbox.h"

bool operator ==(const contents& a, const contents& b){
	return(a.val1==b.val1)&&(a.val2==b.val2);
}

Mailbox::Mailbox() {
	sem_init(&recSem, 0 ,0);
	sem_init(&sendSem, 0 ,1);
	this->iSender=-1;
	this->type=-1;
	this->msgContents=NULL_CONTENTS;
}

//a thread can send a message from the get-go. But only
//one message can be in the mailbox at a time.
void Mailbox::SendMsg(contents msgContents, int type, int iSender){
	sem_wait(&sendSem);
	this->msgContents=msgContents;
	this->type=type;
	this->iSender=iSender;
	sem_post(&recSem);
}

//because of the way the semaphores are set up, this won't let
//the caller read until it has a valid message
contents Mailbox::RecvMsg(int iSender){
	sem_wait(&recSem);
	if(this->iSender!=EMPTY){
		retval=msgContents;
		ClearMsg();
	}
	else{
		retval=NULL_CONTENTS;
	}
	sem_post(&sendSem);
	return retval;
}

void Mailbox::ClearMsg() {
	msgContents=NULL_CONTENTS;
	this->iSender=EMPTY;
}

bool Mailbox::isEmpty(){
	return(iSender==-1);
}

Mailbox::~Mailbox() {
	sem_destroy(&sendSem);
	sem_destroy(&recSem);
}

