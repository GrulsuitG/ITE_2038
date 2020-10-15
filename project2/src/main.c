#include "db.h"

// MAIN

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
		root = insert(root, key, str);
	//	fgets(str, 100, stdin);
           // if(!db_insert(key, str))
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
}
