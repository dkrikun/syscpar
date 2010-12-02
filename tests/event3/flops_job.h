/*
 * flops_job.h
 *
 *  Created on: Aug 12, 2010
 *      Author: sdaniele
 */

#ifndef FLOPS_JOB_H_
#define FLOPS_JOB_H_

#define BODY(another_op)					\
double dummy = ::std::numeric_limits<double>::max();		\
		for(size_t i=0; i<(1UL<<logflops); i++){	\
			dummy /= 1.01;				\
			another_op;				\
}								\




#include <limits>
#include <string>
#include <iostream>

namespace calc{


void flops_job(size_t logflops){
	BODY((void*)0);
}


template <class T>
void flops_job(size_t logflops, T msg){
	BODY(::std::cout << msg);
}

template <class T, class D>
void flops_job(size_t logflops, T msg, D delim ){
	BODY(::std::cout << msg << delim);
}



}

#endif /* FLOPS_JOB_H_ */
