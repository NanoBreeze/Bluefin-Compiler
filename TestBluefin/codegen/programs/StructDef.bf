/*
Verifies struct definitions, including nested members
Verifies varDecl with expr() inside struct, including forward referencing and internal method calls
Verifies global and local varDecl of struct types
TODO: Add struct member access, external method call, inheritance, and polymorphism
*/


int globalVar = 8;

int globalFunc() {
	return 1;
}

struct Normal {
	int a;						// no expr() for VarDecl
	float b = a + 6;			// normal referencing

	int method(int a) {
		a + b + globalVar;			// contains normal referencing, local var, and global var
		return 2;
	}

	int orange = method(99);	// normal call to method

	int c = globalFunc();		// references global function

	float d = globalVar * a + method(1); // normal internal method call with global var and member
};

struct Forwards {
	void there() {
		b - d;					// forward ref member
	}	 

	int a = first();			// forward ref method with no args
	int first() { return 1; }

	float b = second(a, c);		// forward ref method for both the method and one arg, with the other arg a not forward ref
	int second(int a, int c) { 
		return a+c+d+globalVar;		// contains forward referencing member, local var, and global
	}

	int c = d;					// forward ref variable
	int d;
};

struct MethodCalls {
	void first() { 
		int a = third(2);		// forward ref method call in local decl
		a = third(3);			// simple assignment
	}			

	void second() {
		first();				// call already defined
		third(third(third(1))); // forward ref to "third", and also chain itself
	}

	int third(int a) {
		return 1;
	}
};

struct StructMemberDecl {
	Normal norm;
	float d = norm.d;
	int a = norm.method(2);
};

Normal norm;					// global var of struct type. Ctor needs to be called

void foo() {
	Normal normal;				// local var of struct type. Expect ctor to be called
	Forwards forwards;
	MethodCalls methodCalls;
	StructMemberDecl decl;
}