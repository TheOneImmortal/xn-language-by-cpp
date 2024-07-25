/**
 * ****************timer.hh****************
 * @brief  计时器
 * @author https://github.com/TheOneImmortal
 * @date   2024-01-06
 * ********************************
 */

#ifndef __XN_TIMER__TIMER_HH__
#define __XN_TIMER__TIMER_HH__

/*----------------include----------------*/
#include <chrono>

#include "../base_type/include_me.hh"

/*----------------body----------------*/
namespace xn {

class timer {
	using ns    = std::chrono::nanoseconds;
	using clock = std::chrono::high_resolution_clock;
	using tp    = std::chrono::time_point<clock>;

  public:
	timer(): start_tp(clock::now()) {}

	~timer() { Stop(); }

  public:
	str Stop() {
		auto end_tp = clock::now();

		auto start = tp_to_time<ns>(start_tp);
		auto end   = tp_to_time<ns>(end_tp);

		auto duration = end - start;
		auto ms       = duration * 1e-6;

		return "{}ns({}ms)"_f(duration, ms);
	}

  private:
	template<typename To>
	static constexpr i64 tp_to_time(tp const &tp) noexcept {
		return std::chrono::time_point_cast<To>(tp)
		    .time_since_epoch()
		    .count();
	}

  private:
	tp start_tp;
};

}  // namespace xn

#endif
