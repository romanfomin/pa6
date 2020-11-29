#include "lab1.h"
#include "lab2.h"
#include "lab4.h"
#include "lab6.h"
#include "ipc.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <errno.h>
#include "priority_queue.h"
#include "pa2345.h"

void initialize_arrays(struct philosoph_proc *philosoph, int N, local_id local_proc_id)
{
	int i;
	local_proc_id--;

	for (i = 0; i < N; i++) {
		if (i == local_proc_id) {
			philosoph->forks[i] = -1;
			philosoph->dirty[i] = -1;
			philosoph->reqf[i] = -1;
			continue;
		}

		if (i < local_proc_id) {
			philosoph->forks[i] = 0;
			philosoph->dirty[i] = 0;
			philosoph->reqf[i] = 1;
			continue;
		}

		if (i > local_proc_id) {
			philosoph->forks[i] = 1;
			philosoph->dirty[i] = 1;
			philosoph->reqf[i] = 0;
			continue;
		}
	}
}

void print_philosoph(SelfStruct *self, struct philosoph_proc *philosoph)
{
	int i;

	printf("proc %d forks=[", self->src);
	for (i = 0; i < self->N; i++) {
		printf("%d ", philosoph->forks[i]);
	}
	printf("]\n");

	printf("proc %d dirty=[", self->src);
	for (i = 0; i < self->N; i++) {
		printf("%d ", philosoph->dirty[i]);
	}
	printf("]\n");

	printf("proc %d reqf=[", self->src);
	for (i = 0; i < self->N; i++) {
		printf("%d ", philosoph->reqf[i]);
	}
	printf("]\n");
}

bool has_all_clear_forks(struct philosoph_proc *philosoph, int N)
{
	int i;
	for (i = 0; i < N; i++) {
		if (philosoph->forks[i] == 0 || philosoph->dirty[i] == 1)
			return false;
	}

	return true;
}

void make_forks_dirty(SelfStruct *self, struct philosoph_proc *philosoph)
{
	int i;
	for (i = 0; i < self->N; i++) {
		if ((i + 1) == self->src)
			continue;
		philosoph->dirty[i] = 1;
	}
}

bool has_all_forks_and_no_reqf(SelfStruct *self, struct philosoph_proc *philosoph)
{
	int i;
	for (i = 0; i < self->N; i++) {
		if (philosoph->forks[i] == 0 || philosoph->reqf[i] == 1)
			return false;
	}

	return true;
}

bool request_cs_philosoph(SelfStruct *self, struct philosoph_proc *philosoph)
{
	int i;
	Message *msg;

	/* check if can reenter cs without requests */
	if (has_all_forks_and_no_reqf(self, philosoph)) {
		// printf("proc %d can reenter\n", self->src);
		return true;
	}


	for (i = 0; i < self->N; i++) {
		msg = create_message(CS_REQUEST, NULL, 0);
		if (philosoph->reqf[i] == 1) {
			send(self, (i + 1), msg);
			// printf("proc %d send REQUEST to %d\n", self->src, i + 1);
			philosoph->reqf[i] = 0;
		}
	}
	philosoph->entering_cs = true;

	return false;
}

bool can_enter_cs(SelfStruct *self, struct philosoph_proc *philosoph)
{
	int i;
	for (i = 0; i < self->N; i++) {
		if (philosoph->forks[i] == 0)
			return false;
		if (philosoph->dirty[i] == 1 && philosoph->reqf[i] == 1)
			return false;
	}

	return true;
}

void process_msg_philosoph(SelfStruct *self, struct philosoph_proc *philosoph, bool once)
{
	int i = 0;
	int N = self->N;

	while(1) {
		for (i = 0; i < N; i++) {
			Message msg;

			if ((i + 1) == self->src)
				continue;

			if (receive(self, (i + 1), &msg) == -1)
				continue;

			if (msg.s_header.s_type == CS_REPLY) {
				// printf("proc %d recv REPLY from %d\n", self->src, i + 1);
				philosoph->forks[i] = 1;
				philosoph->dirty[i] = 0;

				if (can_enter_cs(self, philosoph)) {
					make_forks_dirty(self, philosoph);
					philosoph->entering_cs = false;
					return;
				}
				continue;
			}

			if (msg.s_header.s_type == CS_REQUEST) {
				philosoph->reqf[i] = 1;
				// printf("proc %d recv REQUEST from %d\n", self->src, i + 1);

				if (philosoph->dirty[i] == 1) {
					Message *repl_msg = create_message(CS_REPLY, NULL, 0);
					send(self, (i + 1), repl_msg);
					// printf("proc %d send REPLY to %d\n", self->src, i + 1);
					philosoph->dirty[i] = 0;
					philosoph->forks[i] = 0;

					if (philosoph->entering_cs) {
						Message *req_msg = create_message(CS_REQUEST, NULL, 0);
						send(self, (i + 1), req_msg);
						philosoph->reqf[i] = 0;
						// printf("proc %d send REQUEST to %d\n", self->src, i + 1);
					}
				}
				continue;
			}
		}

		if (once)
			return;
	}
}

void release_cs_philosoph(SelfStruct *self, struct philosoph_proc *philosoph)
{
	int i;
	for (i = 0; i < self->N; i++) {
		if (philosoph->reqf[i] == 1) {
			Message *msg = create_message(CS_REPLY, NULL, 0);
			send(self, (i + 1), msg);
			// printf("proc %d send REPLY to %d\n", self->src, i + 1);
			philosoph->forks[i] = 0;
			philosoph->dirty[i] = 0;
		}
	}
}

void do_cs(SelfStruct *self, int i)
{
	char buf[1000];

	sprintf(buf, log_loop_operation_fmt, self->src, i + 1, self->src * 5);
	print(buf);
}

void do_prints_mutexl_philosoph(int*** matrix, local_id proc_numb, int N, int log_fd,
							    struct philosoph_proc *philosoph)
{
	SelfStruct self;
	int i;
	bool can_reenter = false;

	self.N = N;
	self.fd_matrix = matrix;
	self.src = proc_numb;
	self.log_fd = log_fd;

	for (i = 0; i < proc_numb * 5; i++) {
		// printf("=========proc %d [request_cs]\n", proc_numb);
		can_reenter = request_cs_philosoph(&self, philosoph);
		if (!can_reenter) {
			// printf("=========proc %d [process_msg]\n", proc_numb);
			process_msg_philosoph(&self, philosoph, false);
		}
		do_cs(&self, i);
		// printf("=========proc %d [release_cs]\n", proc_numb);
		release_cs_philosoph(&self, philosoph);
		// printf("=========proc %d [process_msg after cs]\n", proc_numb);
		process_msg_philosoph(&self, philosoph, true);
	}
}
