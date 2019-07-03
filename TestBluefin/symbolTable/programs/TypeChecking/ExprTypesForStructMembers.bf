struct First {
	int a;
	bool b;
	int f() { return a; }
};

struct Second {
	First one;
	float two;
	float g(float x) { return x; }
};


int main() {
	First first;
	first.a + 5.2;
	while (first.b) {}
	
	Second second;
	second.one;
	second.one.b;
	second.one.a * second.two;
	second.one.f() - second.g(5);
}