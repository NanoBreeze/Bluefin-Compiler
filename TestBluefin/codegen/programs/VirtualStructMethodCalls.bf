/*
Call a virtual method by using the vptr
*/

struct Base {
	int a;
	virtual	void virtualMethod() {
		one(true);						// use vptr for calling 'one'. The actual 'one' imple may be in Der so no need to cast the "this" ptr to Der*
	}

	virtual	bool one(bool val) {
		nonVirtualMethod(1);			// don't use vptr when making this func call
		return !val;
	}

	void nonVirtualMethod(int val) {
		a + val;
		virtualMethod();				// use vptr for making this call
	}
};

struct Der extends Base {
	bool one(bool val) override {
		return val;
	}

	void foo() {
		virtualMethod();				// calls Base::virtualMethod() since we don't override
	}
};