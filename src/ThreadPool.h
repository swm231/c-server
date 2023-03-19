#ifndef _THREADPOOL_H_
#define _THREADPOOL_H_

#include <vector>
#include <queue>
#include <thread>
#include <functional>
#include <mutex>
#include <condition_variable>
#include <future>

class ThreadPool{
private:
    std::vector<std::thread> threads;
    std::queue<std::function<void()> > tasks;
    std::mutex mtx;
    std::condition_variable cv;
    bool stop;
public:
    ThreadPool(int size = 10);
    ~ThreadPool();
    template <class F, class... Args>
    auto add(F &&f, Args&&... args) 
        -> std::future<typename std::result_of<F(Args...)>::type>;
};

template <class F, class... Args>
auto ThreadPool::add(F &&f, Args&&... args) -> std::future<typename std::result_of<F(Args...)>::type>{
    using return_type = typename std::result_of<F(Args...)>::type;
    auto task = std::make_shared<std::packaged_task<return_type()> >(
        std::bind(std::forward<F>(f), std::forward<Args>(args)...)
    );
    std::future<return_type> res = task->get_future();
    {
        std::unique_lock<std::mutex> lock(mtx);
        if(stop)
            throw std::runtime_error("enqeue on stopped ThreadPool");
        tasks.emplace([task](){(*task)();});
    }
    cv.notify_one();
    return res;
}

#endif