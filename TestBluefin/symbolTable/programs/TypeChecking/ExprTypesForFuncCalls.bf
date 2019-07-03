
struct First {};

First f() {}
void g(int x) {}
bool h() {}
int i() {}
float j() {}

int main() {
	First first;
	f();
	g(5);
	0.5 + i();
	j() + i();
	if (h() && true) {}
}