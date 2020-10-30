/*
Verifies that while statements work correctly, including recursive ones
Also checks that if statement can be placed inside a while statement's body
*/

void foo(int a) {

	a + 1;

	while (a > 2) {
		a + 3;
		while (a < 4) {}
		if (a == 6) {}
		else {}
	}

	a + 5;
}