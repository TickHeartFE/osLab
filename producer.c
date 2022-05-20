#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/shm.h>
#include <semaphore.h>
#include <fcntl.h>

int main() {
    // share memory
    key_t key = (key_t)114514;
    int shmid = shmget(key, 2048, 0666 | IPC_CREAT);	// create memory return id 创建内存空间 返回内存id
    printf("share memory id: %d\n", shmid);
    printf("--------------------\n");
    void* buffer = shmat(shmid, 0, 0);	// get addr by memory id

    // create public mutex 
    // 以下创建对应的公共信号量 
    // w_mutex=1, empty=1, full=0
    sem_t* w_mutex = sem_open("w_mutex", O_CREAT | O_RDWR, 0666, 1);
    sem_t* empty = sem_open("empty", O_CREAT | O_RDWR, 0666, 1);
    sem_t* full = sem_open("full", O_CREAT | O_RDWR, 0666, 0);

    char str[2048];

    // 启动一个生产者进程
    while(1) {
        printf("producer> ");
        scanf("%s", str);

        sem_wait(empty);	// wait empty
        sem_wait(w_mutex);	// wait w_mutex

        strcpy((char*)buffer, str);//进行一个写(生产操作)

        sem_post(w_mutex);	// signal w_mutex
        sem_post(full);	    // signal full
    }

    return 0;
}