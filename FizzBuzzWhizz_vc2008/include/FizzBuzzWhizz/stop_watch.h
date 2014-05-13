/*
    Copyright 2005-2011 Intel Corporation.  All Rights Reserved.

    This file is part of Threading Building Blocks.

    Threading Building Blocks is free software; you can redistribute it
    and/or modify it under the terms of the GNU General Public License
    version 2 as published by the Free Software Foundation.

    Threading Building Blocks is distributed in the hope that it will be
    useful, but WITHOUT ANY WARRANTY; without even the implied warranty
    of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Threading Building Blocks; if not, write to the Free Software
    Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

    As a special exception, you may use this file as part of a free software
    library without restriction.  Specifically, if other files instantiate
    templates or use macros or inline functions from this file, or you compile
    this file and link it with other files to produce an executable, this
    file does not by itself cause the resulting executable to be covered by
    the GNU General Public License.  This exception does not however
    invalidate any other reasons why the executable file might be covered by
    the GNU General Public License.
*/

#ifndef _ANNLAB_STOP_WATCH_H_
#define _ANNLAB_STOP_WATCH_H_

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#ifdef _MSC_VER
#include <FizzBuzzWhizz/vs/stdint.h>
#else
#include <stdint.h>
#endif

#if _WIN32 || _WIN64
#include <windows.h>
#include <time.h>
#elif __linux__
#include <ctime>
#else /* generic Unix */
#include <sys/time.h>
#endif /* (choice of OS) */

namespace annlab {

///////////////////////////////////////////////////////////////////////////
// stop_watch_base<T>
///////////////////////////////////////////////////////////////////////////

//! Absolute timestamp
/** @ingroup timing */
template<class T>
class stop_watch_base
{
public:
    typedef typename int64_t timestamp_t;

public:
    //! Construct an absolute timestamp initialized to zero.
    stop_watch_base() : bIsRunning(false), startTime(0), stopTime(0), elapsedTime(0) {};
    stop_watch_base(const stop_watch_base &src);

    stop_watch_base &operator = (const stop_watch_base &t);

    bool    isRunning(void) { return bIsRunning; };

    void    clear(void);
    void    reset(void);

    //! restart() is equivalent to reset() and begin()
    void    restart(void);
    void    rebegin(void);

    void    start(void);
    void    stop(void);

    void    begin(void);
    void    end(void);

    //! Return current time.
    static timestamp_t  now(void);

    //! Return current time(double).
    static double       nowf(void);

    static double       intervalSeconds(int64_t t1, int64_t t2);
    static double       intervalSeconds(double t1, double t2);

    //! Return current time(millisecs).
    static timestamp_t  currentTimeMillis(void);
    static double       currentTimeMillisf(void);

    timestamp_t getStartTime(void) { return startTime; };
    timestamp_t getStopTime(void)  { return stopTime;  };

    double  getSeconds(void);
    double  getMillisec(void);
    double  getElapsedTime(void);

protected:
    void    native_start();
    void    native_stop();

