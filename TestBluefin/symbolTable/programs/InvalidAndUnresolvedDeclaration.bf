First a;

struct Another {
	Second second;
	int a;
	void f() {
		a = 1;
		b = 3;
	}
};
Another b;

void g(Second b) {
	Third c;
}

struct Third {};