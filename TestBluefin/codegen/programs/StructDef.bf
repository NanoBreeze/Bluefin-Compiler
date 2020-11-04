/*
Verifies struct definitions, including nested members
Verifies varDecl with expr() inside struct (fields)
TODO: Add struct member access, methods, inheritance, and polymorphism
*/

int blow = 8;
int here = blow + 10;

int nice() {
	return 1;
}

struct Hey {
	int a;
	int b = a;
	float c = 123 * a;
};


struct Hi {
	int a;
	//int aa = a;
	Hey hey;
	//Hey hi = hey;
	bool b = a > blow;
};

void foo() {
	Hey hey;  // in order for StructDefs to show up, we must make a local variable with them
	Hi hi;
}