    static timestamp_t  native_now();
    static double       native_nowf();

protected:
    bool        bIsRunning;
    timestamp_t startTime, stopTime;
    timestamp_t elapsedTime;
};

template<class T>
inline stop_watch_base<T>::stop_watch_base(const stop_watch_base &src)
{
    bIsRunning  = src.bIsRunning;
    startTime   = src.startTime;
    stopTime    = src.stopTime;
    elapsedTime = src.elapsedTime;
}

template<class T>
inline stop_watch_base<T> &stop_watch_base<T>::operator = (const stop_watch_base &t)
{
    bIsRunning  = t.bIsRunning;
    startTime   = t.startTime;
    stopTime    = t.stopTime;
    elapsedTime = t.elapsedTime;
    return *this;
}

template<class T>
inline void stop_watch_base<T>::native_start()
{
#if _WIN32 || _WIN64
    LARGE_INTEGER qp_cnt;
    QueryPerformanceCounter(&qp_cnt);
    startTime = static_cast<timestamp_t>(qp_cnt.QuadPart);
    stopTime  = startTime;
#elif __linux__
    struct timespec ts;
#if GMTL_USE_ASSERT
    int status =
#endif /* GMTL_USE_ASSERT */
        clock_gettime(CLOCK_REALTIME, &ts);
    _GMTL_ASSERT(status == 0, "CLOCK_REALTIME not supported");
    startTime = static_cast<timestamp_t>(static_cast<int64_t>(1000000000UL) * static_cast<int64_t>(ts.tv_sec) + static_cast<int64_t>(ts.tv_nsec));
    stopTime  = startTime;
#else /* generic Unix */
    struct timeval tv;
#if GMTL_USE_ASSERT
    int status =
#endif /* GMTL_USE_ASSERT */
        gettimeofday(&tv, NULL);
    _GMTL_ASSERT(status == 0, "gettimeofday failed");
    startTime = static_cast<timestamp_t>(static_cast<int64_t>(1000000) * static_cast<int64_t>(tv.tv_sec) + static_cast<int64_t>(tv.tv_usec));
    stopTime  = startTime;
#endif /*(choice of OS) */
}

template<class T>
inline void stop_watch_base<T>::native_stop()
{
#if _WIN32 || _WIN64
    LARGE_INTEGER qp_cnt;
    QueryPerformanceCounter(&qp_cnt);
    stopTime = static_cast<timestamp_t>(qp_cnt.QuadPart);
#elif __linux__
    struct timespec ts;
#if GMTL_USE_ASSERT
    int status =
#endif /* GMTL_USE_ASSERT */
        clock_gettime(CLOCK_REALTIME, &ts);
    _GMTL_ASSERT(status == 0, "CLOCK_REALTIME not supported");
    stopTime = static_cast<timestamp_t>(static_cast<int64_t>(1000000000UL) * static_cast<int64_t>(ts.tv_sec) + static_cast<int64_t>(ts.tv_nsec));
#else /* generic Unix */
    struct timeval tv;
#if GMTL_USE_ASSERT
    int status =
#endif /* GMTL_USE_ASSERT */
        gettimeofday(&tv, NULL);
    _GMTL_ASSERT(status == 0, "gettimeofday failed");
    stopTime = static_cast<timestamp_t>(static_cast<int64_t>(1000000) * static_cast<int64_t>(tv.tv_sec) + static_cast<int64_t>(tv.tv_usec));
#endif /*(choice of OS) */
}

template<class T>
inline typename stop_watch_base<T>::timestamp_t stop_watch_base<T>::native_now(void)
{
    timestamp_t result;

#if _WIN32 || _WIN64
    LARGE_INTEGER qp_cnt, qp_freq;
    QueryPerformanceCounter(&qp_cnt);
    QueryPerformanceFrequency(&qp_freq);
    result = static_cast<timestamp_t>(((double)qp_cnt.QuadPart / (double)qp_freq.QuadPart) * 1000000000.0);
#elif __linux__
    struct timespec ts;
#if GMTL_USE_ASSERT
    int status =
#endif /* GMTL_USE_ASSERT */
        clock_gettime(CLOCK_REALTIME, &ts);
    _GMTL_ASSERT(status == 0, "CLOCK_REALTIME not supported");
    result = static_cast<timestamp_t>(static_cast<int64_t>(1000000000UL) * static_cast<int64_t>(ts.tv_sec) + static_cast<int64_t>(ts.tv_nsec));
#else /* generic Unix */
    struct timeval tv;
#if GMTL_USE_ASSERT
    int status =
#endif /* GMTL_USE_ASSERT */
        gettimeofday(&tv, NULL);
    _GMTL_ASSERT(status == 0, "gettimeofday failed");
    result = static_cast<timestamp_t>(static_cast<int64_t>(1000000000UL) * static_cast<int64_t>(tv.tv_sec) + static_cast<int64_t>(1000UL) * static_cast<int64_t>(tv.tv_usec));
#endif /*(choice of OS) */

    return result;
}

template<class T>
inline double stop_watch_base<T>::native_nowf(void)
{
    double result;

#if _WIN32 || _WIN64
    LARGE_INTEGER qp_cnt, qp_freq;
    QueryPerformanceCounter(&qp_cnt);
    QueryPerformanceFrequency(&qp_freq);
    result = (double)qp_cnt.QuadPart / (double)qp_freq.QuadPart;
#elif __linux__
    int64_t time_usecs;
    struct timespec ts;
#if GMTL_USE_ASSERT
    int status =
#endif /* GMTL_USE_ASSERT */
        clock_gettime(CLOCK_REALTIME, &ts);
    _GMTL_ASSERT(status == 0, "CLOCK_REALTIME not supported");
    time_usecs = static_cast<int64_t>(1000000000UL) * static_cast<int64_t>(ts.tv_sec) + static_cast<int64_t>(ts.tv_nsec);
    result = (double)time_usecs * 1E-9
#else /* generic Unix */
    int64_t time_usecs;
    struct timeval tv;
#if GMTL_USE_ASSERT
    int status =
#endif /* GMTL_USE_ASSERT */
        gettimeofday(&tv, NULL);
    _GMTL_ASSERT(status == 0, "gettimeofday failed");
    time_usecs = static_cast<int64_t>(1000000UL) * static_cast<int64_t>(tv.tv_sec) + static_cast<int64_t>(tv.tv_usec);
    result = (double)time_usecs * 1E-6;
#endif /*(choice of OS) */

    return result;
}

template<class T>
inline void stop_watch_base<T>::clear(void)
{
    T *pT = static_cast<T *>(this);
    pT->clear();
}

template<class T>
inline void stop_watch_base<T>::reset(void)
{
    T *pT = static_cast<T *>(this);
    pT->reset();
}

//! restart() is equivalent to reset() and begin()
template<class T>
inline void stop_watch_base<T>::restart(void)
{
    T *pT = static_cast<T *>(this);
    pT->restart();
}

template<class T>
inline void stop_watch_base<T>::rebegin(void)
{
    T *pT = static_cast<T *>(this);
    pT->restart();
}

template<class T>
inline void stop_watch_base<T>::start(void)
{
    T *pT = static_cast<T *>(this);
    pT->start();
}

template<class T>
inline void stop_watch_base<T>::begin(void)
{
    T *pT = static_cast<T *>(this);
    pT->start();
}

template<class T>
inline void stop_watch_base<T>::stop(void)
{
    T *pT = static_cast<T *>(this);
    pT->stop();
}

template<class T>
inline void stop_watch_base<T>::end(void)
{
    T *pT = static_cast<T *>(this);
    pT->stop();
}

template<class T>
inline typename stop_watch_base<T>::timestamp_t stop_watch_base<T>::now(void)
{
    return native_now();
}

template<class T>
inline double stop_watch_base<T>::nowf(void)
{
    return native_nowf();
}

template<class T>
inline double stop_watch_base<T>::intervalSeconds(int64_t t1, int64_t t2)
{
    double seconds = (double)(t2 - t1) * 1E-9;
    return seconds;
}

template<class T>
inline double stop_watch_base<T>::intervalSeconds(double t1, double t2)
{
    double seconds = (double)(t2 - t1);
    return seconds;
}

template<class T>
inline typename stop_watch_base<T>::timestamp_t stop_watch_base<T>::currentTimeMillis(void)
{
    timestamp_t now_usecs = stop_watch_base<T>::native_now();
    return now_usecs / static_cast<timestamp_t>(1000UL);
}

template<class T>
inline double stop_watch_base<T>::currentTimeMillisf(void)
{
    double now_usecs = stop_watch_base<T>::native_nowf();
    return now_usecs * 1E-3;
}

template<class T>
inline double stop_watch_base<T>::getMillisec(void)
{
    T *pT = static_cast<T *>(this);
    pT->stop();

#if _WIN32 || _WIN64
    LARGE_INTEGER qp_freq;
    QueryPerformanceFrequency(&qp_freq);
    return ((double)pT->elapsedTime / (double)qp_freq.QuadPart) * 1E3;
#elif __linux__
    return (double)pT->elapsedTime * 1E-6;
#else /* generic Unix */
    return (double)pT->elapsedTime * 1E-3;
#endif /* (choice of OS) */
}

template<class T>
inline double stop_watch_base<T>::getSeconds(void)
{
    T *pT = static_cast<T *>(this);
    pT->stop();

#if _WIN32 || _WIN64
    LARGE_INTEGER qp_freq;
    QueryPerformanceFrequency(&qp_freq);
    return (double)pT->elapsedTime / (double)qp_freq.QuadPart;
#elif __linux__
    return (double)pT->elapsedTime * 1E-9;
#else /* generic Unix */
    return (double)pT->elapsedTime * 1E-6;
#endif /* (choice of OS) */
}

template<class T>
inline double stop_watch_base<T>::getElapsedTime(void)
{
    T *pT = static_cast<T *>(this);
    return pT->getSeconds();
}

///////////////////////////////////////////////////////////////////////////
// stop_watch
///////////////////////////////////////////////////////////////////////////

//! Absolute timestamp
/** @ingroup timing */
class stop_watch : public stop_watch_base<stop_watch>
{
public:
    //! Construct an absolute timestamp initialized to zero.
    stop_watch() : stop_watch_base<stop_watch>() {};
    stop_watch(const stop_watch &src);

