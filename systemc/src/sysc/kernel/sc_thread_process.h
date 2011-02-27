/*
 * sc_thread_process.h
 *
 *  Created on: Sep 15, 2010
 *      Author: sdaniele
 */

#ifndef SC_THREAD_PROCESS_PAR_H_
#define SC_THREAD_PROCESS_PAR_H_


#include "sysc/kernel/sc_spawn_options.h"
#include "sysc/kernel/sc_process.h"
#include "sysc/kernel/sc_method_process.h"
#include "sysc/kernel/sc_cor.h"
#include "sysc/kernel/sc_event.h"
#include "sysc/kernel/sc_except.h"
#include "sysc/kernel/sc_reset.h"

#include <boost/context/all.hpp>
#include <boost/bind.hpp>
#include "sysc/kernel/dbgprint_config.hpp"


namespace sc_core {

class sc_event_and_list;
class sc_event_or_list;
class sc_reset;
class sc_event;
class sc_join;
class sc_module;
class sc_process_handle;
class sc_process_table;
class sc_simcontext;
class sc_runnable;

void wait(sc_simcontext*);
void wait(const sc_event&, sc_simcontext*);
void wait(sc_event_or_list&, sc_simcontext*);
void wait(sc_event_and_list&, sc_simcontext*);
void wait(const sc_time&, sc_simcontext*);
void wait(const sc_time&, const sc_event&, sc_simcontext*);
void wait(const sc_time&, sc_event_or_list&, sc_simcontext*);
void wait(const sc_time&, sc_event_and_list&, sc_simcontext*);


void wrapper(void* arg);	// wrapper for boost::context

class sc_thread_process : public sc_core::sc_method_process {
	friend class sc_event;
	friend class sc_join;
	friend class sc_module;
	friend class sc_process_handle;
	friend class sc_process_table;
	friend class sc_simcontext;
	friend class sc_runnable;

	friend void wait(sc_simcontext*);
	friend void wait(const sc_event&, sc_simcontext*);
	friend void wait(sc_event_or_list&, sc_simcontext*);
	friend void wait(sc_event_and_list&, sc_simcontext*);
	friend void wait(const sc_time&, sc_simcontext*);
	friend void wait(const sc_time&, const sc_event&, sc_simcontext*);
	friend void wait(const sc_time&, sc_event_or_list&, sc_simcontext*);
	friend void wait(const sc_time&, sc_event_and_list&, sc_simcontext*);


	friend void wrapper(void* arg);
public:
	sc_thread_process(const char* name_p, bool free_host,
			SC_ENTRY_FUNC method_p, sc_process_host* host_p,
			const sc_spawn_options* opt_p);

	virtual ~sc_thread_process(){}

	virtual const char* kind() const {
		return "sc_thread_process";
	}

protected:
	virtual void kill_process();
	sc_thread_process* next_exist();
	sc_thread_process* next_runnable();
	inline bool ready_to_run();
	void set_next_exist(sc_thread_process* next_p);
	void set_next_runnable(sc_thread_process* next_p);

	void wait(const sc_event&);
	void wait(sc_event_or_list&);
	void wait(sc_event_and_list&);
	void wait(const sc_time&);
	void wait(const sc_time&, const sc_event&);
	void wait(const sc_time&, sc_event_or_list&);
	void wait(const sc_time&, sc_event_and_list&);
	void wait_cycles(int n = 1);

protected:
	void add_monitor(sc_process_monitor* monitor_p);
	void remove_monitor(sc_process_monitor* monitor_p);
	void signal_monitors(int type = 0);

protected:
	boost::protected_stack  m_stack;
	boost::context<>	m_main;
	boost::context<>	m_active;
	std::vector<sc_process_monitor*> m_monitor_q;
	int m_wait_cycle_n;

private:
	// disabled
	sc_thread_process(const sc_thread_process&);
	const sc_thread_process& operator =(const sc_thread_process&);

protected:
	void suspend();
private:
    void semantics(){ resume(); }
	void resume()  {
		with_tag<thread_context>::println("resuming..");
		m_main.jump_to(m_active);
	}
	void wrapper_helper() { sc_method_process::semantics(); }
};

inline sc_thread_process* sc_thread_process::next_exist() {
	//assert_nonpar();
	return (sc_thread_process*) m_exist_p;
}

inline
void sc_thread_process::set_next_exist(sc_thread_process* next_p) {
	//assert_nonpar();
	m_exist_p = next_p;
}

inline
void sc_thread_process::set_next_runnable(sc_thread_process* next_p) {				//queue
	m_runnable_p = next_p;
}

inline sc_thread_process* sc_thread_process::next_runnable() {						//queue
	return (sc_thread_process*) m_runnable_p;
}

inline bool sc_thread_process::ready_to_run() {
	if ((m_throw_type == THROW_NONE) && (m_wait_cycle_n > 0)) {
		--m_wait_cycle_n;
		return false;
	}
	return true;
}

inline void sc_thread_process::suspend() {
	with_tag<thread_context>::println("suspending..");
	m_active.jump_to(m_main);

	switch (m_throw_type) {
	case THROW_NONE:
		break;
	case THROW_RESET:
		throw sc_user();
	default:
		break;
	}
}

inline
void sc_thread_process::add_monitor(sc_process_monitor* monitor_p) {
	m_monitor_q.push_back(monitor_p);
}

inline
void sc_thread_process::remove_monitor(sc_process_monitor* monitor_p) {
	int mon_n = m_monitor_q.size();

	for (int mon_i = 0; mon_i < mon_n; mon_i++) {
		if (m_monitor_q[mon_i] == monitor_p) {
			m_monitor_q[mon_i] = m_monitor_q[mon_n - 1];
			m_monitor_q.resize(mon_n - 1);
		}
	}
}

inline
void sc_thread_process::wait(const sc_event& e) {
	sc_method_process::next_trigger(e);
	suspend();
}

inline
void sc_thread_process::wait(sc_event_or_list& el) {
	sc_method_process::next_trigger(el);
	suspend();
}

inline
void sc_thread_process::wait(sc_event_and_list& el) {
	sc_method_process::next_trigger(el);
	suspend();
}

inline
void sc_thread_process::wait(const sc_time& t) {
	sc_method_process::next_trigger(t);
	suspend();
}

inline
void sc_thread_process::wait(const sc_time& t, const sc_event& e) {
	sc_method_process::next_trigger(t,e);
	suspend();
}

inline
void sc_thread_process::wait(const sc_time& t, sc_event_or_list& el) {
	sc_method_process::next_trigger(t,el);
	suspend();
}

inline
void sc_thread_process::wait(const sc_time& t, sc_event_and_list& el) {
	sc_method_process::next_trigger(t,el);
	suspend();
}

inline
void sc_thread_process::wait_cycles(int n) {
	m_wait_cycle_n = n - 1;
	suspend();
}

} //namespace sc_core

#endif /* SC_THREAD_PROCESS_PAR_H_ */
