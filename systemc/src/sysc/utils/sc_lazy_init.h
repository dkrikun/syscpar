/*
 * sc_lazy_init.h
 *
 *  Created on: Sep 6, 2010
 *      Author: sdaniele
 */

#ifndef SC_LAZY_INIT_H_
#define SC_LAZY_INIT_H_

#include "tbb/atomic.h"

namespace sc_core {

template <class T>
class lazy {
	tbb::atomic<T*> value;
public:
	lazy() : value() {}
	~lazy() {delete value;}
	T& get() {
		if( !value ) {
			T* tmp = new T();
			if( value.compare_and_swap(tmp,0)) delete tmp;
		}
		return *value;
	}
};

}

#endif /* SC_LAZY_INIT_H_ */
