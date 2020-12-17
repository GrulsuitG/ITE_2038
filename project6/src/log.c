#include "log.h"

void init_log(){
	
	log_buffer_latch = (pthread_mutex_t*)malloc(sizeof(pthread_mutex_t));
	pthread_mutex_init(log_buffer_latch, 0);
	log_record* r = (log_record*) malloc(sizeof(log_record));
	r->log_size = 0;
	r->LSN =0;
	log_tail =r;
	log_head =r;
}

void recovery(int flag, int log_num, char* log_path, char* logmsg_path){
	int *loser = (int*)malloc(sizeof(int) * LOSER_SIZE);
	int i, trxnum;
	log_record **log;
	if(open_log(log_path, logmsg_path)){
		trxnum = analysis(loser);
		log = (log_record**)malloc(sizeof(log_record*)*(trxnum+1));
		if(flag == NORMAL){
			redo(0, log);
			undo(0, loser, log, trxnum);
			lseek(fd_log_path,0, SEEK_SET);
			truncate(log_path, 0);
			
		}
		if(flag == REDO_CRASH){
			redo(log_num, log);
			logbuf_flush();
			for(i=1;i<=10; i++){
				close_table(i);
			}			
		}
		else if(flag == UNDO_CRASH){
			redo(0, log);
			undo(log_num, loser, log, trxnum);
			logbuf_flush();
			for(i=1;i<=10; i++){
				close_table(i);
			}
		}
		
		free(log);
	}
	free(loser);
	
}

int analysis(int* loser){
	int *winner = (int*)malloc(sizeof(int) * WINNER_SIZE);
	int winnum =0, losenum =0, tempnum=0;
	int winnersize, losersize;
	int size, type, trx_id;
	int i,j, flag;
	int *temp;
	winnersize= 1;
	losersize =1;
	fprintf(logmsg,"[ANALYSIS] Analysis pass start\n");
	
	lseek(fd_log_path, 0, SEEK_SET);
	temp = (int*)malloc(sizeof(int)*winnersize*WINNER_SIZE); 
	while(read(fd_log_path, &size, sizeof(int)) > 0){
		lseek(fd_log_path, 16, SEEK_CUR);
		read(fd_log_path, &trx_id, sizeof(int));
		read(fd_log_path, &type, sizeof(int));
		if(type == BEGIN){
			winner[winnum++] = trx_id;
			if(winnum ==(winnersize*WINNER_SIZE)){
				winnersize++;
				winner = (int*)realloc(winner, (winnersize*WINNER_SIZE)*sizeof(int));
				losersize++;
				temp = (int*)realloc(temp, (losersize*LOSER_SIZE)*sizeof(int));
			}
		}
		else if (type == COMMIT || type == ROLLBACK){
			temp[tempnum++] = trx_id;
			if(tempnum == (losersize*LOSER_SIZE)){
				winnersize++;
				winner = (int*)realloc(winner, (winnersize*WINNER_SIZE)*sizeof(int));
				losersize++;
				temp = (int*)realloc(temp, (losersize*LOSER_SIZE)*sizeof(int));
			}
		}
		lseek(fd_log_path, size-DEFAULT_SIZE, SEEK_CUR);
	}
	
	for(i=0; i<winnum; i++){
		flag =0;
		for(j=0; j<tempnum; j++){
			if(winner[i] == temp[j]){
				flag = 1;
				break;
			}
		}
		if(flag ==0){
			loser[losenum++] = winner[i];
			winner[i] = 0;
		}
	}
	fprintf(logmsg, "[ANALYSIS] Analysis success. Winner: ");
	for(i=0; i<winnersize*WINNER_SIZE; i++){
		if(winner[i] !=0){
			fprintf(logmsg, "%d ", winner[i]);
		}
	}
	fprintf(logmsg, "Loser: ");
	for(i=0; i<losersize*LOSER_SIZE; i++){
		if(loser[i] != 0){
			fprintf(logmsg, "%d ", loser[i]);
		}
	}
	fprintf(logmsg,"\n");
	free(winner);
	free(temp);
	return winnum;
}

