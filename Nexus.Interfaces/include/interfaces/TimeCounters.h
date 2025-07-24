#pragma once

#include <atomic>

struct  ServerMetaDataTimer
{
  const	int compare_general_work  = 1 * 12 * 5;
  const int compare_work  = 6;
  const int compare_initialization_ = 6;
  const int compare_work_send_count_ = 5;

  std::atomic<int> _timeGeneralWork { 0 };
  std::atomic<int> _timeWork { 0 };
  std::atomic<int> _timeInitialization { 0 };
  std::atomic<int> _workSendCount { 0 };
  // использование   _timeGeneralWork += 1;

  void reset() {
    _timeGeneralWork.store(0);
    _timeWork.store(0);
    _timeInitialization.store(0);
    _workSendCount.store(0);
  }
  void reset_timeGeneralWorke() { _timeGeneralWork.store(0); }
  void reset_timeWork() { _timeWork.store(0); }
  void reset_timeInitialization() { _timeInitialization.store(0); }
  void reset_workSendCount() { _workSendCount.store(0); }

};

//struct ITimeCounters
//{
//  std::atomic<int> wait_response_count_{ 0 };
//  std::atomic<int> work_module_{ 0 };
//  std::atomic<int> reset_channel_{ 0 };
//  std::atomic<int> wait_3_{ 0 };
//
//
//  virtual void reset() = 0; 
//
//  virtual  void event_timer() = 0;
//
//  virtual void reset_wait_response() = 0;
//  virtual void reset_work_module() = 0;
//  virtual void reset_channel() = 0;
//  virtual void reset_wait_3() = 0;
//
//};
//
//struct TimeCounters:public ITimeCounters {
////  std::atomic<int> wait_response_count_{ 0 };
////  std::atomic<int> work_module_{ 0 };
////  std::atomic<int> reset_channel_{ 0 };
////  std::atomic<int> wait_3_{ 0 };
//
//  TimeCounters() { TimeCounters::reset(); }
//
//  void reset() override {
//    wait_response_count_.store(0);
//    work_module_.store(0);
//    reset_channel_.store(0);
//    wait_3_.store(0);
//  }
//
//  void event_timer() override {
//    wait_response_count_ += 1;
//    work_module_ += 1;
//    reset_channel_ += 1;
//    wait_3_ += 1;
//  }
//
//  void reset_wait_response() override { wait_response_count_.store(0); }
//  void reset_work_module() override { work_module_.store(0); }
//  void reset_channel() override { reset_channel_.store(0); }
//  void reset_wait_3()override { wait_3_.store(0); }
//};
