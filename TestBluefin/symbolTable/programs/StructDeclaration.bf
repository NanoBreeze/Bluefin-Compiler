struct A {
	int a;
	int b;
	void f() {}
	int c;
} ;

struct B {
	int a = 9;
	int d = a;
} ;

struct C {
	A a;
	B b;
	int c;
	B bb = b;
};