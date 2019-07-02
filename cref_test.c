#include "cref.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct {
	struct ref refcount;
	char data[100];
} message_t;

typedef struct {
	pthread_t thread;
	unsigned numthread;
	unsigned nbmsgs;
	message_t **msgs;
} thread_params_t;

void* routine(void* arg) {
	thread_params_t* params = (thread_params_t*) arg;
	for(unsigned i=0; i<params->nbmsgs; i++) {
		ref_dec(&params->msgs[i]->refcount);
	}
	return 0;
}

static void message_free(const struct ref *ref) {
	message_t *msg = (message_t*) ref;
	printf("delete %s\n",msg->data);
	free(msg);
}

int test(unsigned nbmsgs,unsigned nbthreads) {
	message_t **msgs = malloc(sizeof(message_t)*nbmsgs);
	for(unsigned i=0; i<nbmsgs; i++) {
		msgs[i] = malloc(sizeof(message_t));
		msgs[i]->refcount.count = 1;
		msgs[i]->refcount.free = message_free;
		snprintf(msgs[i]->data,sizeof(msgs[i]->data),"MSG%d",i);
	}
	thread_params_t* params = malloc(sizeof(thread_params_t)*nbthreads);
	for(unsigned i=0; i<nbthreads; i++) {
		params[i].numthread = i;
		params[i].nbmsgs = nbmsgs;
		params[i].msgs = msgs;
		for(unsigned j=0; j<nbmsgs; j++) {
			ref_inc(&msgs[j]->refcount);
		}
		pthread_create(&params[i].thread,0,routine,&params[i]);
	}
	for(unsigned j=0; j<nbmsgs; j++) {
		ref_dec(&msgs[j]->refcount);
	}
	for(unsigned i=0; i<nbthreads; i++) {
		void* retval;
		pthread_join(params[i].thread,&retval);
	}
	free(msgs);
	free(params);
}

int main(int argc,char** argv) {
	unsigned nbmsgs = 1;
	unsigned nbthreads = 1;
	if(argc>2) {
		nbmsgs = atoi(argv[1]);
		nbthreads = atoi(argv[2]);
		return test(nbmsgs,nbthreads);
	}
	return 0;
}
