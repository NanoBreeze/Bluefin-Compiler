struct First {
	float a;
};

struct Second {
	First one;
};

First f() {}


int main() {
	First first = f();
	
	float x = 5+first.a;
	
	Second second;
	int y = second.one.a;
}