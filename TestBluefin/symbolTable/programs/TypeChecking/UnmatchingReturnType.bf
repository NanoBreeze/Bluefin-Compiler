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

struct A {
	int one() {
		return true; // true doesn't convert to int
	}
};

struct B {};

A returnA() {
	B b;
	return b; // Since B doesn't inherit from A, this is invalid
}
