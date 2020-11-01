/*
Verify error messages when the rhs of a variable declaration doesn't match with specified type
*/

struct A {
	float f() {
		return 4;
	}
};

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

	int h = g = 5;

	return 0;
}