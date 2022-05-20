#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/shm.h>
#include <pthread.h>
#include <semaphore.h>
#include <fcntl.h>

sem_t* r_mutex;
sem_t* w_mutex;
sem_t* empty;
sem_t* full;

void* buffer;

#define __NR_gettid 186
void myRead() {
    long int tid = (long int)syscall(__NR_gettid);
    while(1) {
        sem_wait(full);	// wait full
        sem_wait(r_mutex);	// wait r_mutex
        sem_wait(w_mutex);	// wait w_mutex

        char str[2048];
        // 提取数据操作 消费者core code
        strcpy(str, buffer);
        printf("[tid: %lu]: %s\n", tid, str);
        memset(buffer, 0, 2048);	// clear

        sem_post(w_mutex);	// signal w_mutex
        sem_post(r_mutex);	// signal r_mutex
        sem_post(empty);	// signal empty
    }
}

int main() {
    // share memory 注意到key必须和生产者的一致 属于同一块共享内存
    key_t key = (key_t)114514;
    int shmid = shmget(key, 2048, 0666 | IPC_CREAT);	// create
    buffer = shmat(shmid, 0, 0);	// get addr

    // get public mutex
    w_mutex = sem_open("w_mutex", O_CREAT | O_RDWR);
    empty = sem_open("empty", O_CREAT | O_RDWR);
    full = sem_open("full", O_CREAT | O_RDWR);

    // create read mutex 
    // 新增的一个信号量 a signal mutex add
    r_mutex = sem_open("r_mutex", O_CREAT | O_RDWR, 0666, 1);
    pthread_t t1, t2;
    // 启动两个消费者进程
    pthread_create(&t1, NULL, (void*)&myRead, NULL);
    pthread_create(&t2, NULL, (void*)&myRead, NULL);

    // 暂停进行观察
    pause();

    return 0;
}
