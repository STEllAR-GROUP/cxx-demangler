/* Copyright (c) 2012 Michael LeSane
 * 
 * Distributed under the Boost Software License, Version 1.0. (See accompanying
 * file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
*/

#include <iostream>
#include <vector>
#include <string>
#include <algorithm>

#include <cstdio>
#include <cstdlib>

#include <algorithm> 
#include <functional> 
#include <cctype>
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
		std::string out = "";
		if(!consume(str,"?")) return NULL;

		std::string scope;

		basicName bN;
		unqualifiedTypeCode uTC;
		storageClass sC;
		qualification q;
		qualifiedTypeCode qTC;

		bN.parse(str);
		std::string bn = bN.toString();
		
		if(consume(str,"@"))
		{
			//basicName, scope, unqualifiedtypecode, storageclass
			scope = "global";

			uTC.parse(str);
			
			sC.parse(str);
			
			out = out
			.append(sC.toString())
			.append(" ")
			.append(
				uTC.toString
				(
					bn
				)
			)
			.append(";")
			;
		}
		else
		{
			//basicName, scope, qualification, qualifiedtypecode, storageclass
			scope = "public:";
			q.parse(str);
			consume(str,"@");

			qTC.parse(str);

			if(qTC.isData) scope = "public: static";

			sC.parse(str);

			if(str_match(bn,"\\q")) bn = q.contents[0];
		else	if(str_match(bn,"~\\q")) bn = std::string("~").append(q.contents[0]);
			
			out = out
			.append(scope)
			.append(" ")
			.append(sC.toString())
			.append(" ")
			.append(
				qTC.toString
				(
					q.toString().append("::").append(bn)
				)
			)
			.append(";")
			;
		}

		global_backref.clear();

		return rmws(trim(out));
	}

	std::string demangle(std::string str){
		global_backref.clear();
		std::string s = parseMangledName(str);
		if(str.length() > 0) std::cout << "str remains: " << str << std::endl;
		return s;
	}
}

int main(){
	std::string args[] = 
	{
		"??$ConvertTo@DH@@YADABH@Z",
		"?xyz@?$abc@V?$def@H@@PAX@@YAXXZ",
		"Z",
		"?myglobal@@3HA",
		"?myStaticMember@myclass@@2HA",
		"?myconstStaticMember@myclass@@2HB",
		"?myvolatileStaticMember@myclass@@2HC",
		"?myglobal@@3HA",
		"?myvolatile@@3HC",
		"?Fv_Lg@@YAOXZ",
		"?Fc_i@@YAHD@Z",
		"?Ff_i@@YAHM@Z",
		"?Fg_i@@YAHN@Z",
		"?Fii_i@@YAHHH@Z",
		"?Fiii_i@@YAHHHH@Z",
		"?Fv_Sc@@YACXZ",
		"?Fv_Uc@@YAEXZ",
		"?Fv_Ui@@YAIXZ",
		"?Fv_Ul@@YAKXZ",
		"?Fv_Us@@YAGXZ",
		"?Fv_c@@YADXZ",
		"?Fv_f@@YAMXZ",
		"?Fv_g@@YANXZ",
		"?Fv_i@@YAHXZ",
		"?Fv_l@@YAJXZ",
		"?Fv_s@@YAFXZ",
		"?Fv_v@@YAXXZ",
		"?Fv_v_cdecl@@YAXXZ",
		"?Fv_v_fastcall@@YIXXZ",
		"?Fv_v_stdcall@@YGXXZ",
		"?Fi_i@@YAHH@Z",
		"?Fie_i@@YAHHZZ",
		"??4myclass@@QAEAAV0@ABV0@@Z",
		"??4Class1@TestLib@@QAEAAV01@ABV01@@Z",
		"?FA10_i_i@@YAHQAH@Z",
		"??0nested@@QAE@XZ",
		"??0nested@myclass@@QAE@XZ",
		"??1nested@myclass@@QAE@XZ",
		"??0myclass@@QAE@XZ",
		"??1myclass@@QAE@XZ",
		"??1nested@@QAE@XZ",
		"??0myclass@@QAE@H@Z",
		"??Emyclass@@QAE?AV0@XZ",
		"??Hmyclass@@QAE?AV0@H@Z",
		"??Emyclass@@QAE?AV0@H@Z",
		"?Fi_i@myclass@@QAEHH@Z",
		"?Fi_i@nested@myclass@@QAEHH@Z",
		"?f@Class1@TestLib@@QAEPADPAD0@Z",
		"?Fi_i@nested@@QAEHH@Z",
		"?Fv_v_fastcall@myclass@@QAIXXZ",
		"?Fv_v_stdcall@myclass@@QAGXXZ",
		"?Fv_v_cdecl@myclass@@QAAXXZ",
		"?myfnptr@@3P6AHH@ZA",
		"?Fx_i@@YAHP6AHH@Z@Z",
		"?Fv_Vi@@YA?CHXZ",
		"?Fv_Ci@@YA?BHXZ",
		"?Fis_i@myclass@@SAHH@Z",
		"?m@C@@SAPAV1@XZ",
		"?Fv_Ri@@YAAAHXZ",
		"?Fv_PPv@@YAPAPAXXZ",
		"?FPi_i@@YAHPAH@Z",
		"?myarray@@3PAHA",
		"?f@@YAPADPADPAF1@Z",
		"?Fv_Pv@@YAPAXXZ",
		"?Fmyclass_v@@YAXVmyclass@@@Z",
		"?Fmxmx_v@@YAXVmyclass@@P6AHH@Z01@Z",
		"?Fxix_i@@YAHP6AHH@ZH0@Z",
		"?Fxx_i@@YAHP6AHH@Z0@Z",
		"?Fxxi_i@@YAHP6AHH@Z00H@Z",
		"?Fxxx_i@@YAHP6AHH@Z00@Z",
		"?Fxyxy_i@@YAHP6AHH@ZP6AHF@Z01@Z",
		"?Fxxx_i@@YAHP6AHH@Z00@Z",
		"?f@@YA?AW4E@@W41@@Z",
		"Z"
	};

	int length = sizeof(args)/sizeof(std::string);
	for(int i = 0; i  < length; i++)
	{
		if(args[i][0]=='Z') break; //debugging stop-code; nothing ever begins with "Z"
		std::cout << "I:\t" << args[i] << std::endl;
		std::string result = cxx_demangler::demangle(args[i]);
		std::cout << "O:\t" << result << std::endl << std::endl;
	}
	std::cout << gcc_demangler::demangle("_Z1fM1AKFvvE") << std::endl;
	return 0;
}
