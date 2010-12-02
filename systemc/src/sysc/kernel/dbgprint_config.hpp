/*
 * dbgprint_config.hpp
 *
 *  Created on: Aug 29, 2010
 *      Author: sdaniele
 */

#ifndef DBGPRINT_CONFIG_HPP_
#define DBGPRINT_CONFIG_HPP_

#include <dbgprint/dbgprint_p1.hpp>
using namespace dbgprint;

DBGPRINT_DEFINE_TAG(tls_log);
DBGPRINT_DEFINE_TAG(thread_context);
DBGPRINT_DEFINE_TAG(cpi);
DBGPRINT_DEFINE_TAG(eval_phase);

typedef dbgprint::tags<
		cpi
	  , thread_context
	  , eval_phase
	  , tls_log
	> disabled;

#include <dbgprint/dbgprint_p2.hpp>

#endif /* DBGPRINT_CONFIG_HPP_ */
