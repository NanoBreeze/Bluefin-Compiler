
struct First {};

First f() {}
void g(int x) {}
bool h() {}

int main() {
	f();
	g(5);
	if (h() && true) {}
}