void redo(int log_num, log_record** log){
	int type, trx_id, size, redo_num;
	int table_id, offset, index;
	uint64_t LSN, prev_LSN,next_undo;
	pagenum_t pagenum;
	page_t *page;
	char new_data[VALUE_SIZE];
	char old_data[VALUE_SIZE];
	log_record* temp;
	char *str = (char*) malloc(sizeof(char) * 10);
	redo_num =1;
	fprintf(logmsg,"[REDO] Redo pass start\n");
	
	lseek(fd_log_path, 0, SEEK_SET);
	
	while(read(fd_log_path, &size, sizeof(int)) > 0){
		read(fd_log_path, &LSN, sizeof(uint64_t));
		read(fd_log_path, &prev_LSN, sizeof(uint64_t));
		read(fd_log_path, &trx_id, sizeof(int));
		read(fd_log_path, &type, sizeof(int));
		if(type == BEGIN){
			fprintf(logmsg,"LSN %lu [BEGIN] Transaction id %d\n", LSN+size, trx_id);
			
		}
		else if (type == COMMIT){
			fprintf(logmsg,"LSN %lu [COMMIT] Transaction id %d\n", LSN+size, trx_id);
			
		}
		else if (type == UPDATE || type == COMPENSATE){
			read(fd_log_path, &table_id, sizeof(int));
			read(fd_log_path, &pagenum, sizeof(pagenum_t));
			read(fd_log_path, &offset, sizeof(int));
			lseek(fd_log_path,sizeof(int),SEEK_CUR);
			read(fd_log_path, old_data, VALUE_SIZE);
			read(fd_log_path, new_data, VALUE_SIZE);
			if(type == COMPENSATE)
				read(fd_log_path, &next_undo, sizeof(uint64_t));
			
			sprintf(str, "DATA%d",table_id);
			open_table(str);
			page= buf_read_page(table_id, pagenum);
			
			if(page->LSN >= LSN){
				buf_return_page(table_id, pagenum, false, page->index);	
				fprintf(logmsg, "LSN %lu [CONSIDER_REDO] Transaction id %d\n", LSN+size, trx_id);
			}
			else{
				temp = (log_record*)malloc(sizeof(log_record));
				if(type == UPDATE)
					temp->log_size = UPDATE_SIZE;
				else if(type == COMPENSATE)
					temp->log_size = COMPENSATE_SIZE;
				temp->LSN = LSN;
				temp->prev_LSN = prev_LSN;
				temp->trx_id = trx_id;
				temp->type = UPDATE;
				temp->table_id = table_id;
				temp->pagenum = pagenum;
				temp->offset = offset;
				temp->data_length = VALUE_SIZE;
				strncpy(temp->old_data, old_data, VALUE_SIZE);
				strncpy(temp->new_data, new_data, VALUE_SIZE);
				if(type == COMPENSATE)
					temp->next_undo = next_undo;
					
				temp->next = log[trx_id];
				log[trx_id] = temp;
				
				index = (offset % PAGE_SIZE) / 128;
				strncpy(page->record[index-1]->value, new_data, VALUE_SIZE);
				page->LSN = LSN;
				buf_return_page(table_id, pagenum, true, page->index);
				if(type == UPDATE)
					fprintf(logmsg, "LSN %lu [UPDATE] Transaction id %d redo apply\n", LSN+size, trx_id);
				else if(type == COMPENSATE)
					fprintf(logmsg, "LSN %lu [CLR] next undo lsn %lu\n", LSN+size, next_undo);
			}
		}
		else if (type == ROLLBACK){
			fprintf(logmsg, "LSN %lu [ROLLBACK] Transaction id %d\n", LSN+size, trx_id);
		}
		
		
		if(redo_num++ == log_num){
			return;
		}
	}
	log_head->LSN = LSN;
	log_head->log_size = size;
	fprintf(logmsg, "[REDO] Redo pass end\n");
}

