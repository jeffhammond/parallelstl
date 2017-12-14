/*
    Copyright (c) 2017 Intel Corporation

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

        http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.




*/

#ifndef __PSTL_parallel_impl_openmp_H
#define __PSTL_parallel_impl_openmp_H

#include <atomic>

#include <omp.h>

namespace pstl {
namespace par_backend {

//! Raw memory buffer with automatic freeing and no exceptions.
/** Some of our algorithms need to start with raw memory buffer,
not an initialize array, because initialization/destruction
would make the span be at least O(N). */
class raw_buffer {
    void* ptr;
    raw_buffer(const raw_buffer&) = delete;
    void operator=(const raw_buffer&) = delete;
public:
    //! Try to obtain buffer of given size.
    raw_buffer(size_t bytes): ptr(operator new(bytes, std::nothrow)) {}
    //! True if buffer was successfully obtained, zero otherwise.
    operator bool() const { return ptr != NULL; }
    //! Return pointer to buffer, or  NULL if buffer could not be obtained.
    void* get() const { return ptr; }
    //! Destroy buffer
    ~raw_buffer() { operator delete(ptr); }
};

//------------------------------------------------------------------------
// parallel_for
//------------------------------------------------------------------------

template<class Index, class F>
void parallel_for(Index first, Index last, F f) {
}

template<class Value, class Index, typename RealBody, typename Reduction>
Value parallel_reduce(Index first, Index last, const Value& identity, const RealBody& real_body, const Reduction& reduction, std::size_t grainsize = 1) {
}

//------------------------------------------------------------------------
// parallel_transform_reduce
//
// Notation:
//      r(i,j,init) returns reduction of init with reduction over [i,j)
//      u(i) returns f(i,i+1,identity) for a hypothetical left identity element of r
//      c(x,y) combines values x and y that were the result of r or u
//------------------------------------------------------------------------

template<class Index, class U, class T, class C, class R>
T parallel_transform_reduce( Index first, Index last, U u, T init, C combine, R brick_reduce) {
}

//------------------------------------------------------------------------
// parallel_scan
//------------------------------------------------------------------------

template<class Index, class U, class T, class C, class R, class S>
T parallel_transform_scan(Index n, U u, T init, C combine, R brick_reduce, S scan) {
    return init;
}

template<typename Index>
Index split(Index m) {
    Index k = 1;
    while( 2*k<m ) k*=2;
    return k;
}

//------------------------------------------------------------------------
// parallel_strict_scan
//------------------------------------------------------------------------

template<typename Index, typename T, typename R, typename C>
void upsweep(Index i, Index m, Index tilesize, T* r, Index lastsize, R reduce, C combine) {
        if( m==1 )
            r[0] = reduce(i*tilesize, lastsize);
        else {
            Index k = split(m);
            if( m==2*k )
                r[m-1] = combine(r[k-1], r[m-1]);
        }
}

template<typename Index, typename T, typename C, typename S>
void downsweep(Index i, Index m, Index tilesize, T* r, Index lastsize, T initial, C combine, S scan) {
        if( m==1 ) {
            scan(i*tilesize, lastsize, initial );
        } else {
            Index k = split(m);
        }
}

template<typename Index, typename T, typename R, typename C, typename S, typename A>
void parallel_strict_scan( Index n, T initial, R reduce, C combine, S scan, A apex ) {
}

//------------------------------------------------------------------------
// parallel_or
//------------------------------------------------------------------------

//! Return true if brick f[i,j) returns true for some subrange [i,j) of [first,last)
template<class Index, class Brick>
bool parallel_or( Index first, Index last, Brick f ) {
    std::atomic<bool> found(false);
    return found;
}

//------------------------------------------------------------------------
// parallel_first
//------------------------------------------------------------------------

/** Return minimum value returned by brick f[i,j) for subranges [i,j) of [first,last)
    Each f[i,j) must return a value in [i,j). */
template<class Index, class Brick>
Index parallel_first( Index first, Index last, Brick f ) {
    return first;
}

//------------------------------------------------------------------------
// parallel_stable_sort
//------------------------------------------------------------------------

//------------------------------------------------------------------------
// stable_sort utilities
//
// These are used by parallel implementations but do not depend on them.
//------------------------------------------------------------------------

//! Destroy sequence [xs,xe)
struct serial_destroy {
    template<typename RandomAccessIterator>
    void operator()(RandomAccessIterator zs, RandomAccessIterator ze) {
        typedef typename std::iterator_traits<RandomAccessIterator>::value_type T;
        while(zs!=ze) {
            --ze;
            (*ze).~T();
        }
    }
};

//! Merge sequences [xs,xe) and [ys,ye) to output sequence [zs,(xe-xs)+(ye-ys)), using std::move
template<class RandomAccessIterator1, class RandomAccessIterator2, class RandomAccessIterator3, class Compare>
void serial_move_merge(RandomAccessIterator1 xs, RandomAccessIterator1 xe, RandomAccessIterator2 ys, RandomAccessIterator2 ye, RandomAccessIterator3 zs, Compare comp) {
    if(xs!=xe) {
        if(ys!=ye) {
            for(;;)
                if(comp(*ys, *xs)) {
                    *zs = std::move(*ys);
                    ++zs;
                    if(++ys==ye) break;
                }
                else {
                    *zs = std::move(*xs);
                    ++zs;
                    if(++xs==xe) goto movey;
                }
        }
        ys = xs;
        ye = xe;
    }
movey:
    std::move(ys, ye, zs);
}

template<typename RandomAccessIterator1, typename RandomAccessIterator2>
void merge_sort_init_temp_buf(RandomAccessIterator1 xs, RandomAccessIterator1 xe, RandomAccessIterator2 zs, bool inplace) {
    const RandomAccessIterator2 ze = zs + (xe-xs);
    typedef typename std::iterator_traits<RandomAccessIterator2>::value_type T;
    if(inplace)
        // Initialize the temporary buffer
        for(; zs!=ze; ++zs)
            new(&*zs) T;
    else
        // Initialize the temporary buffer and move keys to it.
        for(; zs!=ze; ++xs, ++zs)
            new(&*zs) T(std::move(*xs));
}

//! Binary operator that does nothing
struct binary_no_op {
    template<typename T>
    void operator()(T, T) {}
};

} // namespace par_backend
} // namespace pstl

#endif /* __PSTL_parallel_impl_openmp_H */
