/*
Verifies basic function definitions, especially return type and param types
Verifies return statements too. 
NOTE: void functions don't have to have a return statement in source code but generated IR must have it
TODO: handle string and user-defined struct type
*/

void hi() { 
	return;
}

void hey(bool a, float b, int c) {
}

float floatOnly(float a) {
	return 1.0;
}

int intOnly(int a) {
	return 1;
}

bool boolOnly(bool a) {
	return false;
}

void bar() {
   	hi();
	hey(true, 5.0, 5);
	floatOnly(5.0);
	intOnly(5);
	boolOnly(true);
	boolOnly(false);
}