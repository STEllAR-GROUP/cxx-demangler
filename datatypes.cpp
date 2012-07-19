/* Copyright (c) 2012 Michael LeSane
 * 
 * Distributed under the Boost Software License, Version 1.0. (See accompanying
 * file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
*/

namespace cxx_demangler
{
	std::string checkExceptions(std::string s,std::string &str)
	{
		if(str_match(s,"P6")) //Function Pointer
		{
			std::string callingConvention = getNextCallingConvention(str);
			dataTypeCode rVTC;
			rVTC.parse(str);
			std::string returnValueTypeCode = rVTC.toString();
			argumentList aL;
			aL.parse(str);
			std::string argList = aL.toString();
			consume(str,"Z");
			std::string n = "";
			return n
			.append("(")
			.append(returnValueTypeCode)
			.append(" ")
			.append(callingConvention)
			.append("*)")
			.append("(")
			.append(argList)
			.append(")")
			;
		}
	else	if(str_match(s,"*")) //Pointer
		{
			std::string type = getNextDataType(str);
			return type
			.append("*");
		}
	else	if(str_match(s,"V")) //Class
		{
			basicName bN;
			bN.parse(str);
			std::string basicName = checkExceptions(bN.toString(),str);
			
			qualification q;
			q.parse(str);
			std::string qualn = q.toString();

			if(hasNumericFirstChar(basicName))
			{
				int index = (int) (basicName[0]-'0');
				std::vector<std::string> gbr = getGlobalBackRef();
				basicName = gbr[index];
			}

			//std::string qualification = 
			consume(str,"@");
			if(qualn.length()>0) return qualn.append("::").append(basicName);
			else return basicName;
		}
	else	if(str_match(s,"Q")) //Array
		{
			std::string callingConvention = getNextCallingConvention(str);
			
			dataTypeCode rVTC;
			rVTC.parse(str);
			std::string returnValueTypeCode = rVTC.toString();
			return returnValueTypeCode
			.append("[]");
		}
	else	if(str_match(s,"?")) //Modifier
		{
			storageClass sC;
			sC.parse(str);
			std::string attribute =  sC.toString();
			std::string dataType = checkExceptions(getNextDataType(str),str);

			return attribute
			.append("")
			.append(dataType);
		}
	else	if(str_match(s,"W")) //Enum
		{
			std::string out = "enum";
			std::string type = getRealType(str);
			
			qualification q;
			q.parse(str);
			std::string qualification = q.toString();
			consume(str,"@");
			
			return out
			.append(" ")
			.append(qualification)
			;
		}
	else	if(str_match(s,"&")) //Reference
		{
			storageClass sC;
			sC.parse(str);
			std::string sClass = sC.toString(); //Going by vc++filt's output conventions.  Or maybe it actually is callingConvention.
			dataTypeCode rVTC;
			rVTC.parse(str);
			std::string returnValueTypeCode = rVTC.toString();
			return
				returnValueTypeCode.append(" ")
				.append(sClass)
				.append(" ")
				.append("&");
		}
	else	return s;
	}

	std::string getNextDataType(std::string &s)
	{
		std::string options[100][2] =
		{
			{"@X",	 "void"},
			{"@",	 "@"},

			{"0",	 "0"},
			{"1",	 "1"},
			{"2",	 "2"},
			{"3",	 "3"},
			{"4",	 "4"},
			{"5",	 "5"},
			{"6",	 "6"},
			{"7",	 "7"},
			{"8",	 "8"},
			{"9",	 "9"},

			{"A",	 "&"},	//Type modifier (reference)
			{"B",	 "[Type Modifier (Volatile Reference)]"},	//Type modifier (reference)
			{"C",	 "signed char"},
			{"D",	 "char"},
			{"E",	 "unsigned char"},
			{"F",	 "short"},
			{"G",	 "unsigned short"},
			{"H",	 "int"},
			{"I",	 "unsigned int"},
			{"J",	 "long"},
			{"K",	 "unsigned long"},
			{"M",	 "float"},
			{"N",	 "double"},
			{"O",	 "long double"},

			{"PA",	"*"},

			{"P6",	"P6"},
			{"P",	 "P"},
			{"Q",	 "Q"},
			{"S",	 "[Type modifier (const volatile pointer)]"},
			{"V",	 "V"},
			{"W",	 "W"},
			{"X",	 "void"},
			{"Z",	 "Z"},
			{"?",	"?"},
			{"",	""}
		};
		int i = 0;
		while(options[i][0].length()>0)
		{
			if(consume(s,options[i][0]))
			{
				std::string result = checkExceptions(options[i][1],s);
				return result;
			}
			i++;
		}
		return "";
	}

