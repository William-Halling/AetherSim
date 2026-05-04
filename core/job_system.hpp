#pragma once

#include <vector>
#include <thread>
#include <queue>
#include <functional>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <future>
#include <memory>


class JobSystem{
    public:
        explicit JobSystem(size_t threadCount = 0);
        ~JobSystem();

        JobSystem(const JobSystem&) = delete;
        JobSystem& operator=(const JobSystem&) = delete;

        using Job = std::function<void()>;

        void schedule(Job job);

        template<typename F>
        auto scheduleWithFuture(F&& f)->std::future<decltype(f())>;

        /// Block until all scheduled jobs are finished
    void waitForCompletion();

    [[nodiscard]] size_t getThreadCount() const { return m_threads.size(); }
    [[nodiscard]] uint32_t getPendingJobCount() const { return m_pendingJobs.load(); }


private:
    void workerThread();

    std::vector<std::thread> m_workerThreads;
    std::queue<Job> m_pendingJobQueue;

    mutable std::mutex m_queueMutex;
    std::condition_variable m_workAvailableCV;

    bool m_isShuttingDown = false;
    std::atomic<uint32_t> m_activeJobCount{0};
};



// Template implementation
template<typename F>
auto JobSystem::scheduleWithFuture(F&& f) -> std::future<decltype(f())>
{
    using ReturnType = decltype(f());

    auto promise = std::make_shared<std::promise<ReturnType>>();
    auto future = promise->get_future();

    schedule([promise = std::move(promise), f = std::forward<F>(f)]() mutable {
        try {
            if constexpr (std::is_void_v<ReturnType>)
            {
                f();
                promise->set_value();
            } else {
                promise->set_value(f());
            }
        } catch (...) {
            promise->set_exception(std::current_exception());
        }
    });

    return future;
}