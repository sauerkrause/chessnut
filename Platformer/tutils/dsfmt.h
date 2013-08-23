/*
 * tutils/dsfmt.h
 *
 * C++ header file for
 * double precision SIMD oriented Fast Mersenne Twister (dSFMT)
 * pseudorandom number generator based on IEEE 754 format.
 *
 *
 * Original C version coded by
 * Mutsuo Saito and Makoto Matsumoto (Hiroshima University).
 * http://www.math.sci.hiroshima-u.ac.jp/~m-mat/MT/SFMT/index.html
 *
 * dSFMT-src-1.2
 * Copyright (C) 2007 Mutsuo Saito, Makoto Matsumoto and Hiroshima University.
 * All rights reserved.
 *
 *
 * This C++ version coded by Takashi Takekawa (RIKEN).
 * Copyright (C) 2007 Takashi Takekawa. All rights reserved.
 *
 * Distributed under the BSD License, see LICENSE.txt.
 */

#ifndef TUTILS_DSFMT_H
#define TUTILS_DSFMT_H
#define __SSE2__
#include "stdint.h"
#include "simd.h"

namespace tutils
{
namespace detail
{

  template <int MEXP>
    class dsfmt
    {
    public:
      static int const N = MEXP / 104;
      static int const S;
      static int const A;
      static int const B;
      static int const C;
      static int const D;
      static uint64v2_t const MSK;
      static uint64v2_t const PCV;

      explicit dsfmt(uint32_t seed = 0) { init(seed); }
      dsfmt(uint32_t const* seed, uint32_t size) { init(seed, size); }

      void init(uint32_t seed);
      void init(uint32_t const* seed, int size);

      double gen_close1_open2() {
        if (i_ == 2 * N) {
          gen_rand_all();
        }
        return reinterpret_cast<double*>(u_)[i_++];
      }
      double gen() { return gen_close1_open2() - 1.0; }

    private:
      uint64v2_t u_[N + 1];
      int i_;

      void init();
      void gen_rand_all();
    };

}

  typedef detail::dsfmt<607> dsfmt607;
  typedef detail::dsfmt<1279> dsfmt1279;
  typedef detail::dsfmt<2281> dsfmt2281;
  typedef detail::dsfmt<4423> dsfmt4423;
  typedef detail::dsfmt<11213> dsfmt11213;
  typedef detail::dsfmt<19937> dsfmt19937;
  typedef detail::dsfmt<44497> dsfmt44497;
  typedef detail::dsfmt<86243> dsfmt86243;
  typedef detail::dsfmt<132049> dsfmt132049;
  typedef detail::dsfmt<216091> dsfmt216091;

}

#endif
