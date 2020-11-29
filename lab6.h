#ifndef _LAB6_H
#define _LAB6_H

#include "ipc.h"
#include <stdbool.h>

struct philosoph_proc
{
	int *forks;
	int *dirty;
	int *reqf;
	bool entering_cs;
};

void initialize_arrays(struct philosoph_proc *philosoph, int N, local_id local_proc_id);
void do_prints_mutexl_philosoph(int*** matrix, local_id proc_numb, int N, int log_fd,
							   struct philosoph_proc *philosoph);

#endif
