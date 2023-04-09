#include "ThreadPool.h"

ThreadPool::ThreadPool(int _n) : stop(false){
    for(int i = 0; i < _n; ++ i)
        workers.emplace_back([this]{
            for(;;){
                std::function<void()> task;
                {
                    std::unique_lock<std::mutex> lock(mtx);
                    cv.wait(lock, [this] { return stop || !task_que.empty(); });
                    if(stop && task_que.empty())
                        return;
                    task = std::move(task_que.front());
                    task_que.pop();
                }
                task();
            }
        });
}

ThreadPool::~ThreadPool(){
    {
        std::unique_lock<std::mutex> lock(mtx);
        stop = true;
    }
    cv.notify_all();
    for(auto& work : workers)
        work.join();
}