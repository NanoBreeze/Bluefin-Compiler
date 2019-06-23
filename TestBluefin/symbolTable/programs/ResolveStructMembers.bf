struct First {
	int a;
	float b;
};

struct Second {
	float a;
	First first;
};

int main() {
	First fir;
	fir.a = 1;
	Second second;
	second.a = 2;
	second.first.a = 3;
	
	second.oops = 8;
	second.oops.hi = 8;
	second.first.oops = 9;
}