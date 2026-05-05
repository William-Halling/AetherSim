#include "core/job_system.hpp"
#include <spdlog/spdlog.h>

JobSystem::JobSystem(size_t threadCount)
{
    if (threadCount == 0) {

        threadCount = std::thread::hardware_concurrency();
        
        if (threadCount == 0) {
            
            threadCount = 4;
        }
    }


    m_workerThreads.reserve(threadCount);
    for (size_t i = 0; i < threadCount; ++i) {

        m_workerThreads.emplace_back([this] { workerThread(); });
    }

    spdlog::info("JobSystem started with {} worker threads", threadCount);
}


JobSystem::~JobSystem()
{
    {
        std::lock_guard<std::mutex> lock(m_queueMutex);
        m_isShuttingDown = true;
    }
    
    m_workAvailableCV.notify_all();

    
    for (auto& thread : m_workerThreads) 
    {
        if (thread.joinable()) {

            thread.join();
        }
    }
}


void JobSystem::schedule(Job job)
{
    {
        std::lock_guard<std::mutex> lock(m_queueMutex);
        m_pendingJobQueue.push(std::move(job));
        m_activeJobCount++;
    }
    m_workAvailableCV.notify_one();
}



void JobSystem::waitForCompletion()
{
    while (m_activeJobCount > 0) {
        std::this_thread::yield();
    }
}


void JobSystem::workerThread()
{
    while (true) 
    {
        Job job;
        {
            std::unique_lock<std::mutex> lock(m_queueMutex);
            
            m_workAvailableCV.wait(lock, [this] {

                return m_isShuttingDown || !m_pendingJobQueue.empty(); 
            });


            if (m_isShuttingDown && m_pendingJobQueue.empty()) {

                return;
            }

            if (!m_pendingJobQueue.empty()) {

                job = std::move(m_pendingJobQueue.front());
                m_pendingJobQueue.pop();
            }
        }


        if (job) {

            job();
            m_activeJobCount--;
        }
    }
}