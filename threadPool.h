#pragma once

#include <queue>
#include <mutex>
#include <thread>


typedef std::function<void(void)> job_function;

class ThreadPool {
public:
    ThreadPool(size_t num_threads = std::thread::hardware_concurrency());

    virtual ~ThreadPool();

    void addJob(const job_function &job);
private:
    bool                                        isRunning;

    std::queue<job_function>                    jobs;
    std::mutex                                  lock;
    std::vector<std::thread>                    threads;
    std::condition_variable                     cv;

};