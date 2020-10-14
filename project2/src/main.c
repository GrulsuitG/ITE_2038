#include "db.h"

int main(){
	int64_t key;
	char* str = "value";
	open_table("an");
	for(int i = 0; i<100; i++){
		printf("%d" , i);
        
        
		//printf("a");
		db_insert(i, str);
	//	printf("b");
	//	print_tree(root);
		//printf("c\n");
	}
}
