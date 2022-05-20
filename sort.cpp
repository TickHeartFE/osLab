#include <pthread.h>
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <thread>
#include <string.h>
#include <unistd.h>
#include <time.h>
using namespace std;

struct node {
    int l, r;
    int id;
};

// 这里有个隐含条件 为了保证算法有效
// 最好保证 data_num % thread_num == 0

// 线程的数量
const int thread_num = 6;
// 数据量
const int data_num = 6000000;
// 容器的存储量 和数据量 保持一致
const int Maxn = data_num;
int a[data_num];

// 多线程归并合并好的区间 归并的核心函数 二路归并
void merge(int left1, int right1, int left2, int right2) {
    int n = right2 - left1 + 1;
    int* data = new int[n];
    for(int i = 0; i < n; i++) data[i] = 0;
    int l1 = left1, l2 = left2, i = 0;
    while(l1 <= right1 && l2 <= right2) {
        if(a[l1] < a[l2]) data[i++] = a[l1++];
        else data[i++] = a[l2++];
    }
    while(l1 <= right1) data[i++] = a[l1++];
    while(l2 <= right2) data[i++] = a[l2++];
    int id = 0;
    for(int i = left1; i <= right2; i++) a[i] = data[id++];  // 将data中合并好的数据进行一个迁移
    delete[] data;
}

// 快排core代码 快速排序 快速排序 简便core
void sort(int left, int right) {
    if(left >= right) return;
    int l = left, r = right;
    while(l < r) {
        while(l < r && a[l] <= a[r]) l++;
        if(l != r) swap(a[l], a[r]);
        while(l < r && a[l] <= a[r]) r--;
        if(l != r) swap(a[l], a[r]);
    }
    sort(left, l - 1);
    sort(l + 1, right);
    return;
}

void* __sort(void* segment) {
    struct node* Segment;
    Segment = (struct node*)segment;
    cout << "Thread " << Segment->id << ":Sort begin!" << endl;
    sort(Segment->l, Segment->r);
    cout << "Thread " << Segment->id << ":Sort End!" << endl;
    void* tmp;
    return tmp;
}

int main() {
    // 随机播种种子
    srand((unsigned)time(NULL));
    // 输入数据量为600w的数据
    for(int i = 0; i < data_num; i++) a[i] = rand();
    //cout << "init array" << endl;
    //for(int i = 0; i < data_num; i++) cout << a[i] << " ";
    cout << endl;
    node* s = new node[thread_num];
    // 将区间分为thread_num段 记录每一段的id 左端点 右端点
    // 一共需要分为 thread_num 个段
    // 每一端 左端点 l -> i * (data_num / thread_num)   r-> (i + 1) * (data_num / thread_num) - 1
    for(int i = 0; i < thread_num; i++) {
        s[i].id = i;
        s[i].l = i * (data_num / thread_num);
        s[i].r = (i + 1) * (data_num / thread_num) - 1;
    }
    // 线程容器
    pthread_t t[thread_num];

    // 开始计时
    time_t begin = clock();

    // 启动对应的线程 最后一个传入的void* -> args
    for(int i = 0; i < thread_num; i++) pthread_create(&t[i], NULL, __sort, (void*)&s[i]);
    // 用来等待thread_num个子线程的结束 处理好所有十块区间的各自的快速排序
    // 才可以回到对应的主线程进行一个合并的操作
    for(int i = 0; i < thread_num; i++) pthread_join(t[i], NULL);
    // 最后将排序好的thread_num块区域进行合并 得到最后的结果
    int left = 0, right = data_num / thread_num - 1;
    for(int i = 1; i < thread_num; i++) {
        merge(left, right, s[i].l, s[i].r);
        right += (data_num / thread_num);
    }

    // 结束计时
    time_t end = clock();

    // 打印时间
    cout << (double)(end - begin) / CLOCKS_PER_SEC;

    //cout << "after sort: " << endl;
    //for(int i = 0; i < data_num; i++) cout << a[i] << " ";
    //cout << endl;
    delete[] s;
    return 0;
}