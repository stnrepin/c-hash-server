#ifndef THREAD_POOL_H
#define THREAD_POOL_H

typedef void (*TaskAction)(void *);
typedef struct {

} ThreadPoolTask;

typedef struct {

} ThreadPool;

void ThreadPool_init(ThreadPool *pool, int thread_count);
void ThreadPool_run_task(ThreadPool *pool, TaskAction ta, void *arg);
void ThreadPool_deinit(ThreadPool *pool);

#endif // !THREAD_POOL_H