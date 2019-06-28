
#include "FunctionSymbol.h"
#include "Symbol.h"
#include <memory>

using namespace bluefin;
using std::shared_ptr;


void FunctionSymbol::attachParam(shared_ptr<Symbol> param) {
	params.push_back(param);
}
