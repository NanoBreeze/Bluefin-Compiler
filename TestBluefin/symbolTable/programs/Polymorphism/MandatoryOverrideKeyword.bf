/*
The 'override' keyword must be applied to a method if its base class (or recursively deep base class) has a virtual method with 
identical name and signature (param types and return type). 
Even if in between grandparent and current class, there's a variable with same name as function, as long as base class has the 
virtual method, we must apply override keyword (eg, look at variable 'h' in Der)
*/

struct Base {
	virtual void f(int a) {}
	virtual float g() {}
	virtual int h() {}

	void x() {}
};

struct Der extends Base {
	void f(int b) {}		// Must apply 'override' keyword
	int h;					// note this is the same name as Base::h(), ok
};

struct DerDer extends Der {
	void f(int b) {}		// Must apply 'override' keyword b/c grandparent has virtual method
	float g() {}			// Must apply 'override' b/c identical signature to grandparent's even though parent's isn't
	int h() {}				// Must apply 'override' 

	void x() {}				// OK, b/c Base::x() isn't virtual

};