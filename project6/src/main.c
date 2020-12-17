#include "db.h"
#include<time.h>
#include<pthread.h>

#define THREAD_NUM 10

int get_random_number(int from, int to) {
	return rand() % (to - from + 1) + from;
}

//문자열 생성함수('a'~'z')
void get_random_str(char* data) {
	int num = get_random_number(1,109);
	for (int i = 0; i < num; i++) {
		data[i] = get_random_number(0, 25) + 'a';
	}
	data[num] = 0;
	}

void* find_func(void *arg){
	char * value = malloc(120);
	int num = trx_begin();
	int id, s;
	for(int i=0; i<100; i++){
		s = rand() %100;
		//lock_acquire(1, s, num, 0);
		db_find(1,s,value,num);
		//printf("%s\n", value);
		
	}
	trx_commit(num);
}
void* func(void *arg){
	char * value =malloc(120);

	int num =trx_begin();
	//printf("a");
	db_update(1,2,"value",num);
	trx_commit(num);
}
int main(){
	int num, choice, a;
	srand(time(0));
	init_db(10, 0, 0, "logfile.data", "logmsg.txt");
	open_table("DATA2");
	for(int i=0; i<20; i++){

		num = trx_begin();
		printf("%d begin \n", num);
		a = (i+1)*15;
		choice = rand() %3;
		db_update(2,a, "Hello", num);
		printf("update %d: %d\n", num, a);
		if(choice == 0){	
			
			trx_commit(num);
			printf("%d commit\n", num);
		}
		else if(choice ==1){
			
			trx_abort(num);
			printf("%d abort\n", num);
		}
		//print_buf();
	}
	logbuf_flush();
}


/*
int main(){
	int num, i, id;
	char c[120];
	char* value = malloc(120);
	int* list;
	page_t *page;
	record *r;
	srand(time(0));
	
	pthread_t aa[THREAD_NUM];
	
	//printf("%d",open_table("db"));
	//printf("%d",open_table("gg"));
	init_db(431);
	num = open_table("db");
	/*for(i = 0; i<1000; i++){
		num = rand()%1000;
		db_insert(1,num,"asdfsadf");
	}
	for(i = 0; i<THREAD_NUM; i++){
		pthread_create(&aa[i], 0, find_func, NULL);
	}
	for(i=0; i<THREAD_NUM; i++){
		pthread_join(aa[i], NULL);
	}
	//id = trx_begin();
	//db_update(1,10,"adadfbv", id);
/*	for(i = 0; i<1000; i++){
		db_find(1,i,c,id);
		printf("%s\n",c);
	}*/
	//trx_commit(id);
//shutdown_db();
//print_tree(1);


// MAIN
/*
int main() {
    char instruction;
	char* str = malloc(100);
    int64_t key, range2;
	int aa;
    
    printf("> ");
	
    while (scanf("%c", &instruction) !=EOF) {
		//getchar();
		//scanf("%c", &instruction);
		//while(getchar() != '\n');
		switch (instruction) {
        case 'd':
            scanf("%ld", &key);
            if(!db_delete(key))
               ;// print_tree(root);
            else
                printf("delete fail\n");
            break;
        case 'i':
            scanf("%ld", &key);
           scanf("%s", str);
		//root = insert(root, key, str);
	//	fgets(str, 100, stdin);
            if(!db_insert(key, str))
                print_tree(root);
           // else
           //     printf("insert fail\n");
            break;
        case 'f':
        case 'p':
            scanf("%ld", &key);
            if(!db_find(key, str))
                printf("%s\n", str);
            else
                printf("find fail\n");
            break;
        case 'r':
            scanf("%ld %ld", &key, &range2);
            if (key > range2) {
                int tmp = range2;
                range2 = key;
                key = tmp;
            }
            find_and_print_range(root, key, range2, instruction == 'p');
            break;
        case 'l':
            print_leaves(root);
            break;
        case 'q':
            while (getchar() != (int)'\n');
            return EXIT_SUCCESS;
            break;
        case 't':
            print_tree(root);
            break;
        case 'v':
            verbose_output = !verbose_output;
            break;
        case 'x':
            if (root)
                root = destroy_tree(root);
            //print_tree(root);
            break;
        case 'o':
            scanf("%s", str);
		//printf("%s\n", str);
            aa=open_table(str);
		break;
        
        default:
            usage_2();
            break;
        }
        while (getchar() != '\n');
        printf("[%d]%s>", aa,filename);
    }
    printf("\n");
	root =destroy_tree(root);
    return EXIT_SUCCESS;
}*/
