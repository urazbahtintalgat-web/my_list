#include <stdio.h>
#include <stdlib.h>

#include "my_list.h"

int main() {
    List list1;
    list_error_storage_type err = 0;

    ListInit(&list1, 10, &err);

    for (int i = 0; i < 10; i++) {
        ListInsertAfter(&list1, i, (double) i + 1, &err);
    }
    
    for (int i = 1; i < 10; i++) {
        printf("data[%d]: %lf\n", i, list1.nodes[i].data);
        printf("prev[%d] -- element -- next[%d]\n", i, i);
        printf("%7d                 %5d\n\n", list1.nodes[i].prev, list1.nodes[i].next);
    }
    list1.nodes[7].next = 16;
    MakeDotFile(&list1, "./graphs/training3.dot");
    ListDump(&list1, "./dumps/dumplist.txt", "./graphs/training3.dot");
    return 0;
}