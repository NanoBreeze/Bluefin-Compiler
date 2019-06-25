struct First {
	int a;
	bool b;
};

struct Second {
	First one;
};


int main() {
	First first;
	first.a + 5;
	while (first.b) {}
	
	Second second;
	second.one;
	second.one.b;
}