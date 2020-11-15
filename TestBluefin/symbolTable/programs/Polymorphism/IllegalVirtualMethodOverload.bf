/*
Verifies that if a base class has a virtual method, derived classes can have a method of same name if and only if
said method overrides it
*/

struct Base {
	virtual void foo() {}
	virtual bool bar() {}
	virtual int car(int a) {}

	void zoo() {}
};

struct Der extends Base {
	virtual void foo() override {}		// not ok
	virtual int car(bool a) {}			// not ok
};

struct DerDer extends Der {
	virtual void bar() override {}		// not ok, goes back to grandparent too

	void car() {}						// not ok, goes back to grandparent

	virtual void zoo() {}				// ok
};