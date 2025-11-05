#ifndef MY_LIST_H
#define MY_LIST_H
#include <stdio.h>

typedef unsigned int list_error_storage_type;
/**
 * @brief Это структура для хранения информации про один элемент в списке
 * 
 * @param data Значение элемента
 * @param next Индекс следующего элемента
 * @param prev Индекс предидущего элемента
 */
typedef struct {
    double data;
    int next;
    int prev;
} Node;

enum ListErr {
    LIST_WAS_ERROR =-1,
    LIST_NO_ERROR  = 0,
    //==========BASIC=================
    LIST_NULL_PTR          = 1 << 0,
    LIST_NULL_NODES        = 1 << 1,
    LIST_INVALID_CAPACITY  = 1 << 2,
    LIST_INVALID_SIZE      = 1 << 3,
    LIST_INVALID_FREE      = 1 << 4,
    LIST_INVALID_HEAD      = 1 << 5,
    LIST_INVALID_TAIL      = 1 << 6,
    //==========DEEP==================
    LIST_WRONG_NEXT_WAY    = 1 << 7,
    LIST_WRONG_PREV_WAY    = 1 << 8,
    LIST_WRONG_LINK        = 1 << 9
};

/**
 * @brief
 * 
 * @param nodes    Массив структур элементов списка (head и tail хранятся соответственно в массивах
 *                 next и prev на месте нулевого служебного индекса)
 * @param free     Индекс первого свободного элемента
 * @param size     Используемый размер
 * @param capacity Доступный размер (с учетом нулевого индекса)
 */
typedef struct {
    Node * nodes;
    int free;
    int size;
    int capacity;
} List;

ListErr ListInit   (List * list, size_t start_size, list_error_storage_type * err = NULL);
void    ListDestroy(List * list,                    list_error_storage_type * err = NULL);

ListErr ListVerifyBasic(const List * list, list_error_storage_type * err = NULL);
ListErr ListVerifyDeep (const List * list, list_error_storage_type * err = NULL);

ListErr ListResize(List * list, int new_capacity, list_error_storage_type * err = NULL);

int ListInsertAfter (List * list, int index, double value, list_error_storage_type * err);
int ListInsertBefore(List * list, int index, double value, list_error_storage_type * err);
int ListInsertBegin (List * list,            double value, list_error_storage_type * err);
int ListInsertEnd   (List * list,            double value, list_error_storage_type * err);
ListErr ListPop(List * list, int physical_index, double * res, list_error_storage_type * err);

ListErr MakeDotFile (const List * list, const char * file_way);
ListErr ListDump(const List * list,const char * dump_file_way, const char * dot_file_way);


#endif//MY_LIST_H