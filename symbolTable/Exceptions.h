#pragma once

#include <exception>
#include <iostream>

namespace bluefin {

	using std::exception;
	using std::string;

	class ReclarationException : public exception
	{
	public:

		ReclarationException(const string redefinedId) :
			redefinedId{ redefinedId }
		{}

		const char* what() const override {
			return
				("Tried to redefine an existing definition in scope."
					"Offending id is " + redefinedId).c_str();
		}

	private:
		const string redefinedId;
	};
}
