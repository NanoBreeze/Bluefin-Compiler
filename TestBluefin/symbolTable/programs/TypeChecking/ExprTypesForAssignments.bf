struct First {
	int a;
};

struct Second {
	First one;
};

First f() {}


int main() {
	First first = f();
	
	float x = 5+first.a;
	
	Second second;
	float y = second.one.a;
}