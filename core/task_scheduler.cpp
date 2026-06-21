#include "TaskScheduler.hpp"
#include <spdlog/spdlog.h>

namespace Core 
{
    TaskScheduler::TaskScheduler(size_t threadCount)
    {
        if (threadCount == 0) 
        {
            threadCount = std::thread::hardware_concurrency();
        
            if (threadCount == 0) 
            {
                threadCount = 4;
            }
        }


        m_WorkerThreads.reserve(threadCount);
        for (size_t i = 0; i < threadCount; ++i) 
        {
            m_WorkerThreads.emplace_back(&TaskScheduler::WorkerLoop, this);
        }

        spdlog::info("TaskScheduler initialized with {} threads.", threadCount);
    }


    TaskScheduler::~TaskScheduler()
    {
        {
            std::lock_guard<std::mutex> lock(m_QueueMutex);
            m_bIsShuttingDown = true;
        }

        m_TaskAvailableCV.notify_all();


        for (auto& worker : m_WorkerThreads) 
        {
            if (worker.joinable()) 
            {
                worker.join();
            }
        }
    }


    void TaskScheduler::Dispatch(Task task) 
    {
        {
            std::lock_guard<std::mutex> lock(m_QueueMutex);
            m_TaskQueue.push(std::move(task));
        }

        m_RemainingTaskCount++;
        m_TaskAvailableCV.notify_one();
    }


    void TaskScheduler::DispatchAndWait(const std::vector<Task>& tasks) 
    {
        for (const auto& task : tasks) 
        {
            Dispatch(task);
        }
        WaitUntilIdle();
    }


    void TaskScheduler::WaitUntilIdle()
    {
        std::unique_lock<std::mutex> lock(m_QueueMutex);

        m_AllTasksFinishedCV.wait(lock, [this] {
            return m_RemainingTaskCount.load() == 0 && m_TaskQueue.empty();
        });
    }


    size_t TaskScheduler::GetThreadCount() const noexcept 
    {
        return m_WorkerThreads.size();
    }


    uint32_t TaskScheduler::GetQueuedTaskCount() const 
    {
        std::lock_guard<std::mutex> lock(m_QueueMutex);
        return static_cast<uint32_t>(m_TaskQueue.size());
    }


    void TaskScheduler::WorkerLoop() 
    {
        while (true) 
        {
            Task task;
            {
                std::unique_lock<std::mutex> lock(m_QueueMutex);
                
                m_TaskAvailableCV.wait(lock, [this] 
                {
                    return m_bIsShuttingDown || !m_TaskQueue.empty();
                });

                if (m_bIsShuttingDown && m_TaskQueue.empty())
                {
                    return;
                }

                if (!m_TaskQueue.empty()) 
                {
                    task = std::move(m_TaskQueue.front());
                    m_TaskQueue.pop();
                }
            }

            if (task)
            {
                task();
                m_RemainingTaskCount--;

                    // Notify threads waiting on WaitUntilIdle()
                if (m_RemainingTaskCount.load() == 0) 
                {
                    std::lock_guard<std::mutex> lock(m_QueueMutex);
                    if (m_TaskQueue.empty())
                    {
                        m_AllTasksFinishedCV.notify_all();
                    }
                }
            }
        }
    }

}
