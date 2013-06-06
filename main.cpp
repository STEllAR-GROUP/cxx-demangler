/* Copyright (c) 2012 Michael LeSane
 * 
 * Distributed under the Boost Software License, Version 1.0. (See accompanying
 * file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
*/

/*
 * Immediate Todo:
 * + Merge unqualifiedTypeCode, qualifiedTypeCode into a single data structure
 * + Incorporate all function/data type codes as specified in calling_conventions
 * + Resolve __ptr64 formatting issue (type codes may influence this)
 * - Resolve storage class / calling convention issue (prefixes/suffixes)
 * - $1 + mangled-name
 * - Document everything for future reference.
 * */
 
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <sstream>

#include <cstdio>
#include <cstdlib>
#include <cctype>

#include <algorithm> 
#include <functional> 
#include <locale>

#include "./header.hpp"
#include "./functions.cpp"
#include "./structures.cpp"
#include "./datatypes.cpp"

#include "./cxa_demangle.cpp"

namespace gcc_demangler
{
	std::string demangle(std::string str)
	{
		int status;
		const char* in = str.c_str();
		char* out = abi::__cxa_demangle(in,0,0,&status);
		std::string result(out);
		return result;
	}
}

namespace cxx_demangler
{	
	/* MangledName:
		'?' BasicName Qualification '@' QualifiedTypeCode StorageClass
		'?' BasicName '@' UnqualifiedTypeCode StorageClass
	*/
	std::string parseMangledName(std::string &str){
		mangledName mN;
		mN.parse(str,global_backref);
		if(GCC_MANGLE) return mN.toGCC(global_backref);
		else return mN.toString(global_backref);
	}

	std::string demangle_debug(std::string str,int code){
		//0 - Demangle MSVC as prototype.
		//1 - Demangle MSVC, remangle as GCC.
		std::string str_original = std::string(str);
		try
		{			
			std::string solution;
			
			if(code==1) GCC_MANGLE = 1;
			
			if(!code)
			{
				solution = cxx_demangler::read_line();
				solution = rmws(trim(solution));
				std::cout
				<< "Soln:\t"
				<< solution
				<< std::endl;
			}
			
			global_backref.clear();
			
			std::string s = parseMangledName(str);
			
			GCC_MANGLE = 0;

			if(!code)
			{	int cc = eq(s,solution);
				if(!cc)
				{
					std::cout << "Out:\t" << s << std::endl;
					if(str.length() > 0) std::cout << "str remains: " << str << std::endl;
					std::cout << "Results do not match. (" << s.length() << ":" << solution.length() << ")" << std::endl;
					exit(1);
				}
			}
			
			if(str.length() > 0) std::cout << "str remains: " << str << std::endl;
			return s;
		}
		catch(std::string error)
		{
			std::cout << "Error: " << error << "\n" << str << "\n";
			return str_original;
		}
	}
	std::string demangle(std::string str,int code){
		//0 - Demangle MSVC as prototype.
		//1 - Demangle MSVC, remangle as GCC.
		std::string str_original = std::string(str);
		try
		{			
			std::string solution;
			
			if(code==1) GCC_MANGLE = 1;
			
			global_backref.clear();
			
			std::string s = parseMangledName(str);
			
			GCC_MANGLE = 0;

			return s;
		}
		catch(std::string error)
		{
			std::cout << "Error: " << error << "\n" << str << "\n";
			return str_original;
		}
	}
	std::string msvc2gcc(std::string name)
	{
		return demangle(name,1);
	}
}

int main(int argc, char** argv){
//	std::cout << "Enter Query." << std::endl;
	int i = 1;
	
	int start = 0;
	int stop = 2048;
	
	if(argc>1)
	{
		for(int i = 1; i < argc; i++)
		{
			std::string _s(argv[i]);
			std::string _dm = cxx_demangler::demangle(_s,0);
			std::cout << _dm << "\n";
		}
		return 0;

		while(1)
		{
			std::string input;
			std::cin >> input;

			std::string output = cxx_demangler::demangle(input,0);
            std::cout << output << "\n";


   		}
	}
    return 0;		
	while(1)
	{
		std::string input, input2;
		std::cin >> input;
		input2 = input;
		
		if(i>stop) break; //debugging stop-code; nothing ever begins with "Z"
		
		if(i<start)
		{
			i++;
			std::string x = cxx_demangler::read_line();
			continue;
		} //debugging skip-code; nothing ever begins with "Y"
		
		for(int j = 0; j < 100; j++) std::cout << "#";
		std::cout << std::endl;
		
		std::cout << i << ":" << std::endl;
		
		std::cout << "In:\t" << input << std::endl;
		std::string result = cxx_demangler::demangle_debug(input,0);
		std::string result2 = cxx_demangler::demangle_debug(input2,1);
		std::cout << "Out:\t" << result << std::endl;
		std::cout << "GCC:\t" << result2 << std::endl;
		std::string check_query = std::string("c++filt ").append(result2);
		std::cout << "GCC_DM:\t" << cxx_demangler::syscall(check_query) << std::endl << std::endl;
		i++;
	}
	return 0;
}
