
struct First {};

First f() {}
void g(float x) {}
bool h() {}
int i() {}
float j() {}

int main() {
	f();
	g(5);
	0.5 + i();
	j() + i();
	if (h() && true) {}
}