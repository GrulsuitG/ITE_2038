#ifndef __LOG_H__
#define __LOG_H__

#include <stdint.h>
#include <stdbool.h>
#include <pthread.h>
#include <stdio.h>

#include "db.h"
#define NORMAL 0
#define REDO_CRASH 1
#define UNDO_CRASH 2

#define DEFAULT_SIZE 28
#define UPDATE_SIZE 288
#define COMPENSATE_SIZE 296

#define BEGIN 0
#define UPDATE 1
#define COMMIT 2
#define ROLLBACK 3
#define COMPENSATE 4

#define LOG_BUFFER_SIZE 100
#define WINNER_SIZE 10000
#define LOSER_SIZE 10000
typedef struct log_record{
	int log_size;
	uint64_t LSN;
	uint64_t prev_LSN;
	int trx_id;
	int type;
	int table_id;
	pagenum_t pagenum;
	int offset;
	int data_length;
	char old_data[VALUE_SIZE];
	char new_data[VALUE_SIZE];
	uint64_t next_undo;

	struct log_record *next;
}log_record;


log_record *log_head;
log_record *log_tail;
int log_num;
int fd_log_path;
FILE *logmsg;
pthread_mutex_t *log_buffer_latch;

void init_log();



void recovery(int flag, int log_num, char* log_path, char* logmsg_path);

int analysis(int* loser);
void redo(int log_num, log_record** log);
void undo(int log_num, int *loser, log_record** log, int trxnum);
bool open_log(char* log_path, char* logmsg_path);

int log_write(int type, int trx_id, uint64_t prev, int table_id ,page_t* page, int index, char* value, uint64_t next);
void logbuf_flush();

#endif //__LOG_H__
