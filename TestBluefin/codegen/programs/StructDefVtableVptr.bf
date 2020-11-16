/*
Verifies that vtable and vptrs are created correctly.
For vtable, order of the func ptrs matter
For vptr, only the most base virtual class should explicitly contain the vptr
*/

struct Base {
	virtual void one() {}
	virtual int two(bool a) {
		return 3;
	}
};

struct Der extends Base {
	void one() override {}

	virtual bool three() {
		return true;
	}

	int two(bool a) override {
		return 4;
	}


	void notVirtualMethod() {}	// this won't be placed in a vtable
};

struct DerDer extends Der {
	int two(bool a) override {
		return 5;
	}
	bool a;
};

struct NonVirtualBase {		// no vtable or vptr should be created
	void one() {}
};

struct VirtualDer extends NonVirtualBase {		// The structType for this should contain explicit vPtr
	virtual void here() {}
	int a;
};