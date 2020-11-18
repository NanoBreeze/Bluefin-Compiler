/*
Call a virtual method by using the vptr. Applies to both internal and external method calls
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
		one(val);
		return val;
	}

	void foo() {
		virtualMethod();				// calls Base::virtualMethod() since we don't override
	}
};

void foo() {
	Base base;
	Der der;

	base.one(true) && der.one(false);	// check that virtual methods can be used in expressions
	der.virtualMethod();				// call virtual method defined only in base impl.
	base.nonVirtualMethod(1);			// make normal method call
}