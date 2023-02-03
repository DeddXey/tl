#ifndef TRANSFORMS_H
#define TRANSFORMS_H

#include "log.h"

#include <cstdint>

//----------------------------------------------------------------------------
template<typename R>
auto mul(R val)
{
  auto ret = [val](auto ran) {

    for (auto &i : ran) {
      i *= val;
    }

    return ran;
  };

  return ret;
}

//----------------------------------------------------------------------------
template<typename R>
auto offset(R val)
{
  auto ret = [val](auto ran) {
    for (auto &i : ran) {
      i += val;
    }
    return ran;
  };
  return ret;
}

//----------------------------------------------------------------------------
//auto printRange()
//{
//  auto ret = [](auto ran) {
//    for (auto i : ran) {
//      con.always() << i << Use::endl;
//    }
//    return ran;
//  };
//  return ret;
//}

#endif // TRANSFORMS_H
