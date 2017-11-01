#ifndef SCOPED_PTR_H_
#define SCOPED_PTR_H_

#include "./console.h"

template <typename T>
struct ScopedPtr {
	ScopedPtr(T * ptr) : ptr_(ptr) {}
	~ScopedPtr() {
		delete ptr_;
	}

	T * operator -> () {
		return ptr_;
	}

	T * get() {
		return ptr_;
	}
private:
	ScopedPtr(const ScopedPtr &);
	T * ptr_;
};


#endif /* SCOPED_PTR_H_ */
