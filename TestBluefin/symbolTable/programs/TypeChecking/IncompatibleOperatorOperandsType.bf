/*
Verifies error messages when the unary or binary operator is not compatible with the operands
(eg, operands can't be promoted to same type, or using '+' with user-defined type)
*/

struct A {};
struct B {};

int main() {
	A a;
	B b;
	int c;

	-5; //ok

	-5*a; // struct instance can't be multiplied
	
	5+true*3-2; //true is incompatible with int

	6.2/3 > b; // can't compare float with b

	true && false || 1; // 1 can't be converted to bool

	a + b; // invalid

	(5==6) > 3; //invalid comparison

	-a; // can't apply unary only user-defined type

	!6; // '!' can be applied to bool type only

	a=b; // they don't inherit each other so not okay

	c = 5.3; // narrowing not allowed

	c = 5*7/3; //allowed

	return 0;
}