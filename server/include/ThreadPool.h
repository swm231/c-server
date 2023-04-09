#ifndef _THREAD_POOL_H_
#define _THREAD_POOL_H_

#include <vector>
#include <queue>
#include <functional>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <future>

class ThreadPool{
private:
    std::vector<std::thread> workers;
    std::queue<std::function<void()> > task_que;
    std::mutex mtx;
    std::condition_variable cv;
    bool stop;
public:
    explicit ThreadPool(int);
    ~ThreadPool();

    template <class F, class... Args>
    auto add(F&& f, Args&&... args) -> std::future<decltype(f(args...))>;
};

template <class F, class... Args>
auto ThreadPool::add(F&& f, Args&&... args) -> std::future<decltype(f(args...))>
{
    auto task_ptr = std::make_shared<std::packaged_task<decltype(f(args...))()> >(
        std::bind(std::forward<F>(f), std::forward<Args>(args)...)
    );
    {
        std::unique_lock<std::mutex> lock(mtx);
        if(stop)
            throw std::runtime_error("add task on stopped thread_pool");
        task_que.push([task_ptr]{ (*task_ptr)(); });
    }
    cv.notify_one();
    return task_ptr->get_future();
}

#endif