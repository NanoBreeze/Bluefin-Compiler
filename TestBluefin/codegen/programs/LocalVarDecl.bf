/*
Verifies local variables are declared (on the stack)
*/

void foo(int a) {
	int b;	 // undefined value
	int c = 5; // primary int
	bool d = true;
	float e = 6.0;

	int f = 7+a*2;
	float g = 5-a; // casting '5-a'
	bool h = 5>a; 

	float i = c = 9+c; // this requires casting
}