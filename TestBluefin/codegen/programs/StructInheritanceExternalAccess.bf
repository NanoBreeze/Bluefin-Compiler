/*
Verifies that external methodCalls and memberAccesses are possible. Can also
externally access a base class' methods and fields.
*/

struct Base {
	void hi() {}
	int hey(int a) {
		return a;
	}
	int x;
};

struct Der extends Base {
	int foo(int a, int b) {
		return a+b;
	}
};

struct DerDer extends Der {
	bool getTrue() {
		return true;
	}
	int z;
};


void foo() {
	DerDer derder;

	int c = derder.x;			// get grandparent's field
	c = derder.z;				// get instance's field, with simple assignment
}

void bar() {
	DerDer derder;
	derder.hi();				// call grandparent's method
	int a = derder.hey(5);		// call grandparent's method with vardecl
	a = derder.foo(1,2);		// verify simple assignment
	bool b = derder.getTrue();	// call instance's method
}