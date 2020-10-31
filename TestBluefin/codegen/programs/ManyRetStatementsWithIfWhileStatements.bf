/*
Verifies multiple return statements, and that code after a return statement will be 
put in its own BasicBlock (to ensure each BasicBlocks will have more only one terminator
and it's at the end)
*/

void foo(int a) {

	a + 6;
	return; // every line after this is unreachable, technically. Create a new BasicBlock right after this return

	if (a == 1) {
		return;
	}
	else {
		return;
	}
}

int wow(int a) {
	while (a > 1) {
		if (a > 2) {
			return 2;
		}
		else {
			return 3;
		}
		return 6; // yes, we accept that this "merge" BB will have to 'callers'/predecessors
		// because 'return 2' and 'return 3' will create an "impossible" BB, into which the branching
		// instructions to "merge" are placed. But since "impossible" BBs are removed from code generation
		// nothing can call this "merge" BB
	}

	return 7;
	return 8;
	a + 9;
	return 10;
}