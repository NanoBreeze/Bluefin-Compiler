/*
The type of some of the returned variables aren't compatible with the return type of the function
*/

void foo() {
	return 1; // can't return value
}

void zoo() {
	return; // no problem
}

int bar() {
	return; // missing value here. This is like returning 'void', which doesn't match with int
}

int car() { // missing return statement in direct function

	if (true) {
		return 1;	
	}
}

struct A {
	int one() {
		return true; // true doesn't convert to int
	}

	float two() {
		return 4; // type promotion, ok	
	}

	float three() {
		return 4*true+2; // not okay, the expr corresponding to '4*true+2' is not valid	
	}
};

struct B {};
struct C extends A {};

A returnA() {
	B b;
	return b; // Since B doesn't inherit from A, this is invalid
}

A returnParent() {
	C c;
	return c; //ok because C is A's subtype
}

C returnChild() {
	A a;
	return a; //invalid, can't assign child to parent
}
