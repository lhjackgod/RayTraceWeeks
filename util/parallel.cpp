#include "parallel.h"

ThreadPool::ThreadPool(int nThreads)
{
    for(int i = 0; i < nThreads - 1; i++)
    {
        threads.push_back(std::thread(&ThreadPool::Worker, this));
    }
}

ThreadPool *ParallelJob::threadPool = new ThreadPool(AvaliableCores());
void ThreadPool::Worker()
{
    std::unique_lock<std::mutex> lock(mutex);
    while(!shutdownThreads)
    {
        WorkOrWait(&lock, false);
    }
}

std::unique_lock<std::mutex> ThreadPool::AddToJobList(ParallelJob* job)
{
    std::unique_lock<std::mutex> lock(mutex);

    if(jobLists)
    {
        jobLists->prev = job;
    }
    job->next = jobLists;
    jobLists = job;

    condition.notify_all();
    return lock;
}

void ThreadPool::WorkOrWait(std::unique_lock<std::mutex>* lock, bool isEnqueuingThread)
{
    if(!lock->owns_lock())
    {
        assert("lock is not owned by current thread");
        exit(-1);
    }
    // Return if this is a worker thread and the thread pool is disabled
    if(!isEnqueuingThread && disabled)
    {
        condition.wait(*lock); //release lock and block itself, waiting for notify
        return;
    }
    ParallelJob* job = jobLists;
    while(job && !job->HaveWork())
    {
        job = job->next;
    }
    if(job)
    {
        job->activeWorkers++;
        job->RunStep(lock);

        if(lock->owns_lock())
        {
            assert("you need to release lock before return");
        }
        lock->lock();
        job->activeWorkers--;
        if(job->Finished())
        {
            condition.notify_all();
        }
    }
    else
    {
        condition.wait(*lock);
    }
}

ThreadPool::~ThreadPool()
{
    if(threads.empty())
    {
        return;
    }
    {
        std::lock_guard<std::mutex> lock(mutex);
        shutdownThreads = true;
        condition.notify_all();
    }
    for(auto& thread : threads)
    {
        thread.join();
    }
}

void ThreadPool::RemoveFromJobList(ParallelJob* job)
{
    if(job->prev)
    {
        job->prev->next = job->next;
    }
    else{
        jobLists = job->next;
    }
    if(job->next)
    {
        job->next->prev = job->prev;
    }
    job->removed = true;
}

void ParallelForLoop2D::RunStep(std::unique_lock<std::mutex>* lock)
{
    Point2i end = nextStart + Vector2i(chunkSize, chunkSize);

    Bounds2i b = Intersect(Bounds2i(nextStart, end), extent);
    if(b.IsEmpty())
    {
        assert("bounds is empty");
        return;
    }
    nextStart.x += chunkSize;
    if(nextStart.x >= extent.pMax.x)
    {
        nextStart.x = extent.pMin.x;
        nextStart.y += chunkSize;
    }
    if(!HaveWork())
    {
        threadPool->RemoveFromJobList(this);
    }
    lock->unlock();
    func(b);
}