    stop_watch &operator = (const stop_watch &t);

    void    clear(void);
    void    reset(void);

    //! restart() is equivalent to reset() and begin()
    void    restart(void);
    void    rebegin(void);

    void    start(void);
    void    stop(void);

    void    begin(void);
    void    end(void);

    double  getElapsedTime(void);
};

inline stop_watch::stop_watch(const stop_watch &src) : stop_watch_base<stop_watch>(src)
{
    // Do nothing!!
}

inline stop_watch &stop_watch::operator = (const stop_watch &t)
{
    bIsRunning  = t.bIsRunning;
    startTime   = t.startTime;
    stopTime    = t.stopTime;
    elapsedTime = t.elapsedTime;
    return *this;
}

inline void stop_watch::clear(void)
{
    startTime   = 0;
    stopTime    = 0;
    elapsedTime = 0;

    bIsRunning = false;
}

inline void stop_watch::reset(void)
{
    startTime   = 0;
    stopTime    = 0;
    elapsedTime = 0;

    bIsRunning = false;
}

//! restart() is equivalent to reset() and begin()
inline void stop_watch::restart(void)
{
    elapsedTime = 0;

    native_start();

    bIsRunning = true;
}

inline void stop_watch::rebegin(void)
{
    restart();
}

inline void stop_watch::start(void)
{
    if (!bIsRunning) {
        native_start();
        bIsRunning = true;
    }
}

inline void stop_watch::begin(void)
{
    start();
}

inline void stop_watch::stop(void)
{
    if (bIsRunning) {
        native_stop();
        elapsedTime = stopTime - startTime;
        bIsRunning = false;
    }
}

inline void stop_watch::end(void)
{
    stop();
}

inline double stop_watch::getElapsedTime(void)
{
    return getSeconds();
}

///////////////////////////////////////////////////////////////////////////
// stop_watch_ex
///////////////////////////////////////////////////////////////////////////

//! Absolute timestamp
/** @ingroup timing */
class stop_watch_ex : public stop_watch_base<stop_watch_ex>
{
public:
    //! Construct an absolute timestamp initialized to zero.
    stop_watch_ex() : stop_watch_base<stop_watch_ex>(), suspendStartTime(0), suspendTotalTime(0), elapsedTimeTotal(0) {};
    stop_watch_ex(const stop_watch_ex &src);

