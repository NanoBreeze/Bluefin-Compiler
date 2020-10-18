/*
Verify types for function calls:
1) Correct number of arguments
2) All arguments' types match
3) Resolved symbol is indeed a FunctionSymbol (this is already checked in "InvalidAndUnresolvedDeclaration")
When we're at type checking phase, all functionSymbols have already been resolved
*/

void foo(int a, bool b) {}

int bar;

struct A {
	int foo(int a, bool b) {
		return a;
	}
};

int main() {
	A a;

	foo(6.2, true); // narrowing for '6.2' is not allowed

	foo(6, true, false); // too many arguments

	foo(6); // too few arguments

	//bar(); // 'bar' is not a function. Already caught before typechecking

	foo(a.foo(6,true), true); // ok

	foo(a.foo(true), false); // a.foo(..) doesn't have enough args

	foo(a.foo(5, 1), false); // a.foo(..)'s second arg, 1, is supposed to be a bool type

	//int b = a.f; already caught before type checking

	return 0;
}