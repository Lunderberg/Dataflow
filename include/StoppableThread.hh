#ifndef _STOPPABLETHREAD_H_
#define _STOPPABLETHREAD_H_

#include <atomic>
#include <thread>

class StoppableThread {
public:
  StoppableThread()
    : running(false) { }

  virtual ~StoppableThread(){
    Stop();
  }

  void Start(){
    if(!running){
      running = true;
      thread = std::thread(&StoppableThread::RunLoop, this);
    }
  }

  void Stop(){
    if(running){
      running = false;
      thread.join();
    }
  }

private:
  virtual void Iteration() = 0;

  void RunLoop(){
    while(running){
      Iteration();
    }
  }

  std::thread thread;
  std::atomic_bool running;
};

#endif /* _STOPPABLETHREAD_H_ */
