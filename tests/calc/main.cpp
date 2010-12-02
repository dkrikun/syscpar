
#include "systemc.h"
#include "calculator.h"
#include <stdio.h>
#include <sstream>
#include <string>


#define usage_and_exit()\
	{printf("\nusage: %s NUM_INSTANCES LOG2_NUM_OPS USE_MARKS {t|m}\n\nwhere:  NUM_INSTANCES - number of objects to instantiate,\n        LOG2_NUM_OPS - log2 FLOPS for each object to perform,\n       USE_MARKS - mark execution progress,\n        t|m - use thread or method processes\n", argv[0]);\
	return 1;}
	
#define exp2(k)\
	(1UL<<(k))
	
#define REPORT_TIME()\
	std::cout << sc_time_stamp() << std::endl;

inline std::string intToStr(int i){
	std::stringstream s;
	s << i;
	return s.str();
}
using namespace calc;
int sc_main(int argc, char *argv[]) {
    
    //parse args
    if(argc!=5)
        usage_and_exit();
             
    int num_insts = atoi(argv[1]);
    int  log_num_ops = atoi(argv[2]);
    bool is_thread = false;
    bool use_marks = static_cast<bool>(atoi(argv[3]));
    if(std::string(argv[4])==std::string("t"))
		is_thread = true;
    else if(std::string(argv[4])!=std::string("m"))
        usage_and_exit();
   
    //instantiate objects
    Calculator** calcs = new Calculator*[num_insts];
    
    for(int i=0; i<num_insts; i++)
	calcs[i] = new Calculator(intToStr(i).c_str(),log_num_ops, use_marks, i, is_thread);

    
    sc_start(SC_ZERO_TIME);

    
    for(int i=0; i<num_insts; i++)
    	delete calcs[i];
    delete calcs;
    
    REPORT_TIME();
    //std::cout << "last statement in main()" << std::endl;
}

