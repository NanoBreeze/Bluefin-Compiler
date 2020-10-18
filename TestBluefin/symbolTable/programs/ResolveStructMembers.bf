struct First {
	int a;
	bool f() {}
	float b;
};

float c;

struct Second {
	float a;
	float f(float x) {
		return a;
		return c;
	}
	First getFirst() {
		return first;
	}
	First first;
};

int main() {
	First fir;
	fir.f();
	fir.a = 1;
	fir.a();
	UndefinedType blah;
	blah.here;
	Second second;
	second.a = 2;
	second.a.b;
	second.first.a = 3;
	second.first.b = second.f(4.5);
	second.first.f();
	
	second.oops = 8;
	second.oops();
	second.g();
	second.oops.hi = 8;
	second.first.oops = 9;
	second.getFirst().f();
	second.getFirst().a;
	second.getFirst;
	second.getFirst.a;

	c.d;
	e.d();

	Third third;
	third.a;  // I think this will still be resolved, but above line will lead to illegal forward ref
}

struct Third {
	int a;
};