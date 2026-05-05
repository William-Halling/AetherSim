#pragma once

#include <queue>
#include <vector>
#include <thread>
#include <functional>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <memory>
#include <future>


namespace Core{

    /**
     * @class TaskScheduler
     * @brief A high-performance thread pool for dispatching asynchronous units of work.
     * 
     * This system manages a fixed pool of worker threads and a synchronized task queue.
     * It is non-copyable and non-movable to ensure singleton-like stability within the engine core.
     */
class TaskScheduler{
    public:
        using Task = std::function<void()>;

        /**
         * @brief Initializes the scheduler and spawns worker threads.
         * @param threadCount The number of threads to spawn. If 0, defaults to hardware concurrency.
         */
    explicit TaskScheduler(size_t threadCount = 0);


        /**
         * @brief Signals shutdown and joins all worker threads.
         */
    ~TaskScheduler();

        // Disable copy and move semantics for architectural integrity
    TaskScheduler(const TaskScheduler&) = delete;
    TaskScheduler& operator=(const TaskScheduler&) = delete;
    TaskScheduler(TaskScheduler&&) = delete;
    TaskScheduler& operator=(TaskScheduler&&) = delete;


        /**
         * @brief Adds a single task to the execution queue.
         * @param task A functional unit of work to execute.
         */
    void Dispatch(Task task);


        /**
         * @brief Adds a batch of tasks and blocks until they are all completed.
         * @param tasks A vector of functional units of work.
         */
    void DispatchAndWait(const std::vector<Task>& tasks);

        /**
         * @brief Dispatches a task and returns a future to retrieve its result later.
         * @tparam F The callable type.
         * @param f The function to execute.
         * @return A std::future containing the eventual result of the function.
         */
    template<typename F>
    auto DispatchWithFuture(F&& f) -> std::future<decltype(f())>;

        /**
         * @brief Blocks the calling thread until the task queue is empty and all workers are idle.
         */
    void WaitUntilIdle();


    /** @return The number of worker threads in the pool. */
    [[nodiscard]] size_t GetThreadCount() const noexcept;


    /** @return The number of tasks currently waiting in the queue. */
    [[nodiscard]] uint32_t GetQueuedTaskCount() const;


    private:
    /** @brief The entry point for each worker thread. */
    void WorkerLoop();

    std::queue<Task>          m_TaskQueue;
    std::vector<std::thread>  m_WorkerThreads;

    mutable std::mutex       m_QueueMutex;
    std::condition_variable  m_TaskAvailableCV;
    std::condition_variable  m_AllTasksFinishedCV;

    bool                     m_bIsShuttingDown{false};
    std::atomic<uint32_t>    m_RemainingTaskCount{0};
};


// --- Template Implementation ---

template<typename F>
auto TaskScheduler::DispatchWithFuture(F&& f) -> std::future<decltype(f())> {
    using ReturnType = decltype(f());

    auto promise = std::make_shared<std::promise<ReturnType>>();
    auto future = promise->get_future();


    Dispatch([promise, f = std::forward<F>(f)]() mutable {
        try {
            if constexpr (std::is_void_v<ReturnType>) {
                f();
                promise->set_value();
            }
            else 
            {
                promise->set_value(f());
            }
        } 
        catch (...)
        {
            promise->set_exception(std::current_exception());
        }
    });

    return future;
}

} // namespace Core