	std::string getNextStorageClass(std::string &s)
	{
		std::string options[100][2] =
		{
			{"A",	 "normal"},
			{"B",	 "const"},
			{"C",	 "volatile"},
			{"D",	"const volatile"},
			{"Z",	 "executable"},
			{"",	""}
		};
		int i = 0;
		while(options[i][0].length()>0)
		{
			if(consume(s,options[i][0])) return options[i][1];
			i++;
		}
		return "";
	}

	std::string getNextCallingConvention(std::string &s)
	{
		if(consume(s,"A")||consume(s,"B")) return "__cdecl";
	else	if(consume(s,"C")||consume(s,"D")) return "__pascal";
	else	if(consume(s,"E")||consume(s,"F")) return "__thiscall";
	else	if(consume(s,"G")||consume(s,"H")) return "__stdcall";
	else	if(consume(s,"I")||consume(s,"J")) return "__fastcall";
	}

	std::string getNextOpCode(std::string &s)
	{
		std::vector<std::string> gbr = getGlobalBackRef();
		if(consume(s,"0")) return "";
	else	if(consume(s,"1")) return "~";
	else	if(consume(s,"2")) return "operator new";
	else	if(consume(s,"3")) return "operator delete";
	else	if(consume(s,"4")) return "operator=";
	else	if(consume(s,"5")) return "operator>>";
	else	if(consume(s,"6")) return "operator<<";
	else	if(consume(s,"7")) return "operator!";
	else	if(consume(s,"8")) return "operator==";
	else	if(consume(s,"9")) return "operator!=";
	else	if(consume(s,"A")) return "operator[]";
	else	if(consume(s,"B")) return "operator returntype";
	else	if(consume(s,"C")) return "operator->";
	else	if(consume(s,"D")) return "operator*";
	else	if(consume(s,"E")) return "operator++";
	else	if(consume(s,"F")) return "operator--";
	else	if(consume(s,"G")) return "operator-";
	else	if(consume(s,"H")) return "operator+";
	else	if(consume(s,"I")) return "operator&";
	else	if(consume(s,"J")) return "operator->*";
	else	if(consume(s,"K")) return "operator/";
	else	if(consume(s,"L")) return "operator%";
	else	if(consume(s,"M")) return "operator<";
	else	if(consume(s,"N")) return "operator<=";
	else	if(consume(s,"O")) return "operator>";
	else	if(consume(s,"P")) return "operator>=";
	else	if(consume(s,"Q")) return "operator,";
	else	if(consume(s,"R")) return "operator()";
	else	if(consume(s,"S")) return "operator~";
	else	if(consume(s,"T")) return "operator^";
	else	if(consume(s,"U")) return "operator|";
	else	if(consume(s,"V")) return "operator&&";
	else	if(consume(s,"W")) return "operator||";
	else	if(consume(s,"X")) return "operator*=";
	else	if(consume(s,"Y")) return "operator+=";
	else	if(consume(s,"Z")) return "operator-=";
	else	if(consume(s,"_0")) return "operator/=";
	else	if(consume(s,"_1")) return "operator%=";
	else	if(consume(s,"_2")) return "operator>>=";
	else	if(consume(s,"_3")) return "operator<<=";
	else	if(consume(s,"_4")) return "operator&=";
	else	if(consume(s,"_5")) return "operator!=";
	else	if(consume(s,"_6")) return "operator^=";
	}

	std::string getRealType(std::string &s)
	{
		if(consume(s,"0")) return "char";
	else	if(consume(s,"1")) return "unsigned char";
	else	if(consume(s,"2")) return "short";
	else	if(consume(s,"3")) return "unsigned short";
	else	if(consume(s,"4")) return "int";
	else	if(consume(s,"5")) return "unsigned int";
	else	if(consume(s,"6")) return "long";
	else	if(consume(s,"7")) return "unsigned long";
	}
}
