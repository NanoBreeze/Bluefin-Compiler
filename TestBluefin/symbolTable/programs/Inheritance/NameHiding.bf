/*
If derived class has member fields with same name as base class, we expect derived fields to hide base class' 
*/

struct Base {
	int a;
	bool b;
	
	float f() {}
};

struct Der extends Base {
	bool a;
	Base b;
	
	void f() {} // since we're just testing resolution and declarations, we can only distinguish Der::f() from Base::f() based on return type, not number of params
};

int main() {
	Der der;
	der.f(); 
	der.a;
	der.b;
}