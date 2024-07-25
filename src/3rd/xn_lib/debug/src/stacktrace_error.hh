/**
 * ****************stacktrace_error.hh****************
 * @brief  函数栈异常
 * @author https://github.com/TheOneImmortal
 * @date   2024-01-26
 * ********************************
 */

#ifndef __XN_DEBUG__STACKTRACE_ERROR_HH__
#define __XN_DEBUG__STACKTRACE_ERROR_HH__

/*----------------include----------------*/
// #include <stacktrace>

// #include "../../base_type/include_me.hh"

/*----------------body----------------*/
namespace xn {

// 暂时无法使用

// class stacktrace_error : public std::exception {
// public:
//     explicit stacktrace_error( str const& message, std::stacktrace
//     trace = std::stacktrace::current() ) {
//         msg.append( "Exception Message: {}\n"_f( message ) );
//         for ( val& entry : trace ) msg.append( "\tat {} in {}:line
//         {}\n"_f( entry.description(), entry.source_file(),
//         entry.source_line() ) );
//     }
//     char const* what() const noexcept override { return msg.c_str();
//     }

// private:
//     string msg;
// };

}  // namespace xn

#endif
