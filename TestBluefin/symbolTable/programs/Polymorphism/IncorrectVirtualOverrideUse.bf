/*
Verifies that the 'override' and 'virtual' keywords can only be applied to methods. 
We also can't apply 'override' to a method in a class without a superclass
*/


struct FakeDer {
	void f(int a, bool b) override{} // no superclass, cannot use override
};

float g() override {}				// 'override' keyword may only appear in methods
virtual float h() {}				// 'virtual' keyword may only appear in methods