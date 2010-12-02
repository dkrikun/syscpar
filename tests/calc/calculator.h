
#ifndef _CALCULATOR_
#define _CALCULATOR_

#include "systemc.h"
#include "flops_job.h"
#include <string>
namespace calc{
std::string gl_marks[10] = {"!","#",".","/","*","O","^","%","`","@"};

class Calculator : public sc_module {
    private:

	unsigned long m_log_num_ops;
	std::string m_name;
	bool m_use_marks;
	int m_i	;

	void report_completion(){
		printf("%s reports %lu flops performed ok\n", m_name.c_str(), m_log_num_ops);
	}

    public:
	SC_HAS_PROCESS(Calculator);
	
        Calculator(sc_module_name name, unsigned long log_num_ops, bool use_marks, int i, bool is_thread)
        	:sc_module(name), m_name(name), m_log_num_ops(log_num_ops), m_use_marks(use_marks), m_i(i)
        {
        	if(is_thread) SC_THREAD(do_work)
        	else SC_METHOD(do_work);
	}
	
	void do_work(){
		if(m_use_marks)
			calc::flops_job(m_log_num_ops,gl_marks[m_i%10]);
		else	
			calc::flops_job(m_log_num_ops);
		report_completion();
	}

	void end_of_elaboration(){
		//::std::cout << m_name << ", end_of_elaboration" << std::endl;
	}
       
};
}
#endif