    stop_watch_ex &operator = (const stop_watch_ex &sw);

    void    clear(void);
    void    reset(void);

    //! restart() is equivalent to reset() and begin()
    void    restart(void);
    void    rebegin(void);

    void    start(void);
    void    stop(void);

    void    begin(void);
    void    end(void);

    //! for suspend and resume
    void    suspend(void);
    void    resume(void);

    void    pause(void);
    void    continue_(void);

    double  getElapsedTime(void);

    //! get total times
    double  getTotalSeconds(void);
    double  getTotalMillisec(void);
    double  getTotalElapsedTime(void);

protected:
    //! for suspend and resume
    void    native_suspend_start();
    void    native_suspend_stop();

protected:
    timestamp_t suspendStartTime;
    timestamp_t suspendTotalTime;
    timestamp_t elapsedTimeTotal;
};

inline stop_watch_ex::stop_watch_ex(const stop_watch_ex &src) : stop_watch_base<stop_watch_ex>(src)
{
    suspendStartTime    = src.suspendStartTime;
    suspendTotalTime    = src.suspendTotalTime;
    elapsedTimeTotal    = src.elapsedTimeTotal;
}

inline stop_watch_ex &stop_watch_ex::operator = (const stop_watch_ex &sw)
{
    bIsRunning          = sw.bIsRunning;
    startTime           = sw.startTime;
    stopTime            = sw.stopTime;
    elapsedTime         = sw.elapsedTime;
    suspendTotalTime    = sw.suspendTotalTime;
    elapsedTimeTotal    = sw.elapsedTimeTotal;
    return *this;
}

inline void stop_watch_ex::native_suspend_start()
{
    if (suspendStartTime != 0)
        return;

#if _WIN32 || _WIN64
    LARGE_INTEGER qp_cnt;
    QueryPerformanceCounter(&qp_cnt);
    suspendStartTime = static_cast<timestamp_t>(qp_cnt.QuadPart);
#elif __linux__
    struct timespec ts;
#if GMTL_USE_ASSERT
    int status =
#endif /* GMTL_USE_ASSERT */
        clock_gettime(CLOCK_REALTIME, &ts);
    _GMTL_ASSERT(status == 0, "CLOCK_REALTIME not supported");
    suspendStartTime = static_cast<timestamp_t>(static_cast<int64_t>(1000000000UL) * static_cast<int64_t>(ts.tv_sec) + static_cast<int64_t>(ts.tv_nsec));
#else /* generic Unix */
    struct timeval tv;
#if GMTL_USE_ASSERT
    int status =
#endif /* GMTL_USE_ASSERT */
        gettimeofday(&tv, NULL);
    _GMTL_ASSERT(status == 0, "gettimeofday failed");
    suspendStartTime = static_cast<timestamp_t>(static_cast<int64_t>(1000000) * static_cast<int64_t>(tv.tv_sec) + static_cast<int64_t>(tv.tv_usec));
#endif /*(choice of OS) */
}

inline void stop_watch_ex::native_suspend_stop()
{
    if (suspendStartTime == 0)
        return;

    timestamp_t suspendStopTime_;
#if _WIN32 || _WIN64
    LARGE_INTEGER qp_cnt;
    QueryPerformanceCounter(&qp_cnt);
    suspendStopTime_ = static_cast<timestamp_t>(qp_cnt.QuadPart);
#elif __linux__
    struct timespec ts;
#if GMTL_USE_ASSERT
    int status =
#endif /* GMTL_USE_ASSERT */
        clock_gettime(CLOCK_REALTIME, &ts);
    _GMTL_ASSERT(status == 0, "CLOCK_REALTIME not supported");
    suspendStopTime_ = static_cast<timestamp_t>(static_cast<int64_t>(1000000000UL) * static_cast<int64_t>(ts.tv_sec) + static_cast<int64_t>(ts.tv_nsec));
#else /* generic Unix */
    struct timeval tv;
#if GMTL_USE_ASSERT
    int status =
#endif /* GMTL_USE_ASSERT */
        gettimeofday(&tv, NULL);
    _GMTL_ASSERT(status == 0, "gettimeofday failed");
    suspendStopTime_ = static_cast<timestamp_t>(static_cast<int64_t>(1000000) * static_cast<int64_t>(tv.tv_sec) + static_cast<int64_t>(tv.tv_usec));
#endif /*(choice of OS) */

    suspendTotalTime += suspendStopTime_ - suspendStartTime;
    suspendStartTime = 0;
}

inline void stop_watch_ex::clear(void)
{
    startTime           = 0;
    stopTime            = 0;
    elapsedTime         = 0;
    suspendStartTime    = 0;
    suspendTotalTime    = 0;
    elapsedTimeTotal    = 0;

    bIsRunning = false;
}

inline void stop_watch_ex::reset(void)
{
    startTime           = 0;
    stopTime            = 0;
    elapsedTime         = 0;
    suspendStartTime    = 0;
    suspendTotalTime    = 0;

    bIsRunning = false;
}

//! restart() is equivalent to reset() and begin()
inline void stop_watch_ex::restart(void)
{
    elapsedTime         = 0;
    suspendStartTime    = 0;
    suspendTotalTime    = 0;

    native_start();

    bIsRunning = true;
}

inline void stop_watch_ex::rebegin(void)
{
    restart();
}

inline void stop_watch_ex::start(void)
{
    if (!bIsRunning) {
        native_start();
        bIsRunning = true;
    }
}

inline void stop_watch_ex::begin(void)
{
    start();
}

inline void stop_watch_ex::stop(void)
{
    if (bIsRunning) {
        native_stop();
        elapsedTime = stopTime - startTime - suspendTotalTime;
        if (elapsedTime > (timestamp_t)0)
            elapsedTimeTotal += elapsedTime;
        else
            elapsedTimeTotal -= elapsedTime;
        bIsRunning = false;
    }
}

inline void stop_watch_ex::end(void)
{
    stop();
}

inline void stop_watch_ex::suspend(void)
{
    native_suspend_start();
}

inline void stop_watch_ex::resume(void)
{
    native_suspend_stop();
}

inline void stop_watch_ex::pause(void)
{
    suspend();
}

inline void stop_watch_ex::continue_(void)
{
    resume();
}

inline double stop_watch_ex::getElapsedTime(void)
{
    return getSeconds();
}

inline double stop_watch_ex::getTotalMillisec(void)
{
    stop();

#if _WIN32 || _WIN64
    LARGE_INTEGER qp_freq;
    QueryPerformanceFrequency(&qp_freq);
    return ((double)elapsedTimeTotal / (double)qp_freq.QuadPart) * 1E3;
#elif __linux__
    return (double)elapsedTimeTotal * 1E-6;
#else /* generic Unix */
    return (double)elapsedTimeTotal * 1E-3;
#endif /* (choice of OS) */
}

inline double stop_watch_ex::getTotalSeconds(void)
{
    stop();

#if _WIN32 || _WIN64
    LARGE_INTEGER qp_freq;
    QueryPerformanceFrequency(&qp_freq);
    return (double)elapsedTimeTotal / (double)qp_freq.QuadPart;
#elif __linux__
    return (double)elapsedTimeTotal * 1E-9;
#else /* generic Unix */
    return (double)elapsedTimeTotal * 1E-6;
#endif /* (choice of OS) */
}

inline double stop_watch_ex::getTotalElapsedTime(void)
{
    return getTotalSeconds();
}

}  /* namespace annlab */

#endif  /* _ANNLAB_STOP_WATCH_H_ */
