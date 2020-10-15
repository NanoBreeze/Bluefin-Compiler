#pragma once

#include <exception>
#include <iostream>

namespace bluefin {

	using std::exception;
	using std::string;

	class RedeclarationException : public exception
	{
	public:

		RedeclarationException(const string redefinedId) :
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


	class UnresolvedSymbolException : public exception
	{
	public:
		UnresolvedSymbolException(const string undefinedId) :
			undefinedId{ undefinedId }
		{}

		const char* what() const override {
			return
				("This symbol could not be resolved." "Its id is " + undefinedId).c_str();
		}

	private:
		const string undefinedId;
	};

	class UnresolvedStructDefException : public exception
	{
	public:
		UnresolvedStructDefException(const string typeName) :
			typeName{ typeName }
		{}

		const char* what() const override {
			return
				("This typename could not be resolved. A struct definition must exist for " + typeName).c_str();
		}

	private:
		const string typeName;
	};


	class VoidVariableDeclException : public exception
	{
	public:
		VoidVariableDeclException(const string varId) :
			varId{ varId }
		{}

		const char* what() const override {
			return
				("Tried to declare a void variable. Varaiable id is " + varId).c_str();
		}

	private:
		const string varId;
	};
}
