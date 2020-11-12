/*
Verifies error messages when the unary or binary operator is not compatible with the operands
(eg, operands can't be promoted to same type, or using '+' with user-defined type)
*/

struct A {};
struct B {};

struct C extends A {};
struct D extends A {};
struct E extends C {};

int main() {
	A a;
	B b;
	C structC;
	D structD;
	E structE;

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

	structC = a; // cannot assign base type to child instance

	A aaa = structE; // ok

	E bad = a; // cannot assign child to parent

	structC = structD; // they both extend A but that doesn't mean they can be assigned to each other

	int d;
	c = d = 4; // we allow chaining assignments, the type is the type of lhs (assuming they're compatible)
	
	float e;
	e = c = d = 9; // ok, because rhs of "e = ..." evaluates to int, so type promotion

	d = e = c = 9; // not ok because lhs of "d = ..." is int; and rhs is float, so no type promotion
		
	// d + 8 = 9; TODO: This really shouldn't be allowed

	// 5 = 6; TODO: this also shouldn't be allowed

	// e = c+6 = 9;; TODO: this also shouldn't be allowed

	return 0;
}