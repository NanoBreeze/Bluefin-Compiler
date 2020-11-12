/*
Verifies global variable declaration, including ones without assignemnt, 
compile-time expr assignment, casting, and dynamic assignment
*/

int foo() {
	return 3;
}

int a;			// no expr
int b = 7;		// primaryId expr
int c = 8 + 9 * 2; // larger expr, requires constant folding

void hi(int a) { // check if global vars can be defined after funcDefs
	a + 1;
}

int d = foo(); // requires a special llvm.global_ctor for this function call entry
int e = foo() + 9;
bool f;
float g;  
float h = 5;  // 5 will need to be casted
float i = 5 + foo();  // 5 and 'foo' will need to be casted

float j = e = 4*foo();  // The subexpr 'e = 4' will need to be casted into a float
