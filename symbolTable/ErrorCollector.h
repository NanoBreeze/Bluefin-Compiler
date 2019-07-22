#pragma once

#include <string>

namespace bluefin {
	
	using std::string;

	class ErrorCollector
	{
	public:

		static const string OVERRIDE_NO_FUNCTION; //= "Override specifier may not appear in a function";
		static const string OVERRIDE_MISSING_SUPERCLASS;// = "Override specifier can only be used on methods whose class contains a superclass";
		static const string OVERRIDE_UNRESOLVED_NAME;// = "Base class doesn't contain any symbol with same name";
		static const string OVERRIDE_RESOLVED_NAME_BUT_NOT_METHOD;// = "Override of base class member must be a method";
		static const string OVERRIDE_UNMATCHED_RETURN_TYPE;// = "Return type of methods must match";
		static const string OVERRIDE_UNMATCHED_PARAMETER_TYPES;// = "Parameter types must match";
		static const string OVERRIDE_SPECIFIER_NEEDED;// = "All overriding methods must contain 'override' specifier";
		static const string OVERRIDE_SUCCESSFUL;// = "Override OK!";

		void err(const string msg);
		void success(const string msg);
		inline string getMsg() const { return totalMsg; }

	private:
		string totalMsg;
		string errMsg;
		string successMsg;
	};
}
