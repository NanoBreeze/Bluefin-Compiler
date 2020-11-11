/*
If a struct has a parent, its ctor shall call its parent's ctor 
*/

struct Base {
	int a = 234;
};

struct Der extends Base {
	int b;
};

struct DerDer extends Der {
	int b;
};

DerDer derder; 

void foo() {
	DerDer derderTwo;
}