#include "db.h"
#include<time.h>

int get_random_number(int from, int to) {
	return rand() % (to - from + 1) + from;
}

//문자열 생성함수('a'~'z')
void get_random_str(char* data) {
	int num = get_random_number(0,120);
	for (int i = 0; i < num; i++) {
		data[i] = get_random_number(0, 25) + 'a';
	}
	data[num] = 0;
}

int main(){
	int num, i;
	char c[120];
	char* value = malloc(120);
	int* list;
	srand(time(0));
	//printf("%d",open_table("db"));
	//printf("%d",open_table("gg"));
	open_table("dddd");
	//printf("%d",open_table("db"));
	/*print_tree(root);
	for(i = 0; i< 1000; i++){
		num = rand()%100;	
		if(!db_find(num, value))	
			printf("%d : %s\n",num,value);
	}
	//print_tree(root);
	//db_insert(11111, "asdasd");
	print_tree(root)c;
	//print_tree(root);
	//open_table("dd");*/
	for( i = 0; i< 500; i++){
		num = rand()%10000;	
		get_random_str(c);	
		//printf("%d %s\n", num, c);
		db_insert(num,c);
		//print_tree(root);

	}
print_tree(root);
num =open_table("asdf");
printf("===================cur table:[%d]=======================\n", num);
num=open_table("dddd");
printf("===================cur table:[%d]=======================\n", num);
print_tree(root);
printf("=========================================\n");
for(i = 0; i< 20; i++){
		num = rand()%10000;	
		if(!db_find(num, value))	
			printf("%d : %s\n",num,value);
	}

printf("=========================================\n");
for( i = 0; i< 1000; i++){
		num = rand()% 10000;	
		get_random_str(c);	
		//printf("%d %s\n", num, c);
		db_delete(num);
//print_tree(root);
	}
print_tree(root);

//print_tree(root);*/
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
