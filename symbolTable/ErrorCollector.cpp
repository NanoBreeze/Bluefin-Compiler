#include "ErrorCollector.h"

using namespace bluefin;

const string ErrorCollector::OVERRIDE_NO_FUNCTION = "Override specifier may not appear in a function";
const string ErrorCollector::OVERRIDE_MISSING_SUPERCLASS = "Override specifier can only be used on methods whose class contains a superclass";
const string ErrorCollector::OVERRIDE_UNRESOLVED_NAME = "Base class doesn't contain any symbol with same name";
const string ErrorCollector::OVERRIDE_RESOLVED_NAME_BUT_NOT_METHOD = "Override of base class member must be a method";
const string ErrorCollector::OVERRIDE_UNMATCHED_RETURN_TYPE = "Return type of methods must match";
const string ErrorCollector::OVERRIDE_UNMATCHED_PARAMETER_TYPES = "Parameter types must match";
const string ErrorCollector::OVERRIDE_SPECIFIER_NEEDED = "All overriding methods must contain 'override' specifier";
const string ErrorCollector::OVERRIDE_SUCCESSFUL = "Override OK!";


void ErrorCollector::err(const string msg) {
	errMsg += msg + "\n";
	totalMsg += msg + "\n";
}

void ErrorCollector::success(const string msg)
{
	successMsg += msg + "\n";
	totalMsg += msg + "\n";
}
