/*
Verifies that we can assign values to local vars, global vars, and function params.
If there's name hiding, make sure to assign to correct var
To allow mutable variables, we use lots of stack space (use mem2reg to optimize later)
*/

int a = 4;
bool b;
float globalFloat;

void foo(float a) {

/*
	a = 3;		// name hiding, refer to the param instead of global
	a = a+7.5;

	if (b == true) {
		b = false;
	}
	
	b = !b;
*/

	int c;
	
	/*
	c = -c;
	{
		int c = 5;  // name hiding
		c = 6;		// refer to the 'c' in this block
	}
	c = 7; // refer to the 'c' in this block
	*/
	int d;
	globalFloat = c; // expect casting

	d = c = 5; // no casting needed
	globalFloat = c = 8;  // requires casting
	globalFloat = c = d;  // requires casting
}