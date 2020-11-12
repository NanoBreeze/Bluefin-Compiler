/*
Verifies basic expressions for bools, including &&, ||, !, and true/false
*/

void hi(bool a, bool b) {
	a && b;
	a || b;

	a && true && b; // we expect the a && true to be folded into 'a'
	b || false || a;  // we expect the b || false to be folded into 'b'

	!a;
}