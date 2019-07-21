struct Another {
	void f() {
		a = 1;
	}
	int a;
};

int main() {
	a = 3;
	int a;
	
	{
		b = 1;
		f();
	}
	First first;
}
void f() {}
float b;
struct First{};