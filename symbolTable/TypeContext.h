#pragma once

#include <memory>
#include "Type.h"

namespace bluefin {
	using std::shared_ptr;

	class TypeContext {
	public:
		TypeContext(shared_ptr<Type> type) : evalType{ type }, promotionType{ type }
		{}

		inline shared_ptr<Type> getEvalType() const { return evalType; }
		inline shared_ptr<Type> getPromotionType() const { return promotionType; }

		inline void setPromotionType(shared_ptr<Type> type) { promotionType = type; }
	
	private:
		shared_ptr<Type> evalType; // This is a sp b/c Symbol::getType() returns a sp. If there were no user-defined types (struct)
		// or if struct didn't inherit from Type, then can use enum instead. We do need the type2str to get the exact 
		// user-defined type, and the struct as a subtype b/c it contains its own scope for members

		shared_ptr<Type> promotionType; // since only builtin types can be promoted, so no need for actual type object
	};
}
