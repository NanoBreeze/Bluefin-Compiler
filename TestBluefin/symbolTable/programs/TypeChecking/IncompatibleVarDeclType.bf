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

	// 5 = 6; TODO: this isn't a VarDecl but this also shouldn't be allowed

	return 0;
}