#include "my_list.h"

#include <stdio.h>
#include <assert.h>


#define ZERO_INDEX 1
ListErr ListInit(List * list, size_t start_capacity, list_error_storage_type * err) {
    assert(list);
    if (start_capacity < 2) {
        printf("ERROR: start capacity <= 1 %s:%d\n", __FILE__, __LINE__);
        return LIST_WAS_ERROR;
    }

    list->capacity = start_capacity;
    list->nodes = (Node *) calloc(list->capacity, sizeof(Node));

    if (list->nodes == NULL) {
        printf("ERROR: memory allocated error %s:%d\n", __FILE__, __LINE__);
        return LIST_WAS_ERROR;
    }

    list->nodes[0].data = 0;
    list->nodes[0].next = 0;
    list->nodes[0].prev = 0;

    list->free = 1;

    for (int i = 1; i < list->capacity - 1; i++) {
        list->nodes[i].next = i + 1;
        list->nodes[i].prev = -1;
    }

    list->nodes[list->capacity - 1].next = 0;
    list->nodes[list->capacity - 1].prev = -1;

    list->size = 0;

    return LIST_NO_ERROR;
}

void ListDestroy(List * list, list_error_storage_type * err) {
    if (list == NULL) {
        printf("ERROR: try to destroy NULL list %s:%d\n", __FILE__, __LINE__);
        return;
    }

    for (int i = 0; i < list->capacity; i++) {
        list->nodes[i].data = 0;
        list->nodes[i].next = 0;
        list->nodes[i].prev = 0;
    }
    free(list->nodes);
    list->nodes = NULL;

    list->free = 0;
    list->size = 0;
    list->capacity = 0;
    printf("List Destroyed correctly\n");
    return;
}

ListErr ListVerifyBasic(const List * list, list_error_storage_type * err) {
    if (list == NULL) {
        printf("ERROR: NULL list %s:%d\n", __FILE__, __LINE__);
        if (err) *err |= LIST_NULL_PTR;
        return LIST_NULL_PTR;
    }
    int flag = 0;
    if (list->nodes == NULL) {
        printf("ERROR: NULL nodes %s:%d\n", __FILE__, __LINE__);
        if (err) *err |= LIST_NULL_NODES;
        flag = 1;
    }
    if (list->capacity < 2) {
        printf("ERROR: invalid capacity %s:%d\n", __FILE__, __LINE__);
        if (err) *err |= LIST_INVALID_CAPACITY;
        flag = 1;
    }
    if (list->size < 0 || list->size >= list->capacity) {
        printf("ERROR: invalid size %s:%d\n", __FILE__, __LINE__);
        if (err) *err |= LIST_INVALID_SIZE;
        flag = 1;
    }
    if (list->free < 0 || list->free >= list->capacity) {
        printf("ERROR: invalid free %s:%d\n", __FILE__, __LINE__);
        if (err) *err |= LIST_INVALID_FREE;
        flag = 1;
    }
    if (list->size > 0 && (list->nodes[0].next <= 0 || list->nodes[0].next >= list->capacity)) {
        printf("ERROR: invalid head %s:%d\n", __FILE__, __LINE__);
        if (err) *err |= LIST_INVALID_HEAD;
        flag = 1;
    }
    if (list->size > 0 && (list->nodes[0].prev <= 0 || list->nodes[0].prev >= list->capacity)) {
        printf("ERROR: invalid tail %s:%d\n", __FILE__, __LINE__);
        if (err) *err |= LIST_INVALID_TAIL;
        flag = 1;
    }
    if (flag) return LIST_WAS_ERROR;
    return LIST_NO_ERROR;
}

