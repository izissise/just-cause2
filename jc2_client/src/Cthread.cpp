#include "Cthread.h"

CThread::CThread(void (*func)(void*), void* data)
{
  m_func = func;
  m_threadExited = false;
  m_pause = PAUSE;
  m_data = data;
  m_created = false;
}

CThread::~CThread()
{
  this->SetState(STOP);
  pthread_cancel(m_thread);
}

void CThread::waitThreadIDLE()
{
  int tmpPause;

  tmpPause = this->GetPause();
  this->SetPause(PAUSE);
  while (this->GetState() != IDLE) // We wait till thread is idle
    {}
  this->SetPause(tmpPause);
}

void CThread::startThread()
{
  this->SetPause(RUN);
  if (!m_created)
    if (!pthread_create(&m_thread, NULL, &CThread::threadHandleFunc, (void*)this)) //create thread
      m_created = true;
    else
      {
        this->SetExited(true);
        this->SetState(STOP);
        this->SetPause(PAUSE);
      }
}

void* CThread::threadHandleFunc(void* obj)
{
  CThread* thread = reinterpret_cast<CThread*>(obj);
  void (*func)(void*) = thread->GetFunc();
  void* data;

  do
    {
      if(thread->GetPause() == PAUSE)
        {
          Sleep(10);
        }
      else
        {
          thread->SetState(BUSY);
          data = thread->GetData();
          func(data);
        }
      thread->SetState(IDLE);
    }
  while(thread->GetState() != STOP);
  thread->SetExited(true);
  return (NULL);
}
