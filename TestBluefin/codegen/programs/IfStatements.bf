/*
Verifies that if statements work correctly. Include if/else, if, and recursive ifs
*/

void foo(int a) {

	if (a > 5) {
		a + 5;
		if (a > 6) { a + 6; }
		else { a - 6; }
		a + 7;
	}
	else {
		if (a < 8) {
			a - 8;	
			if (a < 9) {}
		}
		a - 5;
	}

	a + 10;

	if (a > 50) {}

	a + 60;
}