/*****************************************************************************

  The following code is derived, directly or indirectly, from the SystemC
  source code Copyright (c) 1996-2006 by all Contributors.
  All Rights reserved.

  The contents of this file are subject to the restrictions and limitations
  set forth in the SystemC Open Source License Version 2.4 (the "License");
  You may not use this file except in compliance with such restrictions and
  limitations. You may obtain instructions on how to receive a copy of the
  License at http://www.systemc.org/. Software distributed by Contributors
  under the License is distributed on an "AS IS" basis, WITHOUT WARRANTY OF
  ANY KIND, either express or implied. See the License for the specific
  language governing rights and limitations under the License.

 *****************************************************************************/

/*****************************************************************************

  sc_main.cpp - Wrapper around user's toplevel routine `sc_main'.

  Original Author: Stan Y. Liao, Synopsys, Inc.

 *****************************************************************************/

/*****************************************************************************

  MODIFICATION LOG - modifiers, enter your name, affiliation, date and
  changes you are making here.

      Name, Affiliation, Date:
  Description of Modification:

 *****************************************************************************/


// $Log: sc_main.cpp,v $
// Revision 1.1.1.1  2006/12/15 20:31:37  acg
// SystemC 2.2
//
// Revision 1.3  2006/01/13 18:44:29  acg
// Added $Log to record CVS changes into the source.
//

#include "sysc/kernel/sc_cmnhdr.h"
#include "sysc/kernel/sc_externs.h"

#include <tbb/tick_count.h>
#include <tbb/tbb_thread.h>
#include <tbb/task_scheduler_init.h>
#include "sysc/kernel/dbgprint_config.hpp"

#include <stdlib.h>

#include <iostream>
#include <ostream>
#include <fstream>
#include <stdio.h>
#include <string.h>

using namespace tbb;

#define PREFIX "[basic] "

int
main( int argc, char* argv[] )
{
	char* tmp = getenv("NT");
	int num_threads = tmp?atoi(tmp): ::tbb::task_scheduler_init::default_num_threads();
	::tbb::task_scheduler_init init(num_threads);

	tick_count t0 = tick_count::now();
	int res =  sc_core::sc_elab_and_sim( argc, argv );
	tick_count t1 = tick_count::now();

	if(!getenv("MIMIC_ORIG")){
		tmp = 0;
		tmp = getenv("OUT");
		bool is_file = false;
		::std::ostream* outs;
		if(!tmp || !strcmp(tmp,"cout"))
			outs = &::std::cout;
		else if(!strcmp(tmp,"cerr"))
			outs = &::std::cerr;
		else{
			is_file = true;
			outs = new ::std::ofstream;
			((::std::ofstream*)outs)->open(tmp, ::std::ios::out | ::std::ios::app);
		}



		tmp = getenv("VERBOSE");
		int verbose_lvl = tmp?atoi(tmp): 0;
		if(verbose_lvl == 1)
			*outs << PREFIX << (t1-t0).seconds() << ::std::endl;
		else if(verbose_lvl == 2){
			*outs << PREFIX << "total work took " << (t1-t0).seconds() << " sec." << ::std::endl;
			*outs << PREFIX << "on " << num_threads << "/" << ::tbb::tbb_thread::hardware_concurrency() << " threads" << ::std::endl;
		}
		if(is_file)
			((::std::ofstream*)outs)->close();
	}
	return res;
}
