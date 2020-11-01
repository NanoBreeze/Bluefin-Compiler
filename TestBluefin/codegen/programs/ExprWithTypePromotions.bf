/*
Verifies that automatic type promotions are correctly generated (integers turn into floats)
Includes promotion of function call args and ret val, global/local VarDecl
TODO: Add checking type promo for struct member access, and method call
*/

int hi(float a) {
	return 3;
}

float one = 1; // global varDecl
float two = hi(1.0); // global varDecl

void foo(int val) {

	1.0+2.1*3.3; // expect constant folding so no IR generated
	val+5.0 - 9.5*val - val/2;  // for val/2, note that both 'val' and '2' have promoton type of int, but the expression has promo type of float
	val+5 - 9.5*val;  // similar to above. for val+5, each have promo type of int, but their expr has promo type of float, so they'll have to be converted too
	
	val > 5.0;
	val >= 5.0;
	val < 5.0;
	val <= 5.0;

	val == 5.0;
	val != 5.0;
	(-val > 3) == (val < 5.0);

	hi(5) + 5; // no promo here
	hi(hi(5)) + 5;
	hi(5) + 5.0;
	hi(5) == 5.0; // the type of the argument '5' should be promoted to float and the return value of hi(5) should also be promoted to float

	float three = 2+val; 
}