ListErr ListVerifyDeep(const List * list, list_error_storage_type * err) {
    if (ListVerifyBasic(list, err) == LIST_NULL_PTR) return LIST_NULL_PTR;

    if (list->size == 0) {
        if (list->nodes[0].next != 0 || list->nodes[0].prev != 0) {
            printf("ERROR: list link error %s:%d\n", __FILE__, __LINE__);
            if (err) *err |= LIST_WRONG_LINK;
            return LIST_WAS_ERROR;
        }
        return LIST_NO_ERROR;
    }

    int flag = 0;

    int visited_next = 0;
    for (int i = list->nodes[0].next; i != 0; i = list->nodes[i].next) {
        visited_next += 1;
        if (visited_next > list->size) {
            flag = 1;
            printf("ERROR: wrong next way %s:%d\n", __FILE__, __LINE__);
            if (err) *err |= LIST_WRONG_NEXT_WAY;
            break;
        }
    }
    if (visited_next != list->size) {
        flag = 1;
        printf("ERROR: wrong next way %s:%d\n", __FILE__, __LINE__);
        if (err) *err |= LIST_WRONG_NEXT_WAY;
    }


    int visited_prev = 0;
    for (int i = list->nodes[0].prev; i != 0; i = list->nodes[i].prev) {
        visited_prev += 1;
        if (visited_prev > list->size) {
            flag = 1;
            printf("ERROR: wrong prev way %s:%d\n", __FILE__, __LINE__);
            if (err) *err |= LIST_WRONG_PREV_WAY;
            break;
        }
    }
    if (visited_prev != list->size) {
        flag = 1;
        printf("ERROR: wrong prev way %s:%d\n", __FILE__, __LINE__);
        if (err) *err |= LIST_WRONG_PREV_WAY;
    }

    if (flag) return LIST_WAS_ERROR;
    return LIST_NO_ERROR;
}

ListErr MakeDotFile (const List * list, const char * file_way) {
    if (list == NULL) {
        printf("ERROR: list NULL pointer %s:%d\n", __FILE__, __LINE__);
        return LIST_WAS_ERROR;
    }
    if (file_way == NULL) {
        printf("ERROR: dot_file_way NULL %s:%d\n", __FILE__, __LINE__);
        return LIST_WAS_ERROR;
    }
    FILE * dot_file = fopen(file_way, "w");
    if (dot_file == NULL) {
        printf("ERROR: dot_file_way was not opened %s:%d\n", __FILE__, __LINE__);
        return LIST_WAS_ERROR;
    }
    //НАЧАЛО ЗАПИСИ В ФАЙЛ ДЛЯ ГРАФА
    fprintf(dot_file, "digraph list {\n");
    fprintf(dot_file, "    rankdir=LR;\n");
    fprintf(dot_file, "\n");

    for (int i = 1; i < list->capacity; i++) {
        if (list->nodes[i].prev == -1) {
            fprintf(dot_file, "    Node_%d [shape = Mrecord, style=filled, fillcolor=grey", i);
        } else {
            fprintf(dot_file, "    Node_%d [shape = Mrecord, style=filled, fillcolor=lightblue", i);
        }
        fprintf(dot_file, ", label=\"<index> index = %d | <value> value = %lf | {<prev> prev = %d | <next> next = %d }\"]\n", i, list->nodes[i].data, list->nodes[i].prev, list->nodes[i].next);
    }
    fprintf(dot_file, "\n");
    fprintf(dot_file, "    subgraph Main_Index {\n");
    fprintf(dot_file, "        rankdir=TB;\n");
    fprintf(dot_file, "        Head [shape = Mrecord, label=\"Head = %d\"]\n", list->nodes[0].next);
    fprintf(dot_file, "        Tail [shape = Mrecord, label=\"Tail = %d\"]\n", list->nodes[0].prev);
    fprintf(dot_file, "        Free [shape = Mrecord, label=\"Free = %d\"]\n", list->free);
    fprintf(dot_file, "        Head -> Node_%d\n", list->nodes[0].next);
    fprintf(dot_file, "        Free -> Node_%d\n", list->free);
    fprintf(dot_file, "        Tail -> Node_%d\n", list->nodes[0].prev);
    fprintf(dot_file, "    }\n");
    
    for (int i = list->nodes[0].next; list->nodes[i].next != 0; i = list->nodes[i].next) {
        fprintf(dot_file, "    Node_%d:next -> Node_%d:prev\n", i, list->nodes[i].next);
        fprintf(dot_file, "    Node_%d:prev -> Node_%d:next\n", list->nodes[i].next, i);
    }
    for (int i = list->free; list->nodes[i].next != 0; i = list->nodes[i].next) {
        fprintf(dot_file, "    Node_%d -> Node_%d [style=invis]\n", i, list->nodes[i].next);
    }
    //запись связей для корректной последовательности
    fprintf(dot_file, "    Node_%d", 1);
    for (int i = 2; i < list->capacity; i++) {
        if (list->nodes[i].prev == -1) continue;
        fprintf(dot_file, " -> Node_%d", i);
    }
    fprintf(dot_file, " [style=invis]\n");
    

    fprintf(dot_file, "}");
    return LIST_NO_ERROR;
}

