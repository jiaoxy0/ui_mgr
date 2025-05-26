#ifndef MY_QUEUE_H
#define MY_QUEUE_H

#include <rtthread.h>

// 定义队列的数据类型
#define MY_QUEUE_DATA_TYPE int

// 定义错误码
#define MY_QUEUE_SUCCESS    0
#define MY_QUEUE_ERROR      -1
#define MY_QUEUE_FULL       -2
#define MY_QUEUE_EMPTY      -3

// 定义内存分配和释放函数
#define my_queue_malloc     rt_malloc
#define my_queue_free       rt_free

// 定义队列结构体
typedef struct {
    MY_QUEUE_DATA_TYPE *data;  // 动态分配的队列数据数组
    int front;                 // 队列头部索引
    int rear;                  // 队列尾部索引
    int size;                  // 队列当前大小
    int max_size;              // 队列最大容量
} my_queue_t;

// 初始化队列
int my_queue_init(my_queue_t *queue, int max_size);

// 入队操作
int my_queue_enqueue(my_queue_t *queue, MY_QUEUE_DATA_TYPE data);

// 出队操作
MY_QUEUE_DATA_TYPE my_queue_dequeue(my_queue_t *queue, int *error);

// 查看队列头部数据（不移除）
MY_QUEUE_DATA_TYPE my_queue_peek(my_queue_t *queue, int *error);

// 重置队列
void my_queue_reset(my_queue_t *queue);

/**
 * @brief Get the size of queue
 */
int my_queue_get_size(my_queue_t *queue, int *error);

// 销毁队列
void my_queue_destroy(my_queue_t *queue);

#endif /* MY_QUEUE_H */
