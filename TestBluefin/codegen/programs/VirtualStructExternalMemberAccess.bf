/*
If a base class contains vptr, ensure it and derived classes still allow external member accesses
*/

struct Base {
	int a;
	bool b;
	virtual void one() {}
};


struct Der extends Base {
	int a;
	void one() override {}
	virtual void two() {}
};

void foo() {
	Base base;
	base.a + 1;
	base.b = true;

	Der der;
	der.a = 2;
	der.b = false;
}
