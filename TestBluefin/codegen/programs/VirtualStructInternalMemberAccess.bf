/*
If a base class contains vptr, ensure it and derived class can still do internal member accesses
*/

struct Base {
	virtual void one() {
		a = 4;
	}
	int a = 0;
};

struct Der extends Base {
	int b;
	int c;
	void here() {
		a = 5;
		b = 5;
	}

	void one() override {
		a = 123;
		b = 123;
		c = 123;
	}
};