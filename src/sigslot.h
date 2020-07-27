#ifndef _SIGNAL_SLOT_H_
#define _SIGNAL_SLOT_H_

#include <functional>
#include <memory>
#include <list>
#include <mutex>

namespace sigslot {

namespace detail {

template<typename Callback>
class SignalCommon
{
 public:
  typedef std::list<Callback> CB_LIST;

  SignalCommon()
    :slot_list_(new CB_LIST)
  {}

  void copyOnWrite()
  {
    if (!slot_list_.unique())
    {
      slot_list_.reset(new CB_LIST(*slot_list_));
    }
  }

 public:
  std::shared_ptr<CB_LIST> slot_list_;
  std::mutex mutex_;
};

} //namespace detail

template<typename... ARGS>
class Signal
{
 public:
  typedef std::function<void(ARGS...)> Callback;
  typedef detail::SignalCommon<Callback> SignalCommonType;

  Signal()
    :com_(new SignalCommonType)
  {}
  ~Signal()
  {}

  void connect(Callback f)
  {
    add(f);
  }

  template<typename CLASS>
  void connect(void (CLASS::*function)(ARGS...), std::shared_ptr<CLASS>& object)
  {
    std::weak_ptr<CLASS> weak_obj = object;
    auto f = [=] (ARGS&&... args) mutable -> void {
        std::shared_ptr<CLASS> ptr(weak_obj.lock());
        if (ptr)
        {
          ((ptr.get())->*function)(std::forward<ARGS>(args)...);
        }
    };
    add(f);
  }

  template<typename CLASS>
  void connect(void (CLASS::*function)(ARGS...) const, std::shared_ptr<CLASS>& object)
  {
    std::weak_ptr<CLASS> weak_obj = object;
    auto f = [=] (ARGS&&... args) mutable -> void {
        std::shared_ptr<CLASS> ptr(weak_obj.lock());
        if (ptr)
        {
          ((ptr.get())->*function)(std::forward<ARGS>(args)...);
        }
    };
    add(f);
  }

  void invoke(ARGS&&... args)
  {
    SignalCommonType& com(*com_);
    std::shared_ptr<typename SignalCommonType::CB_LIST> slot_list;
    {
      std::lock_guard<std::mutex> lock(com.mutex_);
      slot_list = com.slot_list_;
    }

    typename SignalCommonType::CB_LIST& cb_list(*slot_list);
    for (auto &cb: cb_list)
    {
      cb(std::forward<ARGS>(args)...);
    }
  }

  void operator()(ARGS&&... args)
  {
    invoke(std::forward<ARGS>(args)...);
  }

 private:
  void add(Callback f)
  {
    SignalCommonType& com(*com_);
    {
      std::lock_guard<std::mutex> lock(com.mutex_);
      com.copyOnWrite();
      com.slot_list_->push_back(f);
    }
  }
  
 private:
  std::shared_ptr<SignalCommonType> com_;
};


} //namespace sigslot

#endif