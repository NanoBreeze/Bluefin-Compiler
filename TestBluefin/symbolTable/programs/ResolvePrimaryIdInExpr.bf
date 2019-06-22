int a;
float b;
void f() {
	a = 5;
	a = b;
}
bool g(float a) {
	b = 35.2;
	a = 34.6;
}

int main() {
	f();
	g(4);
	c = 9;
}