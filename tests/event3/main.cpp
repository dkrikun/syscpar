
#define SC_INCLUDE_DYNAMIC_PROCESSES 	//necessary for sc_spawn() to compile

#include <boost/bind.hpp>
#include "systemc.h"
#include "flops_job.h"

#include <iostream>
#include <assert.h>
#include "tbb/tick_count.h"
#include <boost/utility.hpp>
#include <boost/function.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/tokenizer.hpp>

#define print(msg)\
        std::cout << msg << std::endl;
        
namespace event3 {
namespace mpl = boost::mpl;

// instead of clock
struct progress : boost::noncopyable {
        sc_event e;
        const sc_time& time;
        size_t cycles;

        void engine(){
                if(cycles--){
                        next_trigger(e);
                        e.notify(time);
                }
                else next_trigger();
        }

        progress(const sc_time& time, size_t cycles)
                : time(time)
                , cycles(cycles)
        {
                sc_spawn_options opt;
                opt.spawn_method();
                sc_spawn(boost::bind(&progress::engine,this),"__ProgresS__",&opt);
        }

        const sc_event* trigger() const { return &e; }
};


typedef boost::function<void ()> action;
struct actions : boost::noncopyable {
 private: actions(){}
        static void _nop(){}
 public:
        static action nop()                                        { return _nop; }
        static action immediate(sc_event* e)                       { return boost::bind(static_cast<void (sc_event::*)()>(&sc_event::notify),e); }
        static action timed(sc_event* e,const sc_time& t)          { return boost::bind(static_cast<void (sc_event::*)(const sc_time&)>(&sc_event::notify),e,t); }
        static action delta(sc_event* e)                           { return timed(e,SC_ZERO_TIME); }
#       if defined(V1)
        static action cancel(sc_event* e)                          { return boost::bind(&sc_event::cancel,e,true); }
#       else
        static action cancel(sc_event* e)                          { return boost::bind(&sc_event::cancel,e); }
#       endif
        static action notify_internal(sc_event* e,const sc_time& t){ return boost::bind(&sc_event::notify_internal,e,t); }
        static action notify_next_delta(sc_event* e)               { return boost::bind(&sc_event::notify_next_delta,e); }

        static action create(std::string arg, sc_event* e){
                typedef boost::tokenizer<boost::char_separator<char> > tokenizer_t;
                tokenizer_t tokens(arg, boost::char_separator<char>(":"));

                for (tokenizer_t::iterator it = tokens.begin(); it != tokens.end(); ++it) {
                        if(*it == std::string("nop"))
                                return nop();
                        if(*it == std::string("immediate"))
                                return immediate(e);
                        if(*it == std::string("timed")) {
                                if(++it == tokens.end()){
                                        print("timed requires time argument, for example \"timed:5\"");
                                        exit(1);
                                }
                                int ms = boost::lexical_cast<int>(*(it++));
                                return timed(e,sc_time(ms,SC_MS));
                        }
                        if(*it == std::string("delta"))
                                return delta(e);
                        if(*it == std::string("cancel"))
                                return cancel(e);

                        print("unknown action requested:\"" << *it << "\"");
                        exit(1);
                }
        }
};

struct actor : boost::noncopyable {
        actor(const std::string name, bool is_thread, action a, const sc_event* trigger, action report)
        : name(name)
        , is_thread(is_thread)
        , a(a)
        , trigger(trigger)
        , report(report)
        {}

        const std::string name;
        bool is_thread;
        action a;
        const sc_event* trigger;
        action report;

        void operator()(){
                sc_spawn_options opt;
                if(!is_thread) opt.spawn_method();
                sc_spawn(boost::bind(&actor::go,a,trigger,report,is_thread),name.c_str(),&opt);
        }

        static void go(action a, const sc_event* trigger, action report, bool is_thread){
                if(is_thread)
                        while(1) {
                                a();
                                report();
                                wait(*trigger);
                        }
                else {
                        a();
                        report();
                        next_trigger(*trigger);
                }
        }
};

// print something to the screen
inline void mark(size_t i){ std::cout << char(33+i%(126-33)) << ' '; }

}

using namespace event3;
int sc_main(int argc, char** argv){
        if(argc!=9){ print("Wrong argc, expected 8 parameters, usage example: " << argv[0] << " 1 5 4 4 0 0 0 timed:1"); exit(1); }
        int period               = atoi(argv[1]);               // progress period
        size_t ncycles           = atoi(argv[2]);               // num. cycles of period
        size_t nactors           = (1UL << atoi(argv[3]));      // num. actors to instantiate
        size_t nevents           = (1UL << atoi(argv[4]));      // num. events to instantiate
        bool is_silent           = atoi(argv[5]);               // suppress output
        bool is_thread           = atoi(argv[6]);               // use threads or methods
        bool progress_only       = atoi(argv[7]);               // only progress, no actors or other events
        std::string action_str   = std::string(argv[8]);        // action description

        // init. progress (clock of the system) 
        progress p(
                sc_time(period,SC_MS)
              , ncycles
          );

        if(!progress_only){
                // instantiate events
                sc_event** es = new sc_event*[nevents];
                for(size_t i=0; i<nevents; ++i)
                        es[i] = new sc_event;
                        

                // instantiate actors
                actor** as = new actor*[nactors];
                for(size_t i=0; i<nactors; ++i)
                        as[i] = new actor(
                                boost::lexical_cast<std::string>(i)
                              , is_thread
                              , actions::create(action_str,es[i%nevents])
                              , p.trigger()
                              , is_silent? actions::nop() : boost::bind(&mark,i)
                         );
                
                // push actor processes into sysc process table
                for(size_t i=0; i<nactors; ++i) as[i]->operator()();
        }
        tbb::tick_count t0 = tbb::tick_count::now();
        sc_start();
        tbb::tick_count t1 = tbb::tick_count::now();
        if(!is_silent)print(std::endl);
        if(getenv("REPORT")) print((t1-t0).seconds());
}

