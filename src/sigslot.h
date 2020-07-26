#ifndef _SIGNAL_SLOT_H_
#define _SIGNAL_SLOT_H_

#include <functional>
#include <memory>
#include <list>
#include <mutex>

namespace sigslot {

namespace detail {
template<typename CLASS, typename... ARGS>
class WeakCallback
{
 public:

  WeakCallback(const std::weak_ptr<CLASS>& object,
               const std::function<void (CLASS*, ARGS...)>& function)
    : object_(object), function_(function)
  {
  }

  // Default dtor, copy ctor and assignment are okay

  void operator()(ARGS&&... args) const
  {
    std::shared_ptr<CLASS> ptr(object_.lock());
    if (ptr)
    {
      function_(ptr.get(), std::forward<ARGS>(args)...);
    }
  }

 private:

  std::weak_ptr<CLASS> object_;
  std::function<void (CLASS*, ARGS...)> function_;
};

template<typename CLASS, typename... ARGS>
WeakCallback<CLASS, ARGS...> makeWeakCallback(const std::shared_ptr<CLASS>& object,
                                              void (CLASS::*function)(ARGS...))
{
  return WeakCallback<CLASS, ARGS...>(object, function);
}

template<typename CLASS, typename... ARGS>
WeakCallback<CLASS, ARGS...> makeWeakCallback(const std::shared_ptr<CLASS>& object,
                                              void (CLASS::*function)(ARGS...) const)
{
  return WeakCallback<CLASS, ARGS...>(object, function);
}

} //namespace detail

template<typename CLASS, typename... ARGS>
class Signal
{
 public:
  Signal() {}
  ~Signal()
  {
    weakCallbackList_.clear();
  }

  void connect(const std::shared_ptr<CLASS>& object, void (CLASS::*function)(ARGS...))
  {
    weakCallbackList_.push_back(detail::makeWeakCallback(object, function));
  }

  void invoke(ARGS&&... args)
  {
    for (auto &cb : weakCallbackList_)
    {
        cb(std::forward<ARGS>(args)...);
    }
  }
  
 private:
  std::list<detail::WeakCallback<CLASS, ARGS...>> weakCallbackList_;
};


} //namespace sigslot

#endif