/*
Verify that the expr in if/while statements has bool type
*/

int main() {

	if (true) {} //ok

	while( (1 + 2) > 3) {} //ok

	if (1) {} // '1' does not have bool type

	while (5*6.3 - false) {} // expr doesn't have bool type, so show only expr's error, not while statement's error

	while ("a") {}

	return 0;
}