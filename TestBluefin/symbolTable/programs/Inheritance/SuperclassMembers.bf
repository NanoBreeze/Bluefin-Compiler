/*
If derived class has member fields with same name as base class, we can still access the superclass' fields and methods with the 'super' keyword. 
Since this test uses declaration and resolution, not type checking, we distinguish name hiding based on types rather than compatible assignments.
*/

struct Base {
	int a;
	bool b;
	
	void f() {}
};

struct Der extends Base {
	bool a;
	Base b;
	
	float f() {
		a;
		super.a;
		super.b;
		super.f();
	}
};

int main() {
	Der der;
	der.f();
	der.super.f();
	
	der.super.a;
	der.super.g();
}