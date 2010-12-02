/*
 * sc_log.h
 *
 *  Created on: Sep 2, 2010
 *      Author: sdaniele
 */

#ifndef SC_LOG_H_
#define SC_LOG_H_

#include "sysc/kernel/dbgprint_config.hpp"

#include <boost/function.hpp>
#include <vector>

#include "sysc/kernel/sc_time.h"

namespace sc_core {

class sc_event;
class sc_event_or_list;
class sc_event_and_list;
class sc_prim_channel;
class sc_process_b;

struct sc_log{

	std::vector<boost::function<void()> >	m_log;
	sc_process_b*							m_process;

	sc_log(sc_process_b* p)
		: m_process(p){}

	void cancel(sc_event*);
	void notify(sc_event*);
	void notify(sc_event*, const sc_time&);
    void notify_delayed(sc_event*);
    void notify_delayed(sc_event*, const sc_time&);
	void notify_internal(sc_event*, const sc_time&);
	void notify_next_delta(sc_event*);

	void request_update(sc_prim_channel*);

	void next_trigger();
	void next_trigger(const sc_event&);
	void next_trigger(sc_event_or_list&);
	void next_trigger(sc_event_and_list&);
	void next_trigger(const sc_time&);
	void next_trigger(const sc_time&, const sc_event&);
	void next_trigger(const sc_time&, sc_event_or_list&);
	void next_trigger(const sc_time&, sc_event_and_list&);

	void execute_and_reset();
};


}

#endif /* SC_LOG_H_ */
