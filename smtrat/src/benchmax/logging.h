/**
 * @file logging.h
 * @author Gereon Kremer <gereon.kremer@cs.rwth-aachen.de>
 */

#pragma once

#include "config.h"

#if defined BENCHMAX_LOGGING

#include <carl/core/carlLogging.h>
namespace benchmax {
	using carl::operator<<;
}

	#define BENCHMAX_LOG_FATAL(channel, msg) __CARL_LOG_FATAL(channel, msg)
	#define BENCHMAX_LOG_ERROR(channel, msg) __CARL_LOG_ERROR(channel, msg)
	#define BENCHMAX_LOG_WARN(channel, msg) __CARL_LOG_WARN(channel, msg)
	#define BENCHMAX_LOG_INFO(channel, msg) __CARL_LOG_INFO(channel, msg)
	#define BENCHMAX_LOG_DEBUG(channel, msg) __CARL_LOG_DEBUG(channel, msg)	
	#define BENCHMAX_LOG_TRACE(channel, msg) __CARL_LOG_TRACE(channel, msg)

	#define BENCHMAX_LOG_FUNC(channel, args) __CARL_LOG_FUNC(channel, args)
	#define BENCHMAX_LOG_ASSERT(channel, condition, msg) __CARL_LOG_ASSERT(channel, condition, msg)
	#define BENCHMAX_LOG_NOTIMPLEMENTED() __CARL_LOG_ERROR("", "Not implemented method-stub called.")
	#define BENCHMAX_LOG_INEFFICIENT() __CARL_LOG_WARN("", "Inefficient method called.")
#else
	#define BENCHMAX_LOG_FATAL(channel, msg) std::cerr << "FATAL: " << msg << std::endl;
	#define BENCHMAX_LOG_ERROR(channel, msg) std::cerr << "ERROR: " << msg << std::endl;
	#define BENCHMAX_LOG_WARN(channel, msg)
	#define BENCHMAX_LOG_INFO(channel, msg)
	#define BENCHMAX_LOG_DEBUG(channel, msg)
	#define BENCHMAX_LOG_TRACE(channel, msg)

	#define BENCHMAX_LOG_FUNC(channel, args)
	#define BENCHMAX_LOG_ASSERT(channel, condition, msg) assert(condition)
	#define BENCHMAX_LOG_NOTIMPLEMENTED()
	#define BENCHMAX_LOG_INEFFICIENT()
#endif
