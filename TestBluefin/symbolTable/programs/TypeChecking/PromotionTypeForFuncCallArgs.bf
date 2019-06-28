int f(int a) {
	return 1;
}

float g(float a) {
	return 2;
}

void h(int a, int b, float c) {
}

int main() {
	f(5);
	g(6);
	{
		h(1,2,-3);
	}
}