/*
Verify error messages when the rhs of a variable declaration doesn't match with specified type
Includes global, local, and struct members
*/

struct A {
	float f() {
		return 4;
	}
};

struct B {
	A a;
	float b = 4;
	A aa = a; // ok
	int c = a.f(); // not ok, narrowing
	float d = a.f() = 4; // ok
};

int globInt = 123;
float glob = globInt = 99;
int h = glob = 5; // not ok

int main() {

	A first;
	A second;

	int a = 5+6*false > true; 

	int b = 6.2;  // narrowing is not allowed

	A c = "hi"; // invalid

	A d = first; // valid, actually

	bool e = true && false || 1; // rhs is not a bool type since '1' is an int

	float f = first.f(); //valid 

	float g = a = b = 34; // ok

	int h = g = 5; // not ok

	return 0;
}