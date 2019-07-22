/*
We expect a derived class to be able to use its base class' members and methods
*/

struct Base {
	int a;
	void f() {}
};

struct Der extends Base {
	void g() {
		a = 5;
		f();
		b = 1;
	}
};
