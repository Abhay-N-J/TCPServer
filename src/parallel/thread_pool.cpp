#include <iostream>
#include <thread>
#include <mutex>
#include <queue>
#include <vector>
#include <functional>
#include <condition_variable>

#include "thread_pool.hpp"

Threadpool::Threadpool() : stop(false) {
    size_t numThreads = std::thread::hardware_concurrency();
    for (int i = 0; i < numThreads; i++)
        _threads.emplace_back(&Threadpool::workerThread, this);
}

Threadpool::Threadpool(size_t numThreads) : stop(false) {
    for (int i = 0; i < numThreads; i++)
        _threads.emplace_back(&Threadpool::workerThread, this);
}

Threadpool::~Threadpool() {
    stop = false;
    _thread_cv.notify_all();

    for (std::thread &t: _threads)
        t.join();
}

void Threadpool::addJob(std::function<void()> f) {
    {
        std::unique_lock<std::mutex> lock(_queue_mutex);
        _jobs.emplace(f);
    }
    _thread_cv.notify_one();
}

void Threadpool::workerThread() {
    while (true) {
        std::function<void()> job;
        {
            std::unique_lock<std::mutex> lock(_queue_mutex);
            _thread_cv.wait(lock, [this] {
                return this->stop || !this->_jobs.empty();
            });
            if (stop && !_jobs.empty())
                return;
            job = std::move(_jobs.front());
            _jobs.pop();
        }
        job();
    }
}



