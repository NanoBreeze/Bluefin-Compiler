/*
Verifies that basic expressions for int literals are okay
*/

void foo(int val) {

	1+2*3; // expect constant folding so no IR generated
	val+5-9*9*val-val/3; // 9*9 will automatically undergo contant folding. Note, for val*9*9, the instructions will be generated
	
	val > 5;
	val >= 5;
	val < 5;
	val <= 5;

	val == 5;
	val != 5;
	-val;

	(val+3)*3;

}