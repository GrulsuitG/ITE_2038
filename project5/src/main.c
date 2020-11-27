#include "db.h"
#include<time.h>
#include<pthread.h>

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
	db_find(1,2,value,num);
	sleep(2);
	db_update(1,2,"value",num);
	trx_commit(num);
}
void* func(void *arg){
	char * value =malloc(120);

	int num =trx_begin();
	db_find(1,2,value,num);
	trx_commit(num);
}

int main(){
	int num, i, id;
	char c[120];
	char* value = malloc(120);
	int* list;
	page_t *page;
	record *r;
	srand(time(0));
	
	pthread_t aa[10];
	
	//printf("%d",open_table("db"));
	//printf("%d",open_table("gg"));
	init_db(100);
	num = open_table("db");
	pthread_create(&aa[0], 0, find_func, NULL);
	sleep(1);
	pthread_create(&aa[1], 0, func, NULL);

	pthread_join(aa[0], NULL);
	pthread_join(aa[1], NULL);
	
	
shutdown_db();
//print_tree(1);

}

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