ListErr ListDump(const List * list,const char * dump_file_way, const char * dot_file_way) {
     if (list == NULL) {
        printf("ERROR: list NULL pointer %s:%d\n", __FILE__, __LINE__);
        return LIST_WAS_ERROR;
    }
    if (dump_file_way == NULL) {
        printf("ERROR: dump_file_way NULL %s:%d\n", __FILE__, __LINE__);
        return LIST_WAS_ERROR;
    }
    if (dot_file_way == NULL) {
        printf("ERROR: dot_file_way NULL %s:%d", __FILE__, __LINE__);
        return LIST_WAS_ERROR;
    }

    MakeDotFile(list, dot_file_way);

    FILE * dump_file = fopen(dump_file_way, "w");

    fprintf(dump_file, "LIST DUMP FILE\n\n");

    fprintf(dump_file, "Head - %d\n", list->nodes[0].next);
    fprintf(dump_file, "Tail - %d\n", list->nodes[0].prev);
    fprintf(dump_file, "Free - %d\n\n", list->free);

    fprintf(dump_file, "INDEXES WITH VALUE:  ");
    for (int i = 0; i < list->capacity; i++) {
        if (list->nodes[i].prev == -1) continue;
        fprintf(dump_file, "%3d ", i);
    }
    fprintf(dump_file, "\n");

    fprintf(dump_file, "FREE INDEXES:  ");
    for (int i = 0; i < list->capacity; i++) {
        if (list->nodes[i].prev != -1) continue;
        fprintf(dump_file, "%3d ", i);
    }
    fprintf(dump_file, "\n");
    fprintf(dump_file, "\n");
    //NEXT WAY
    fprintf(dump_file, "NEXT WAY\n");
    fprintf(dump_file, "HEAD");
    for (int i = list->nodes[0].next; i != 0; i = list->nodes[i].next) {
        fprintf(dump_file, " -> %d", i);
    }
    fprintf(dump_file, "\n\n");
    //PREV WAY
    fprintf(dump_file, "PREV WAY\n");
    fprintf(dump_file, "TAIL");
    for (int i = list->nodes[0].prev; i != 0; i = list->nodes[i].prev) {
        fprintf(dump_file, " -> %d", i);
    }
    fprintf(dump_file, "\n\n");
    //FREE WAY
    fprintf(dump_file, "FREE WAY\n");
    fprintf(dump_file, "FREE");
    for (int i = list->free; i != 0; i = list->nodes[i].next) {
        fprintf(dump_file, " -> %d", i);
    }
    fprintf(dump_file, "\n\n");

    //NODES INFO
    for (int i = 1; i < list->capacity; i++) {
        fprintf(dump_file, "index = %3d, next = %3d, prev = %3d, value = %3.4lf\n", i, list->nodes[i].next, list->nodes[i].prev, list->nodes[i].data);
    }



    return LIST_NO_ERROR;
}

