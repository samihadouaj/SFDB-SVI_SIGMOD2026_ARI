/**
 * This code was adapted from https://stackoverflow.com/a/13194652/10110639
**/
#ifndef ATOMWRAPPER
#define ATOMWRAPPER
#include <atomic>
#include <vector>

template <typename T>
struct atomwrapper
{
  std::atomic<T> _a;

  atomwrapper()
    :_a()
  {}

inline T get_a() const
{return _a.load(std::memory_order_relaxed);}

  atomwrapper(const std::atomic<T> &a)
    :_a(a.load(std::memory_order_relaxed))
  {}


// Copy constructor
  atomwrapper(const atomwrapper &other)
    :_a(other._a.load(std::memory_order_relaxed))
  {}

  atomwrapper &operator=(const atomwrapper &other)
  {
    _a.store(other._a.load(std::memory_order_relaxed),std::memory_order_relaxed);
  }

  inline void storeWrapper(const T& val) const
  {
    _a.store(val,std::memory_order_relaxed);
  }


 inline void fetch_add(const T& val) 
  {
    _a.fetch_add(val,std::memory_order_relaxed);
  }


// inline bool compare_exchange_weak(T expected,  T desired)
//   {
//     return _a.compare_exchange_weak(expected,desired,std::memory_order_relaxed);
//   }

// bool compare_exchange_strong( const T expected,const T desired)
//   {
//     return _a.compare_exchange_strong(expected,desired,std::memory_order_relaxed);
//   }

// T load(const atomwrapper other)
// {
//   return other._a(load(std::memory_order_relaxed));
// }

};



#endif 