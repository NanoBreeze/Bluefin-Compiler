/*
Verifies basic function definitions, especially return type and param types
TODO: handle string and user-defined struct type
*/

bool hi() { }

void hey(bool a, float b, int c) {
}

float floatOnly(float a) {}

void intOnly(int a) {}

void boolOnly(bool a) {}

int bar() {
   	hi();
	floatOnly(5.0);
	intOnly(5);
	boolOnly(true);
	boolOnly(false);
}