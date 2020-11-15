/*
Verifies the error messages when a derived class' method contains the 'override' keyword but base class doesn't 
have a corresponding virtual method
*/

struct Base {
	virtual float a() {}
	virtual float b(int x) {}
	virtual float c(int a, bool b) {}

	int i;
	void normal(int a, bool b) {}
	void normalBaseOnly() {}

	virtual bool fromBase() {}
};

struct Der extends Base {
	int a() override {} 					// No corresponding virtual method (return type doesn't match)
	float b(float a) override {} 			// param type doesn't match
	void nonExistant() override {} 			// No corresponding virtual method
	int i() override {} 					// No corresponding virtual method. i is a variable
	void normal(int a, bool b) override {}	// Nothing to 'override' since base's is not virtual
	float c(int x, bool y) override {} 		// OK

	virtual void fromDer(float a) {}		
	virtual void normalBaseOnly() {}			
};

struct DerDer extends Der {
	bool fromBase() override {}				// OK
	void fromDer(float a) override {}		// OK
	void normal(int a, bool b) override {}	// No virtual method to override
	void normalBaseOnly() override {}		// OK, will override Der's
};
