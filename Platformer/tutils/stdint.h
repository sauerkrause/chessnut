#ifndef TUTILS_STDINT_H
#define TUTILS_STDINT_H

namespace tutils
{

  template <bool B, typename T1, typename T2>
    struct if_
    {
      typedef T1 type;
    };

  template <typename T1, typename T2>
    struct if_<false, T1, T2>
    {
      typedef T2 type;
    };

  typedef if_<sizeof(signed char) == 1, signed char, void>::type int8_t;
  typedef if_<sizeof(unsigned char) == 1, unsigned char, void>::type uint8_t;

  typedef if_<sizeof(short) == 2, short, void>::type int16_t;
  typedef if_<sizeof(unsigned short) == 2, unsigned short, void>::type uint16_t;

  typedef if_<sizeof(int) == 4, int,
          if_<sizeof(long) == 4, long,
          void>::type>::type int32_t;
  typedef if_<sizeof(unsigned) == 4, unsigned,
          if_<sizeof(unsigned long) == 4, unsigned long,
          void>::type>::type uint32_t;

  typedef if_<sizeof(long) == 8, long,
          if_<sizeof(long long) == 8, long long,
          void>::type>::type int64_t;
  typedef if_<sizeof(unsigned long) == 8, unsigned long,
          if_<sizeof(unsigned long long) == 8, unsigned long long,
          void>::type>::type uint64_t;

}

#endif
