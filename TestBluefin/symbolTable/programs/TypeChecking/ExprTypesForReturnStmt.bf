int f() {
	return 45;
}

int g() {
	int a = 9;
	return a;
}

float h() {
	int a = 1;
	return a;
}

struct First {
	int one;
};

float i() {
	First first;
	return first.one + 9;
}