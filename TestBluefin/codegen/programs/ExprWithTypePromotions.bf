/*
Verifies that automatic type promotions are correctly generated (integers turn into floats)
TODO: Add checking type promo for function call, ret val, struct member access, and method call, and varDecl
*/

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
}