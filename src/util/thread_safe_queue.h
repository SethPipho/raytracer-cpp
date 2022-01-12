#ifndef THREAD_SAFE_QUEUE_H
#define THREAD_SAFE_QUEUE_H

#include <queue>
#include <mutex>
#include <condition_variable>

template <typename T>
class ThreadSafeQueue {
  
  public:  
    void push(T const &v){
      std::lock_guard<std::mutex> guard(queue_mutex);
      queue.push(v);
      pop_con.notify_one();
    }
    bool pop(T &v){
      {
        std::unique_lock<std::mutex> lock(queue_mutex);
        while(queue.empty() && !_closed){
          pop_con.wait(lock);
        }
        if (queue.empty() && _closed){
          return false;
        }
        v = queue.front();
        queue.pop();
        return true;
      }
    }
    void close(){
      std::lock_guard<std::mutex> guard(queue_mutex);
      _closed = true;
      pop_con.notify_all();
    }

    bool empty(){
      return queue.empty();
    }

    bool closed(){
        return _closed;
    }

  private:
    std::queue<T> queue;
    bool _closed = false;
    std::mutex queue_mutex;
    std::condition_variable pop_con;
};


#endif