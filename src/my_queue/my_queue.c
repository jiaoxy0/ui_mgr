#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "my_queue.h"

// 初始化队列
int my_queue_init(my_queue_t *queue, int max_size) {
    if (queue == NULL || max_size <= 0) {
        return MY_QUEUE_ERROR;  // 参数错误
    }

    queue->data = (MY_QUEUE_DATA_TYPE *)my_queue_malloc(max_size * sizeof(MY_QUEUE_DATA_TYPE));
    if (queue->data == NULL) {
        return MY_QUEUE_ERROR;  // 内存分配失败
    }

    queue->front = 0;
    queue->rear = 0;
    queue->size = 0;
    queue->max_size = max_size;

    return MY_QUEUE_SUCCESS;  // 初始化成功
}

// 入队操作
int my_queue_enqueue(my_queue_t *queue, MY_QUEUE_DATA_TYPE data) {
    if (queue == NULL) {
        return MY_QUEUE_ERROR;  // 参数错误
    }

    if (queue->size >= queue->max_size) {
        return MY_QUEUE_FULL;  // 队列已满
    }

    queue->data[queue->rear] = data;
    queue->rear = (queue->rear + 1) % queue->max_size;
    queue->size++;

    return MY_QUEUE_SUCCESS;  // 入队成功
}

// 出队操作
MY_QUEUE_DATA_TYPE my_queue_dequeue(my_queue_t *queue, int *error) {
    if (queue == NULL || queue->size == 0) {
        if (error) *error = MY_QUEUE_EMPTY;  // 队列为空
        return (MY_QUEUE_DATA_TYPE)-1;
    }

    MY_QUEUE_DATA_TYPE data = queue->data[queue->front];
    queue->front = (queue->front + 1) % queue->max_size;
    queue->size--;

    if (error) *error = MY_QUEUE_SUCCESS;
    return data;  // 返回出队的数据
}

// 查看队列头部数据（不移除）
MY_QUEUE_DATA_TYPE my_queue_peek(my_queue_t *queue, int *error) {
    if (queue == NULL || queue->size == 0) {
        if (error) *error = MY_QUEUE_EMPTY;  // 队列为空
        return (MY_QUEUE_DATA_TYPE)-1;
    }

    if (error) *error = MY_QUEUE_SUCCESS;
    return queue->data[queue->front];  // 返回队列头部数据
}

// 重置队列
void my_queue_reset(my_queue_t *queue) {
    if (queue == NULL) {
        return;  // 参数错误
    }

    queue->front = 0;
    queue->rear = 0;
    queue->size = 0;
}

int my_queue_get_size(my_queue_t *queue, int *error) {
    if (queue == NULL) {
        if (error) *error = MY_QUEUE_ERROR;  // 参数错误
        return -1;
    }

    if (error) *error = MY_QUEUE_SUCCESS;
    return queue->size;
}

// 销毁队列
void my_queue_destroy(my_queue_t *queue) {
    if (queue == NULL) {
        return;  // 参数错误
    }

    my_queue_free(queue->data);
    queue->data = NULL;
    queue->front = 0;
    queue->rear = 0;
    queue->size = 0;
    queue->max_size = 0;
}
