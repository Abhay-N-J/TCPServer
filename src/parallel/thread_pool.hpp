#pragma once

#include <iostream>
#include <thread>
#include <mutex>
#include <queue>
#include <vector>
#include <functional>
#include <condition_variable>

class Threadpool {
    private:
        std::vector<std::thread> _threads;
        std::queue<std::function<void()>> _jobs;
        std::mutex _queue_mutex;
        std::condition_variable _thread_cv;
        bool stop;

        void workerThread();

    public:
        Threadpool();
        Threadpool(size_t);
        ~Threadpool();
        void addJob(std::function<void()>);
};