#ifndef CTHREAD_H_INCLUDED
#define CTHREAD_H_INCLUDED

#define BUSY 1
#define IDLE 0
#define PAUSE 1
#define RUN 0

#include <windows.h>

class CThread
{
public:
  CThread(void func(void*), void* data);
  ~CThread();

  int GetState() {return m_state;} //Return thread status
  int GetPause() {return m_pause;} //Return pause status
  int GetExited() {return m_threadExited;}
  void (*GetFunc())(void*) {return m_func;}
  void* GetData() {return m_data;}
  void SetPause(int pause) {m_pause = pause;}
  void SetState(int state) {m_state = state;}
  void SetExited(bool exit) {m_threadExited = exit;}

  void startThread();
  void waitThreadIDLE();

private:
  static DWORD WINAPI threadHandleFunc(void* obj);

  HANDLE m_thread;
  int m_pause; //pause state
  int m_state; //thread state
  bool m_threadExited; //thread terminated
  void (*m_func)(void*); //threaded func ptr
  void* m_data;
};

#endif // CTHREAD_H_INCLUDED
