#include "ErrorListener.h"
#include <iostream>

using namespace bluefin;

void ErrorListener::err(const string errMsg) {
	msg += errMsg + "\n";
}
