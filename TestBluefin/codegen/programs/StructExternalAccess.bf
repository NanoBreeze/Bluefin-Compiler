/*
Verifies external memberAccess of struct variables.
TODO: verify method calls
TODO: verify return values of method calls too. eg) f().g().h(); or f().a;
TODO: refactor the implementation. Right now, primaryIds whose resolved symbol has type of StructSymbol
and intermediate member access both create "load" instructions, which isn't necessary
*/

struct Foo {
	int x;
	float y;
	int here() {
		return x;
	}
	int there(int a) {
		return a;
	}
};

struct Bar {
	Foo foo;
	int a;
	float b;
};

void foo() {
	Foo foo;
	foo.y;				// single member access
	foo.x + 5;			// using member access in expr
	foo.y = foo.x - 5;	// simple assignment with member access
}


void bar() {
	Bar bar;
	bar.foo;			// member access where member is a variable with struct type

	Bar barTwo;
	barTwo.foo;			// check that we're referring to barTwo's addr and not bar's

	bar.foo.x;			// chained member access

	bar.foo.y = 10; //bar.foo.x - bar.foo.y;		// chained member access used in expr and simple assignment
	bar.foo.y = bar.foo.x - bar.foo.y;		// chained member access used in expr and simple assignment
}

void method() {
	Foo foo;
	foo.here();
	int a = foo.there(5);	// VarDecl with method call with args
	foo.there(foo.x);

	Bar bar;
	bar.foo.here();
	bar.foo.there(bar.foo.x);

	a = foo.here();			// Simple assignment with method call
}

