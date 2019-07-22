/*
This program tests that the 'override' keyword is used correctly. Specifically, it tests that
- override can occur only in methods, not functions
- override applies only if the method truly does override the base class' method with same name, return type, and param types
*/

struct Base {
	void f(int a, bool b) {}
	float g() {}
	int i;
};

struct FakeDer {
	void f(int a, bool b) override{} // no superclass
};

struct Der extends Base {
	void f(int a, int b) override{} // param list doesn't match
	int g() override {} 			// return type doesn't match, (promotable doesn't count)
	void h() override {} 			// no matching name
	int i() override {} 			// base class member not a method
	void f(int a, bool b) {}		// missing 'override' specifier
	float g() override {} 			// OK
	float g() {}					// missing 'override' specifier
};

float g() override {}				// override may only appear in methods