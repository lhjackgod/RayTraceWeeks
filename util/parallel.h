#ifndef PARALLEL_H
#define PARALLEL_H
#include <thread>
#include <mutex>
#include <condition_variable>
#include "rtweekend.h"
#include "vecmath.h"
#include <stdio.h>
#include <cassert>
inline int AvaliableCores()
{
    return std::max<int>(1, std::thread::hardware_concurrency());
}

class ParallelJob;

class ThreadPool
{
public:
    explicit ThreadPool(int nThreads);
    ~ThreadPool();
    size_t size() const { return threads.size(); }
    
    std::unique_lock<std::mutex> AddToJobList(ParallelJob* job);
    void RemoveFromJobList(ParallelJob* job);

    void WorkOrWait(std::unique_lock<std::mutex>* lock, bool isEnqueuingThread);
private:
    void Worker();
private:
    std::vector<std::thread> threads;
    ParallelJob* jobLists = nullptr;
    mutable std::mutex mutex;
    bool shutdownThreads = false;
    bool disabled = false;
    std::condition_variable condition;
};

class ParallelJob
{
public:
    virtual ~ParallelJob() { assert(removed && "ParallelJob is being destroyed without being removed!"); }
    virtual bool HaveWork() const = 0;
    virtual void RunStep(std::unique_lock<std::mutex>* lock) = 0;
    bool Finished() const { return !HaveWork() && activeWorkers == 0; }

    virtual std::string ToString() const = 0;

    static ThreadPool* threadPool;
protected:
    std::string BasicToString() const 
    {
        char resString[256];
        sprintf(resString, "activeWorkers: %d removed: %s", activeWorkers, removed);
        return std::string(resString);
    }
private:
    friend class ThreadPool;
    bool removed = false;
    int activeWorkers = 0;
    ParallelJob* prev = nullptr, *next = nullptr;
};

class ParallelForLoop2D : public ParallelJob
{
public:
    ParallelForLoop2D(const Bounds2i& extent, int chunkSize, 
    std::function<void(Bounds2i)> func) 
    : extent(extent) ,
      nextStart(extent.pMin),
      chunkSize(chunkSize),
      func(std::move(func))
    {}
    virtual std::string ToString() const override
    {
        return BasicToString();
    }
    virtual bool HaveWork() const override { return nextStart.y < extent.pMax.y; }
    virtual void RunStep(std::unique_lock<std::mutex>* lock) override;
private:
    std::function<void(Bounds2i)> func;
    const Bounds2i extent;
    Point2i nextStart;
    int chunkSize;
};

inline int RunningThreads()
{
    return ParallelJob::threadPool ? (1 + ParallelJob::threadPool->size()) : 1;
}

inline void ParallelFor2D(const Bounds2i& extent, const std::function<void(const Bounds2i)>& func)
{
    if(extent.IsEmpty())
    {
        return;
    }
    else if(extent.Area() == 1)
    {
        func(extent);
    }

    int tileSize = std::clamp((int)(
        extent.Diagonal().x * extent.Diagonal().y / (8 * RunningThreads())), 1, 32
    );
    ParallelForLoop2D loop(extent, tileSize, std::move(func));
    std::unique_lock<std::mutex> lock = ParallelJob::threadPool->AddToJobList(&loop);

    while(!loop.Finished())
    {
        ParallelJob::threadPool->WorkOrWait(&lock, true);
    }
} 

inline void ParallelFor2D(const Bounds2i& extent, const std::function<void(Point2i)>& func)
{
    ParallelFor2D(extent, [&](const Bounds2i& b)
    {
        for(const Point2i& p : b)
        {
            func(p);
        }
    });
}
#endif