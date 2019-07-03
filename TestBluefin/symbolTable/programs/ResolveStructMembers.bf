struct First {
	int a;
	bool f() {}
	float b;
};

struct Second {
	float a;
	float f(float x) {
		return a;
	}
	First first;
};

int main() {
	First fir;
	fir.f();
	fir.a = 1;
	Second second;
	second.a = 2;
	second.first.a = 3;
	second.first.b = second.f(4.5);
	second.first.f();
	
	second.oops = 8;
	second.g();
	second.oops.hi = 8;
	second.first.oops = 9;
}