struct First {
	int a = 1;
	
	void f(int a) {
		int x = g();
		bool b = h();
	}
	
	int g() {
		return 2;
	}
	
	bool h() {
		return b;
	}
	bool b;
};