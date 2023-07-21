#include "ipc/sem.h"

void sem_init(sem_t* sem, int init_count) {
    sem->count = init_count;
    list_init(&sem->wait_list);
}