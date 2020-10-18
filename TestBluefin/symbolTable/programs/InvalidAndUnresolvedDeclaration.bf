First a;

void foo() {}

struct Another {
	Second second;
	int a;
	int foo;
	void f() {
		a = 1;
		b = 3;
		foo();  // this will resolve to int foo, as expected (same as in C++) instead of the global foo().
		// Since we use call operator here and the resolved isn't a FunctionSymbol, we expect to broadcast a FOUND_RESOLVED_SYMBOL_BUT_NOT_FUNCSYM error
	}
};
Another b;

void g(Second b) {
	Third c;
	foo; // resolves to foo() but since we use it as a variable, we expect to broadcast a FOUND_RESOLVED_SYMBOL_BUT_NOT_VARSYM
}

struct Third {};
