#include "lab1.h"
#include "lab2.h"
#include "lab4.h"
#include "ipc.h"
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include "priority_queue.h"
#include "pa2345.h"


int do_prints_mutexl(int*** matrix, local_id proc_numb, int N, int log_fd){
	Message* msg=(Message*)malloc(sizeof(Message));
	SelfStruct* selfStruct = (SelfStruct*)malloc(sizeof(SelfStruct));
	int i, j, reply_count;
	Node* queue = NULL;
	char buf[100];
	int release_count = 0;

	selfStruct->N = N;
	selfStruct->fd_matrix = matrix;
	selfStruct->src = proc_numb;
	selfStruct->log_fd = log_fd;

	i=0;
// printf("proc_numb=%i\n", proc_numb);
	// for(i=0;i<proc_numb*5;i++){
	while(i<proc_numb*5 || release_count < get_condition_value(N, proc_numb)){

		if(i<proc_numb*5){
			msg = create_message(CS_REQUEST, NULL, 0);
			push(&queue, msg->s_header.s_local_time, proc_numb);
			for(j=1;j<=N;j++){
				if(j == proc_numb){
					continue;
				}
				send(selfStruct, j, msg);
			}
		}

		// printf("proc%i send all requests[%i]\n", proc_numb, i+1);

		reply_count = 0;
		j = 1;
		while(reply_count != (N-1) || (peek(&queue)).proc_id != proc_numb){
			if(j==proc_numb){
				j=(j+1)%(N+1);
				continue;
			}
			if(receive(selfStruct, j, msg)==-1){
			// printf("proc%i reply_count=%i %i\n",proc_numb, reply_count,(peek(&queue)).proc_id);
				j=(j+1)%(N+1);
				continue;
			}
			if(msg->s_header.s_type == CS_REPLY){
				// printf("proc%i recieved reply from proc%i\n", proc_numb, j);
				reply_count++;
				j=(j+1)%(N+1);
				continue;
			}
			if(msg->s_header.s_type == CS_RELEASE){
				// printf("proc%i recieved release from proc%i\n", proc_numb, j);
				pop(&queue);
				release_count++;
				if(release_count==get_condition_value(N, proc_numb)){
					break;
				}
				j=(j+1)%(N+1);
				continue;
			}
			if(msg->s_header.s_type == CS_REQUEST){
				// printf("proc%i recieved request from proc%i\n", proc_numb, j);
				push(&queue, msg->s_header.s_local_time, j);
				msg = create_message(CS_REPLY, NULL, 0);
				send(selfStruct, j, msg);
				
				j=(j+1)%(N+1);
				continue;
			}
			if(msg->s_header.s_type == DONE){
				printf("!!!!!proc%i received DONE\n", proc_numb);
			}
			j=(j+1)%(N+1);
		}
		// printf("%s\n", "123");
		if(i<proc_numb*5){
			sprintf(buf, log_loop_operation_fmt, proc_numb, i+1, proc_numb*5);
			print(buf);
			pop(&queue);
			msg = create_message(CS_RELEASE, NULL, 0);
			for(j=1;j<=N;j++){
				if(j == proc_numb){
					continue;
				}
				send(selfStruct, j, msg);
			}
			i++;
		}
	}
	return 0;
}

int do_prints(local_id proc_numb, int N){
	int i;
	char buf[100];

	for(i = 0;i<proc_numb*5;i++){
		sprintf(buf, log_loop_operation_fmt, proc_numb, i+1, proc_numb*5);
		print(buf);
	}
	return 0;
}

int get_condition_value(int N, local_id proc_id){
	int i = 0;
	int s = 0;
	for(i=1;i<=N;i++){
		s+=i;
	}
	s-=proc_id;
	return s*5;
}
