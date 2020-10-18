/*
When we use derived class, we expect to be able to access the base class' fields and member methods.
*/

struct Base {
	int a;
	float b;
	void f() {}
};

struct Der extends Base {
	bool a;
	void g() {}
};

struct DerDer extends Der {
	float h() {	return 3; }
};

int x;

int main() {
	DerDer derder;
	derder.a;
	derder.b;
	derder.f();
	derder.g();
	derder.h();
	
	derder.x;
}