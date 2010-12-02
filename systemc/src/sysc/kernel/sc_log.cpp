/*
 * sc_log.cpp
 *
 *  Created on: Sep 5, 2010
 *      Author: sdaniele
 */

#include "sc_log.h"

#include <boost/bind.hpp>
#include "sysc/kernel/dbgprint_config.hpp"

#include "sysc/kernel/sc_event.h"
#include "sysc/kernel/sc_method_process.h"
#include "sysc/communication/sc_prim_channel.h"


namespace sc_core {

// sc_event family
void sc_log::cancel(sc_event* e)
{
	m_log.push_back(boost::bind(&sc_event::cancel,e));
	with_tag<tls_log>::println("logged cancel with e=",e);
}

void sc_log::notify_internal(sc_event* e, const sc_time& t)
{
	m_log.push_back(boost::bind(&sc_event::notify,e,t));
	with_tag<tls_log>::println("logged internal notification with:\n\te=",e,"\n\tt=",t);
}

void sc_log::notify_next_delta(sc_event* e)
{
	m_log.push_back(boost::bind(&sc_event::notify_next_delta,e));
	with_tag<tls_log>::println("logged next_delta notification with e=",e);
}

void sc_log::notify(sc_event* e)
{
	m_log.push_back(boost::bind(&sc_event::notify,e));
	with_tag<tls_log>::println("logged imm. notification with e=",e);
}

void sc_log::notify(sc_event* e, const sc_time& t)
{
	m_log.push_back(boost::bind(&sc_event::notify,e,t));
	with_tag<tls_log>::println("logged delta/timed notification with:\n\te=",e,"\n\tt=",t);
}

void sc_log::notify_delayed(sc_event* e)
{
	m_log.push_back(boost::bind(&sc_event::notify_delayed,e));
	with_tag<tls_log>::println("logged notify_delayed with e=",e);
}

void sc_log::notify_delayed(sc_event* e, const sc_time& t)
{
	m_log.push_back(boost::bind(&sc_event::notify_delayed,e,t));
	with_tag<tls_log>::println("logged notify_delayed with:\n\te=",e,"\n\tt=",t);
}

// request_update
void sc_log::request_update(sc_prim_channel* c){
	m_log.push_back(boost::bind(&sc_prim_channel::request_update,c));
	with_tag<tls_log>::println("logged request_update with c=",c);
}

// next_trigger family

void sc_log::next_trigger(){
	m_log.push_back(boost::bind(&sc_method_process::clear_trigger,(sc_method_process*)m_process));
	with_tag<tls_log>::println("logged next_trigger");
}

void sc_log::next_trigger(const sc_event& e){
	m_log.push_back(boost::bind(SCAST<void (sc_method_process::*)(const sc_event&)>(&sc_method_process::next_trigger)
			, SCAST<sc_method_process*>(m_process)
			, boost::ref(e)));

	with_tag<tls_log>::println("logged next_trigger with e=",&e);
}

void sc_log::next_trigger(sc_event_or_list& el){
	m_log.push_back(boost::bind(SCAST<void (sc_method_process::*)(sc_event_or_list&)>(&sc_method_process::next_trigger)
			, SCAST<sc_method_process*>(m_process)
			, boost::ref(el)));
	with_tag<tls_log>::println("logged next_trigger(or_list) with el=",&el);
}

void sc_log::next_trigger(sc_event_and_list& el){

	m_log.push_back(boost::bind(SCAST<void (sc_method_process::*)(sc_event_and_list&)>(&sc_method_process::next_trigger)
			, SCAST<sc_method_process*>(m_process)
			, boost::ref(el)));
	with_tag<tls_log>::println("logged next_trigger(and_list) with el=",&el);
}

void sc_log::next_trigger(const sc_time& t){

	m_log.push_back(boost::bind(SCAST<void (sc_method_process::*)(const sc_time&)>(&sc_method_process::next_trigger)
			, SCAST<sc_method_process*>(m_process)
			, t));
	with_tag<tls_log>::println("logged next_trigger with t=",t);
}

void sc_log::next_trigger(const sc_time& t, const sc_event& e){

	m_log.push_back(boost::bind(SCAST<void (sc_method_process::*)(const sc_time&, const sc_event&)>(&sc_method_process::next_trigger)
			, SCAST<sc_method_process*>(m_process)
			, t
			, boost::cref(e)));
	with_tag<tls_log>::println("logged next_trigger with t=",t," with e=",&e);
}

void sc_log::next_trigger(const sc_time& t, sc_event_or_list& el){

	m_log.push_back(boost::bind(SCAST<void (sc_method_process::*)(const sc_time&, sc_event_or_list&)>(&sc_method_process::next_trigger)
			, SCAST<sc_method_process*>(m_process)
			, t
			, boost::ref(el)));
	with_tag<tls_log>::println("logged next_trigger(or_list) with t=",t," with el=",&el);
}

void sc_log::next_trigger(const sc_time& t, sc_event_and_list& el){

	m_log.push_back(boost::bind(SCAST<void (sc_method_process::*)(const sc_time&, sc_event_and_list&)>(&sc_method_process::next_trigger)
			, SCAST<sc_method_process*>(m_process)
			, t
			, boost::ref(el)));
	with_tag<tls_log>::println("logged next_trigger(and_list) with t=",t," with el=",&el);
}

void sc_log::execute_and_reset(){
	with_tag<tls_log>::println("\tfound ",m_log.size(), " entries in the log=",this);
	for(size_t i=0; i<m_log.size(); ++i)
		m_log[i]();
	m_log.clear();
}



}
