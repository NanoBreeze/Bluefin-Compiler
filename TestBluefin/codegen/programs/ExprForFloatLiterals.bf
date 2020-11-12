/*
Verifies that basic expressions for float literals are okay
No type promotions here. All types are float. 
We use nice rounded float numbers because their floating point impl is nicer for the eye
*/

void foo(float val) {

	1.0+2.1*3.3; // expect constant folding so no IR generated
	val+5.0 - 9.5*val - val/3.5; 
	
	val > 5.0;
	val >= 5.0;
	val < 5.0;
	val <= 5.0;

	val == 5.0;
	val != 5.0;
	-val;

	(val+1.0)*2.5;

}