ListErr ListResize(List * list, int new_capacity, list_error_storage_type * err) {
    if (ListVerifyBasic(list, err)) return LIST_WAS_ERROR;

    if (new_capacity <= list->capacity) {
        printf("ERROR: new capacity must be greater than now capacity %s:%d\n", __FILE__, __LINE__);
        if (err) *err |= LIST_INVALID_CAPACITY;
        return LIST_WAS_ERROR;
    }

    Node * new_nodes = (Node *) realloc(list->nodes, new_capacity * sizeof(Node));
    if (new_nodes == NULL) {
        printf("ERROR: memory realloc error %s:%d\n", __FILE__, __LINE__);
        if (err) *err |= LIST_NULL_NODES;
        return LIST_WAS_ERROR;
    }
    list->nodes = new_nodes;

    for (int i = list->capacity; i < new_capacity; i++) {
        list->nodes[i].data = 0;
        list->nodes[i].prev = -1;
        if (i != new_capacity - 1) list->nodes[i].next = i + 1;
        else                       list->nodes[i].next = 0;
    }

    if (list->free == 0) list->free = list->capacity;
    else {
        int last_free = list->free;
        while (list->nodes[last_free].next != 0) last_free = list->nodes[last_free].next;
        list->nodes[last_free].next = list->capacity;
    }

    list->capacity = new_capacity;
    return LIST_NO_ERROR;
}

ListErr ListInsertAfter(List * list, int index, double value, list_error_storage_type * err) {
    if (ListVerifyBasic(list, err) != LIST_NO_ERROR) {
        return LIST_WAS_ERROR;
    }

    if (index < 0 || index >= list->capacity || (index != 0 && list->nodes[index].prev == -1)) {
        printf("ERROR: invalid index %s:%d\n", __FILE__, __LINE__);
        return LIST_WAS_ERROR;
    }

    if (list->free == 0) {
        if (ListResize(list, list->capacity * 2, err) != LIST_NO_ERROR) {
            printf("ERROR: list resize error %s:%d\n", __FILE__, __LINE__);
            return LIST_WAS_ERROR;
        }
    }

    int free_index = list->free;
    list->free = list->nodes[free_index].next;

    list->nodes[free_index].data = value;

    list->nodes[free_index].next = list->nodes[index].next;
    list->nodes[free_index].prev = index;

    if (list->nodes[index].next != 0) {//этот индекс не был последним
        list->nodes[list->nodes[index].next].prev = free_index;
    } else {//этот индекс был последним
        list->nodes[0].prev = free_index;
    }

    list->nodes[index].next = free_index;

    list->size++;
    return LIST_NO_ERROR;
}

ListErr ListPop(List * list, int physical_index, double * res, list_error_storage_type * err) {
    if (ListVerifyBasic(list, err) != LIST_NO_ERROR) {
        return LIST_WAS_ERROR;
    }

    if (physical_index <= 0 || physical_index >= list->capacity) {
        printf("ERROR: invalid index %s:%d\n", __FILE__, __LINE__);
        return LIST_WAS_ERROR;
    } else if (list->nodes[physical_index].prev == -1) {
        printf("ERROR: try to pop empty index %s:%d\n", __FILE__, __LINE__);
        return LIST_WAS_ERROR;
    }

    if (res != NULL) {
        *res = list->nodes[physical_index].data;
    } else {
        printf("ERROR: NULL pointer for result, result will not returned %s:%d\n", __FILE__, __LINE__);
    }
    //КОНЕЦ ПРОВЕРОК

    //ИЗМЕНЕНИЯ В СТРУКТУРЕ
    list->nodes[physical_index].data = 0;

    int next_index = list->nodes[physical_index].next;
    int prev_index = list->nodes[physical_index].prev;

    if (next_index == 0) {//это был последний элемент
        list->nodes[0].prev = prev_index;
    } else {//это был НЕ последний элемент
        list->nodes[next_index].prev = prev_index;
    }
    if (prev_index == 0) {//это был первый элемент
        list->nodes[0].next = next_index;
    } else {//это был НЕ первый элемент
        list->nodes[prev_index].next = next_index;
    }

    list->nodes[physical_index].next = list->free;
    list->free = physical_index;
    list->nodes[physical_index].prev = -1;

    list->size--;

    return LIST_NO_ERROR;
}




//int main() {
//    return 0;
//}