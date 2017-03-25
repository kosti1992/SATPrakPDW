/** 
 * @file   adaption_cln/hash.h
 * @ingroup cln
 * @author Sebastian Junges
 * @author  Florian Corzilius
 *
 */

#pragma once

#ifndef INCLUDED_FROM_NUMBERS_H
static_assert(false, "This file may only be included indirectly by numbers.h");
#endif

#include "../../util/platform.h"
//CLANG_WARNING_DISABLE("-Wmismatched-tags")
#include <cln/cln.h>
//CLANG_WARNING_RESET
namespace std
{
	
template<>
struct hash<cln::cl_RA> {
	size_t operator()(const cln::cl_RA& n) const {
		return cln::equal_hashcode(n);
	}
};

template<>
struct hash<cln::cl_I> {
	size_t operator()(const cln::cl_I& n) const {
		return cln::equal_hashcode(n);
	}
};

}
