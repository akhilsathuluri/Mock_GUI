/* Copyright 2019-2020 Baumer Optronic */
#pragma once

#include <string>
#include <algorithm>
#include <vector>
#include <mutex>

class TimingStat {
 public:
    TimingStat();
    virtual ~TimingStat();

    void Reset();

    // start the timer from a prior time
    void Start(std::chrono::steady_clock::time_point start_tp);

    // start the timer now
    uint64_t Start();

    // end the timer now, add to intev
    void Stop(std::chrono::steady_clock::time_point end_tp);

    uint64_t Stop();

    // adds ms value to the statistics
    void Add_ms(double new_ms);

    double Count() const { return count_; }
    double Last_ms() const { return std::min(max_display_double_, last_); }
    double Min_ms() const { return std::min(max_display_double_, min_); }
    double Max_ms() const { return std::min(max_display_double_, max_); }
    double Ave_ms() const {
        double ave = count_ ? sum_ / count_ : 0;
        return std::min(max_display_double_, ave);
    }

    // formats interval statistics:
    //  last   count   ave     min  ---  histogram  ---   max  (max-min)
    // 9.2ms   12345 x 8.3ms   1.1< 340 <5< 12000 <10< 5 <12.3 (11.2)
    //
    // In the 'histogram' the interval bounds are next to the '<' and '<'
    //   and the counts in the intervals are surrounded by spaces
    // In the above example:
    //   340 timings were above 1.1ms and less/equal to 5ms
    //   12000 timings were above 5ms and less/equal to 10ms
    //   5 timings were above 10ms and less/equal to 12.3ms
    std::string Hist();

 private:
    const double max_display_double_ = 999999.99;
    std::chrono::steady_clock::time_point start_;
    std::chrono::steady_clock::time_point end_;
    int count_;
    double sum_;
    double last_;
    double min_;
    double max_;
    std::vector<int> hist_times_;
    std::vector<int> hist_cnts_;
    // mutex to protect timing data
    std::mutex statistic_lock_;
};
