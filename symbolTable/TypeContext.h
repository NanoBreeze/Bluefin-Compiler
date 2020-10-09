#pragma once

#include <memory>
#include "Type.h"

namespace bluefin {

	class TypeContext {
	public:
		TypeContext(Type type) : evalType{ type }, promotionType{ type }
		{}

		inline Type getEvalType() const { return evalType; }
		inline Type getPromotionType() const { return promotionType; }

		inline void setPromotionType(Type type) { promotionType = type; }
	
	private:
		Type evalType; // If there were no user-defined types (struct)
		// or if struct didn't inherit from Type, then can use enum instead. We do need the type2str to get the exact 
		// user-defined type, and the struct as a subtype b/c it contains its own scope for members

		Type promotionType; // since only builtin types can be promoted (for now), so no need for actual type object
		// TODO: some types can't be promoted by language rules (eg, bool), so we should
		// separate promotable ones into a different kind of type context
		// TODO 2: How to handle polymorphism withi promotionType? To think, if this is even possible
	};
}
