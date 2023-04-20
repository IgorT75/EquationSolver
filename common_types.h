#pragma once

namespace defs {
  template<typename R, typename... T>
  using FnPtr = R(*) (T...);
  
  typedef double num_t;
  typedef const double cnum_t;
  typedef bool bool_t;
  typedef const bool cbool_t;
  
  typedef FnPtr<num_t>                  num_empty_t;
  typedef FnPtr<num_t, num_t>           num_1num_t;
  typedef FnPtr<num_t, num_t, num_t>    num_2num_t;
  typedef FnPtr<num_t, bool_t, num_t, num_t> num_2bool_t;
  typedef FnPtr<bool_t, num_t, num_t>   bool_2num_t;
  typedef FnPtr<bool_t, bool_t, bool_t> bool_2bool_t;
}