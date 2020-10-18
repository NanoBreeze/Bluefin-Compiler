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
		Type evalType; // We do need the type2str to get the exact 
		// user-defined type, and the struct as a subtype b/c it contains its own scope for members

		Type promotionType; // builtin types can be promoted (eg, int => float) 
		// For inheritance, a child type can be 'promoted' into a parent type. eg) Base b = der; 
		// some types can't be promoted by language rules (eg, bool), so this value may stay fixed
		// Maybe somehow separate promotable ones into a different kind of type context
	};
}
