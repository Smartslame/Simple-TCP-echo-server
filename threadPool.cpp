#include "threadPool.h"

ThreadPool::ThreadPool(size_t num_threads) : isRunning(true) {
    auto thread_loop = [&](size_t id) {
        while (isRunning) {
            std::unique_lock<std::mutex> lock_guard(lock);
            cv.wait(lock_guard, [this]{
                return (!jobs.empty());
            });

            if (!jobs.empty()) {
                auto func   = std::move(jobs.front());
                jobs.pop();
                lock_guard.unlock();
                func();
                lock_guard.lock();
            }
        }
    };

    threads.reserve(num_threads);
    for (size_t i = 0; i < num_threads; ++i) {
        threads.emplace_back(std::thread(thread_loop, i));
    }
}

ThreadPool::~ThreadPool() {
    std::unique_lock<std::mutex> lock_guard(lock);
    isRunning = false;
    while(!jobs.empty()) {
        jobs.pop();
    }
    lock_guard.unlock();
    cv.notify_all();
    for (auto &t : threads) {
        t.detach();
    }
}

void ThreadPool::addJob(job_function job) {
    std::unique_lock<std::mutex> lock_guard(lock);
    jobs.push(job);
    lock_guard.unlock();
    cv.notify_all();
}