struct First {
	int a;
	bool b;
};

struct Second {
	First one;
	float two;
};


int main() {
	First first;
	first.a + 5.2;
	while (first.b) {}
	
	Second second;
	second.one;
	second.one.b;
	second.one.a * second.two;
}