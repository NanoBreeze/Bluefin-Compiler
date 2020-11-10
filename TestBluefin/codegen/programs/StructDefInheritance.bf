/*
Verifies that structs with inheritance are generated correctly
*/

struct Base {
	int a;
};

struct Der extends Base {
	int b;
};

struct DerDer extends Der {
	int c;
	Base base;
	Der der;
};