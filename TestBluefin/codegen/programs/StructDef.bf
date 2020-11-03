/*
Verifies struct definitions, including nested members
TODO: Verify varDecl with expr() inside struct
TODO: Add methods, inheritance, and polymorphism
*/

struct Hey {
	int a = 5;
	bool b;
	float c;
};

struct Hi {
	int a;
	//int aa = a;
	Hey hey;
	//Hey hi = hey;
	bool b;
};

void foo() {
	Hey hey;  // in order for StructDefs to show up, we must make a local variable with them
	Hi hi;
}