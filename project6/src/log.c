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
	int *loser;
	if(open_log(log_path, logmsg_path)){
		loser = analysis();
		if(flag == NORMAL){
			redo(0);
			undo(0, loser);
			truncate(log_path, 0);
			lseek(fd_log_path,0, SEEK_SET);
		}
		if(flag == REDO_CRASH){
			redo(log_num);
			indexbuf_flush();
			
		}
		else if(flag == UNDO_CRASH){
			redo(0);
			undo(log_num, loser);
			logbuf_flush();
		}
		
	}
	
	
}

int* analysis(){
	int *winner = (int*)malloc(sizeof(int) * WINNER_SIZE);
	int *loser = (int*)malloc(sizeof(int) * LOSER_SIZE);
	int winnum =0, losenum =0;
	int winnersize, losersize;
	int size, type, trx_id;
	int i,j, flag;
	int *temp;
	winnersize= 1;
	losersize =1;
	fprintf(logmsg,"[ANALYSIS] Analysis pass start\n");
	
	lseek(fd_log_path, 0, SEEK_SET);
	
	while(read(fd_log_path, &size, sizeof(int)) > 0){
		lseek(fd_log_path, 16, SEEK_CUR);
		read(fd_log_path, &trx_id, sizeof(int));
		read(fd_log_path, &type, sizeof(int));
		if(type == BEGIN){
			winner[winnum++] = trx_id;
			if(winnum ==(winnersize*WINNER_SIZE)){
				winnersize++;
				winner = (int*)realloc(winner, (winnersize*WINNER_SIZE)*sizeof(int));
			}
		}
		else if (type == COMMIT){
			loser[losenum++] = trx_id;
			if(losenum == (losersize*LOSER_SIZE)){
				losersize++;
				loser = (int*)realloc(loser, (losersize*LOSER_SIZE)*sizeof(int));
			}
		}
		lseek(fd_log_path, size-DEFAULT_SIZE, SEEK_CUR);
	}
	temp = (int*)malloc(sizeof(int)*winnersize*WINNER_SIZE); 
	for(i=0; i<winnum; i++){
		flag =0;
		for(j=0; j<losenum; j++){
			if(winner[i] == loser[j]){
				loser[j] = 0;
				flag = 1;
				break;
			}
		}
		if(flag ==0){
			temp[i] = winner[i];
			winner[i] = 0;
		}
	}
	for(i=0; i<winnum; i++){
		if(temp[i] !=0)
			loser[losenum++] = temp[i]; 
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
	return loser;
}

void redo(int log_num){
	int type, trx_id, size, redo_num;
	int table_id, offset, index;
	uint64_t LSN, next_undo;
	pagenum_t pagenum;
	page_t *page;
	char data[VALUE_SIZE];
	char *str = (char*) malloc(sizeof(char) * 10);
	redo_num =1;
	fprintf(logmsg,"[REDO] Redo pass start\n");
	
	lseek(fd_log_path, 0, SEEK_SET);
	
	while(read(fd_log_path, &size, sizeof(int)) > 0){
		read(fd_log_path, &LSN, sizeof(uint64_t));
		lseek(fd_log_path, 8, SEEK_CUR);
		read(fd_log_path, &trx_id, sizeof(int));
		read(fd_log_path, &type, sizeof(int));
		if(type == BEGIN){
			fprintf(logmsg,"LSN %lu [BEGIN] Transaction id %d\n", LSN+size, trx_id);
			
		}
		else if (type == COMMIT){
			fprintf(logmsg,"LSN %lu [COMMIT] Transaction id %d\n", LSN+size, trx_id);
			
		}
		else if (type == UPDATE){

			read(fd_log_path, &table_id, sizeof(int));
			read(fd_log_path, &pagenum, sizeof(pagenum_t));
			read(fd_log_path, &offset, sizeof(int));
			lseek(fd_log_path,sizeof(int)+VALUE_SIZE,SEEK_CUR);
			read(fd_log_path, data, VALUE_SIZE);
			sprintf(str, "DATA%d",table_id);
			open_table(str);
			page= buf_read_page(table_id, pagenum);
			if(page->LSN >= LSN+size){
				//printf("%lu %lu\n", page->LSN, LSN+size);
				buf_return_page(table_id, pagenum, false, page->index);	
				fprintf(logmsg, "LSN %lu [CONSIDER_REDO] Transaction id %d\n", LSN+size, trx_id);
			}
			else{
				
				index = (offset % PAGE_SIZE) / 128;
				strncpy(page->record[index-1]->value, data, VALUE_SIZE);
				page->LSN = LSN+size;
				buf_return_page(table_id, pagenum, true, page->index);
				fprintf(logmsg, "LSN %lu [UPDATE] Transaction id %d redo apply\n", LSN+size, trx_id);
			}
			buf_return_page(table_id, pagenum, false, page->index);
		}
		else if (type == ROLLBACK){
			fprintf(logmsg, "LSN %lu [ROLLBACK] Transaction id %d\n", LSN+size, trx_id);
		}
		else if (type == COMPENSATE){
			fprintf(logmsg, "LSN %lu [CLR] next undo lsn %lu", LSN+size, next_undo);
		}
		if(redo_num++ == log_num){
			return;
		}
	}
	
	fprintf(logmsg, "[REDO] Redo pass end\n");
	
}

void undo(int log_num, int *loser){

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

int log_write(int type, int trx_id, uint64_t prev, int table_id ,page_t* page, int index, char* value){
	log_record* r = (log_record*)malloc(sizeof(log_record));
	r->next = NULL;
	int offset;
		
	r->LSN = log_tail->LSN + log_tail->log_size;	
	r->prev_LSN = prev;
	r->trx_id = trx_id;
	r->type = type;
	
	if(type == BEGIN || type == COMMIT || type == ROLLBACK)
		r->log_size = DEFAULT_SIZE;
	else if (type == UPDATE){
		r->log_size = UPDATE_SIZE;
		r->table_id = table_id;
		r->pagenum = page->mypage;
		offset = page->mypage*PAGE_SIZE + 128*(index+1);
		r->offset = offset;
		r->data_length = VALUE_SIZE;
		strncpy(r->old_data, page->record[index]->value, VALUE_SIZE);
		strncpy(r->new_data, value, VALUE_SIZE);
	}/*
	else if(type == COMPENSATE){	 
		log_buf[log_num]->log_size = COMPENSATESIZE;
	}*/

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
		write(fd_log_path , &r->log_size , sizeof(int));
		write(fd_log_path , &r->LSN , sizeof(uint64_t));
		write(fd_log_path , &r->prev_LSN , sizeof(uint64_t));
		write(fd_log_path , &r->trx_id , sizeof(int));
		write(fd_log_path , &r->type , sizeof(int));
		if(type == UPDATE){
			write(fd_log_path , &r->table_id , sizeof(int));
			write(fd_log_path , &r->pagenum , sizeof(pagenum_t));
			write(fd_log_path , &r->offset , sizeof(int));
			write(fd_log_path , &r->data_length , sizeof(int));
			write(fd_log_path , r->old_data , VALUE_SIZE);
			write(fd_log_path , r->new_data , VALUE_SIZE);
		}
		else if(type == COMPENSATE){
			write(fd_log_path, r, COMPENSATE_SIZE);
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
	

	
