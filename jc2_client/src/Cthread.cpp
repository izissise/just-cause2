#include "Cthread.h"

CThread::CThread(void (*func)(void*), void* data)
{
  m_func = func;
  m_threadExited = false;
  m_pause = PAUSE;
  m_data = data;
}

CThread::~CThread()
{
  CloseHandle(m_thread);
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
  m_thread = CreateThread(NULL, 0,
                          &CThread::threadHandleFunc, (void*)this, 0, NULL);
  if (m_thread == NULL)
    {
      this->SetPause(PAUSE);
      this->SetExited(true);
    }
}

DWORD WINAPI CThread::threadHandleFunc(void* obj)
{
  CThread* thread = reinterpret_cast<CThread*>(obj);
  void (*func)(void*) = thread->GetFunc();
  void* data = thread->GetData();

  while(1)
    {
      if(thread->GetPause() == PAUSE)
        Sleep(10);
      else
        {
          thread->SetState(BUSY);
          func(data);
        }
      thread->SetState(IDLE);
    }
  thread->SetExited(true);
  return (0);
}
