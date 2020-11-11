/*
Verifies that a derived class can access its own elements from base class
TODO: what happens if method has polymorphism?
*/

struct Normal {
	int a;
	int wow() {
		return 9;
	}
};

struct Base {
	int a;
	float b;
	Normal norm;
};

struct Der extends Base {
	void one() {
		a = 3;			// resolves to parent class' 'a'
		int c = a+3;
	}
	int two() {
		b = 1;			// resolves to current class' 'b'
		return 2;
	}
	int b = b;			// the rhs 'b' should also resolves to Der::b
};

struct DerDer extends Der {
	int c = a;			// resolves to grandparent's 'a'
	int d = b;			// resolves to parent's 'b'
	int e = norm.a;		// resolves grandparent's struct instance, and then get field from it
	int f = norm.wow();
	int g = two();		// can call inherited methods 

	void derThree() {
		b = 4;		// even in methods, can use inherited members from parent and parent's parent
		a = 5;
	}
};