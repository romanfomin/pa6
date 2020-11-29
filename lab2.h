#ifndef _LAB2_H
#define _LAB2_H

#include "banking.h"
#include "ipc.h"

timestamp_t global_time;

int do_transfers(int*** matrix, local_id proc_numb, int N, int log_fd, balance_t* balance, BalanceHistory* balanceHistory);
int send_history(int*** matrix, local_id proc_numb, int N, int log_fd, BalanceHistory* balanceHistory);
AllHistory* receive_and_print_all_history(int*** matrix, local_id proc_numb, int N, int log_fd);
void complete_history(BalanceHistory* balanceHistory);
void increment_time();
void set_time(timestamp_t msg_time);

#endif