void undo(int log_num, int *loser, log_record** log, int trxnum){
	int trx,i, index,num =1;
	log_record* cur;
	page_t* page;
	uint64_t LSN;
	fprintf(logmsg, "[UNDO] Undo pass start\n");
	for(i=trxnum; i>0; i--){
		if(loser[i] != 0){
			trx = loser[i];
			cur = log[trx];
			while(cur){
				if(cur->type == UPDATE){
					page= buf_read_page(cur->table_id, cur->pagenum);
					index = (cur->offset % PAGE_SIZE) / 128;
					log_write(COMPENSATE, cur->trx_id, cur->LSN, cur->table_id, page,index-1, cur->new_data, cur->prev_LSN);
					strncpy(page->record[index-1]->value, cur->old_data, VALUE_SIZE);
					buf_return_page(cur->table_id, cur->pagenum, true, page->index);
					fprintf(logmsg, "LSN %lu [UPDATE] Transaction id %d undo apply\n", cur->LSN+cur->log_size, cur->trx_id); 
					cur = cur->next;
				}
				else if(cur->type == COMPENSATE){
					LSN = cur->next_undo;
					while(cur && cur->LSN == LSN)
						cur = cur->next;
				}
				else{
					cur = cur->next;
				}
				if(log_num == num++)
					return;
			}
		}
	}
	fprintf(logmsg, "[UNDO] undo pass end\n");
}

bool open_log(char* log_path, char* logmsg_path){
	int isExist;
	isExist = access(log_path, 00);
	//log file 이 존재 안하면 새로 만들어 주고
	if(isExist == -1){
		if((fd_log_path = open(log_path,O_RDWR|O_CREAT,0644)) < 0){
			perror("make log path error");
			exit(EXIT_FAILURE);
		} 
		return false;
	}
	else{
		if((fd_log_path = open(log_path,O_RDWR | O_SYNC)) < 0){
			perror("open log path error");
			exit(EXIT_FAILURE);
		}
		if(read(fd_log_path,&isExist, sizeof(int))<=0){
			return false;
		}
		logmsg = fopen(logmsg_path, "w");
		return true;
	}
}

int log_write(int type, int trx_id, uint64_t prev, int table_id ,page_t* page, int index, char* value, uint64_t next){
	log_record* r = (log_record*)malloc(sizeof(log_record));
	log_record* temp;
	r->next = NULL;
	int offset;
		
	r->LSN = log_tail->LSN + log_tail->log_size;	
	r->prev_LSN = prev;
	r->trx_id = trx_id;
	r->type = type;
	
	if(type == BEGIN || type == COMMIT || type == ROLLBACK)
		r->log_size = DEFAULT_SIZE;
	else if (type == UPDATE || type == COMPENSATE){
		r->table_id = table_id;
		r->pagenum = page->mypage;
		offset = page->mypage*PAGE_SIZE + 128*(index+1);
		r->offset = offset;
		r->data_length = VALUE_SIZE;
		
		if(type == UPDATE){
			r->log_size = UPDATE_SIZE;
			strncpy(r->old_data, page->record[index]->value, VALUE_SIZE);
			strncpy(r->new_data, value, VALUE_SIZE);
		}
 		else if(type == COMPENSATE){
 			r->log_size = COMPENSATE_SIZE;	 
			strncpy(r->old_data, value, VALUE_SIZE);
			strncpy(r->new_data, page->record[index]->value, VALUE_SIZE);
			r->next_undo = next;
		}
	}

	log_tail->next = r;
	log_tail = r;
	return r->LSN;
}

void logbuf_flush(){
	int type;
	log_record* r = log_head->next;
	log_record* temp;
	
	while(r != NULL){
		type = r->type;
		write(fd_log_path, &r->log_size, sizeof(int));
		write(fd_log_path, &r->LSN, sizeof(uint64_t));
		write(fd_log_path, &r->prev_LSN, sizeof(uint64_t));
		write(fd_log_path, &r->trx_id, sizeof(int));
		write(fd_log_path, &r->type, sizeof(int));
		if(type == UPDATE || type == COMPENSATE){
			write(fd_log_path, &r->table_id, sizeof(int));
			write(fd_log_path, &r->pagenum, sizeof(pagenum_t));
			write(fd_log_path, &r->offset, sizeof(int));
			write(fd_log_path, &r->data_length, sizeof(int));
			write(fd_log_path, r->old_data, VALUE_SIZE);
			write(fd_log_path, r->new_data, VALUE_SIZE);
			if(type == COMPENSATE)
				write(fd_log_path, &r->next_undo, sizeof(uint64_t));
		}

		if(r->next == NULL){
			log_head = r;
			log_tail = r;
			r= r->next;
		}
		else{
			temp = r;
			r= r->next;
			free(temp);
		}
		
	}
}
	

	
