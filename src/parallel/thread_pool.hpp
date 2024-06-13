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
        std::queue<std::function<void>> _jobs;
        std::mutex _queue_mutex;
        std::condition_variable _thread_cv;

        void workerThread();

    public:
        Threadpool(size_t numThreads = std::thread::hardware_concurrency());
        ~Threadpool();
        void addJob(std::function<void>);
};