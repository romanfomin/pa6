#ifndef _LAB4_H
#define _LAB4_H

#include "ipc.h"

int do_prints_mutexl(int*** matrix, local_id proc_numb, int N, int log_fd);
int do_prints(local_id proc_numb, int N);
int get_condition_value(int N, local_id proc_id);

#endif
