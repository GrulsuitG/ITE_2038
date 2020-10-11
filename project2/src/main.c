#include "index.h"

// MAIN

int main( int argc, char ** argv ) {
    char instruction;
    char* str;
    int64_t key, range2;
    
    printf("> ");
    while (scanf("%c", &instruction) != EOF) {
        switch (instruction) {
        case 'd':
            scanf("%ld", &key);
            root = delete(root, key);
            print_tree(root);
            break;
        case 'i':
            scanf("%ld", &key);
            scanf("%s", str);
            root = insert(root, key, str);
            print_tree(root);
            break;
        case 'f':
        case 'p':
            scanf("%ld", &key);
            find_and_print(root, key, instruction == 'p');
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
            print_tree(root);
            break;
        default:
            usage_2();
            break;
        }
        while (getchar() != (int)'\n');
        printf("> ");
    }
    printf("\n");

    return EXIT_SUCCESS;
}
