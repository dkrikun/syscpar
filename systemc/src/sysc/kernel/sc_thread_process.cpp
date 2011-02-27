/*
 * sc_thread_process_par.cpp
 *
 *  Created on: Sep 15, 2010
 *      Author: sdaniele
 */

#include "sc_thread_process.h"

namespace sc_core {

void wrapper(void* arg){ SCAST<sc_thread_process*>(arg)->wrapper_helper(); }

#define DEFAULT_STACK_SIZE 65536


sc_thread_process::sc_thread_process( const char* name_p, bool free_host, SC_ENTRY_FUNC method_p, sc_process_host* host_p,	const sc_spawn_options* opt_p )
	  : sc_method_process(name_p,free_host,method_p,host_p,opt_p)
	  , m_stack((opt_p && opt_p->m_stack_size)? opt_p->m_stack_size : DEFAULT_STACK_SIZE)
	  , m_active(
				  wrapper
				, this
				, boost::move(m_stack)
			)
	  , m_wait_cycle_n(0)
{
	m_process_kind = SC_THREAD_PROC_;
}

void sc_thread_process::kill_process() {

	signal_monitors(sc_process_monitor::spm_exit);
	sc_method_process::kill_process();
}

void sc_thread_process::signal_monitors(int type) {
	int mon_n = m_monitor_q.size();

	for (int mon_i = 0; mon_i < mon_n; mon_i++)
		m_monitor_q[mon_i]->signal(this, type);
}


}
