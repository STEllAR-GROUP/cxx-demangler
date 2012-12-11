/* Copyright (c) 2012 Michael LeSane
 * 
 * Distributed under the Boost Software License, Version 1.0. (See accompanying
 * file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
*/

namespace cxx_demangler
{
	std::vector<std::string> global_backref;
	int DO_DEBUG = 0;
	int GCC_MANGLE = 0;
	int NESTED = 0;
	
	std::vector<std::string> getGlobalBackRef(){
		return global_backref;
	}

	/*
	* functions headers
	*/
	std::vector<std::string> getGlobalBackRef();	

	int is_alphanumeric(char c);
	int is_valid_first_character(char c);
	char consume1(std::string &s);
	int consume(std::string &s,std::string ss);
	std::string implode(std::string,std::vector<std::string>);
	std::string parseQualification(std::string &str);
	void debug(std::string a, std::string &b);
	std::string syscall(std::string);

	/*
	* datatypes headers
	*/
	std::string getNextDataType(std::string &s, std::vector<std::string> &backref);
	std::string getNextOpCode(std::string &s);
	std::string getNextContainer(std::string &s);
	std::string getRealType(std::string &s);
	std::string getNextCallingConvention(std::string &s);
	std::string checkExceptions(std::string s, std::string &str, std::vector<std::string> &backref);
	std::string getNextStorageClass(std::string &s);
	std::string checkPrefix(std::string &str);
	
	/*
	* main headers
	*/
	std::string demangle(std::string s);
	std::string parseFunctionTypeCode(std::string &s);
	std::string parseReturnValueTypeCode(std::string &s);
	std::string parseArgumentList(std::string &s);
	std::string parseBasicName(std::string &s);
	std::string parseMangledName(std::string &s);
	
	/*
	* Struct prototypes
	*/
	struct basicName{
		int hasOperator;
		int hasTemplate;
		int templateName;
		std::string operatorCode;
		std::string nameFragment;
		std::string templateStr;
		std::string gcc_template;
		basicName();
		void parse(std::string &str, std::vector<std::string> &backref);
		
		std::vector<std::string> local_backref;

		std::string toString();
		std::string toGCC();
		void build(std::string add);
	};

	struct argumentList{
		std::vector<std::string> args;	
		std::vector<std::string> local_backref;
		int forTemplate;
		argumentList();
		void parse(std::string &str, std::vector<std::string> &backref);
		std::string toString();
		std::string toGCC();
	};

	struct templateArg{
		std::string name;
		std::string arguments;
		std::string gcc_arguments;
		std::string qualificn;
		std::vector<std::string> local_backref;
		
		argumentList aL;
		basicName bN;

		templateArg();
		void parse(std::string &str);
		std::string toString();
		std::string toGCC();
	};
	
	struct functionTypeCode{
		std::string
			cConvention,
			returnValueTypeCode,
			argList,
			aL_gcc;		
		functionTypeCode();
		void parse(std::string &str);
		std::string toString(std::string name, std::string suffix);
		std::string toGCC();
	};
	
	struct qualification{
		std::vector<std::string> contents;
		std::vector<basicName> bN_contents;
		std::vector<std::string> local_backref;
		int initial;
		
		qualification();
		void parse(std::string &str,std::vector<std::string> &backref);
		std::string toString(std::vector<std::string> backref);
		std::string toGCC(std::vector<std::string> backref);
		std::string check(std::string, std::vector<std::string>);
	};
	
	int main();
}
