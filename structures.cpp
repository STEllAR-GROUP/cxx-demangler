/* Copyright (c) 2012 Michael LeSane
 * 
 * Distributed under the Boost Software License, Version 1.0. (See accompanying
 * file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
*/

namespace cxx_demangler
{
	/* StorageClass:
		'A' (Normal Storage)
		'B' (Volatile Storage)
		'C' (Const Storage)
		'Z' (Executable Storage)
	*/
	struct storageClass{
		std::string sClass;
		int display;
		storageClass(){}
		void parse(std::string &str)
		{
			if(str[0]=='A'||str[0]=='Z') this->display = 0;
			else this->display = 1;	
			this->sClass = getNextStorageClass(str);
		}
		std::string toString()
		{
			if(display==1) return this->sClass;
			else return "";
		}
	};

	/* Data Type Codes

		signed char    C
		char           D
		unsigned char  E
		short          F
		unsigned short G
		int            H
		unsigned int   I
		long           J
		unsigned long  K
		float          M
		double         N
		long double    O
		pointer        P (see below)
		array          Q (see below)
		struct/class   V (see below)
		void           X (terminates argument list)
		elipsis        Z (terminates argument list)
	*/
	struct dataTypeCode{
		std::string contents;
		dataTypeCode(){}
		void parse(std::string &str)
		{
			this->contents = getNextDataType(str);
		}
		std::string toString()
		{
			return this->contents;
		}
	};

	/* Function Argument Type Lists
	Function arguments types are listed left to right using the following codes.
	If the parameter list is not void and does not end in elipsis,
	its encoding is terminated by an @. 
	*/
	struct argumentList{
		std::vector<std::string> args;	
		std::vector<std::string> local_backref;
		argumentList(){}
		void parse(std::string &str)
		{
			local_backref.clear();
			while(1)
			{
				dataTypeCode dTC;
				dTC.parse(str);
				std::string add = dTC.toString();
				if(str_match(add,"Z"))
				{
					args.push_back("..."); //Is this acceptable?
					break;
				}
				else if (str_match(add,"@"))
				{
					break;
				}
				else if(str_match(add,"void"))
				{
					args.push_back(add);
					return;
				}
				else
				{
					if(hasNumericFirstChar(add))
					{
						int index = (int) (add[0]-'0');
						int index2 = ((int)local_backref.size())-1;
						if(index > index2) add = global_backref[index];
						else add = local_backref[index];
					}
					args.insert(args.end(),add);
					local_backref.insert(local_backref.end(),add);
				}
			}
		}
		std::string toString()
		{
			std::string out;
			out = implode(",",this->args);
			return out;
		}
	};

	/* Function Calling Convention Codes

		__cdecl -> "A"
		__fastcall -> "I"
		__stdcall -> "G" 
	*/
	struct callingConvention{
		std::string cConvention;
		callingConvention(){}
		void parse(std::string &str)
		{
			this->cConvention = getNextCallingConvention(str);
		}
		std::string toString()
		{
			return this->cConvention;
		}
	};

	/* FunctionTypeCode:
		CallingConvention ReturnValueTypeCode ArgumentList
	*/
	struct functionTypeCode{
		std::string
			cConvention,
			returnValueTypeCode,
			argList;
		functionTypeCode(){}
		void parse(std::string &str)
		{
			callingConvention cC;
			cC.parse(str);
			this->cConvention = cC.toString();
			
			dataTypeCode rVTC;
			rVTC.parse(str);
			this->returnValueTypeCode = rVTC.toString();
			argumentList aL;
			aL.parse(str);
			this->argList = aL.toString();
		}
		std::string toString(std::string name)
		{
			std::string out = "";
			return out
			.append(returnValueTypeCode)
			.append(" ")
			.append(cConvention)
			.append(" ")
			.append(name)
			.append("(")
			.append(argList)
			.append(")")
			;
		}
	};

	struct functionTypeCode2{
		std::string
			container,
			cConvention,
			returnValueTypeCode,
			argList;
		functionTypeCode2(){}
		void parse(std::string &str)
		{
			storageClass sC;
			sC.parse(str);
			this->container = sC.toString();
			callingConvention cC;
			cC.parse(str);
			this->cConvention = cC.toString();

			if(consume(str,"@")) this->returnValueTypeCode = "";
			else
			{
				dataTypeCode rVTC;
				rVTC.parse(str);
				this->returnValueTypeCode = rVTC.toString();
			}
			argumentList aL;
			aL.parse(str);
			this->argList = aL.toString();
		}
		std::string toString(std::string name)
		{
			std::string out = "";
			return out
			.append(" ")
			.append(container)
			.append(" ")
			.append(returnValueTypeCode)
			.append(" ")
			.append(cConvention)
			.append(" ")
			.append(name)
			.append("(")
			.append(argList)
			.append(")")
			;
		}
	};

	/* UnqualifiedTypeCode:
		'Y' FunctionTypeCode
		'3' DataTypeCode
	*/
	struct unqualifiedTypeCode{
		std::string
			funcType,
			dataType,
			staticTypeCode;
		int
			isFunction,
			isData,
			isStatic;
		
		functionTypeCode fTypeCode;
		functionTypeCode sTypeCode;
		
		unqualifiedTypeCode(){}
		void parse(std::string &str)
		{
			this->isFunction = this->isData = this->isStatic = 0;
			if(consume(str,"Y")) //FunctionTypeCode
			{
				this->fTypeCode.parse(str);
				this->isFunction = 1;
			}
			else if(consume(str,"3")) //DataTypeCode
			{
				dataTypeCode dTC;
				dTC.parse(str);
				this->dataType = dTC.toString();
				this->isData = 1;
			}
			else if(consume(str,"S")) //static -- please review/check for rules, if any
			{
				this->sTypeCode.parse(str);
				this->isStatic = 1;
			}
		}
		std::string toString(std::string name)
		{
			if(this->isFunction) return this->fTypeCode.toString(name);
		else	if(this->isData) return this->dataType.append(" ").append(name);
		else	if(this->isStatic) return this->sTypeCode.toString(name);
		}
	};

	/* QualifiedTypeCode:
		'Q' FunctionTypeCode
		'2' DataTypeCode
	*/
	struct qualifiedTypeCode{
		std::string
			fTypeCode,
			dataType,
			staticTypeCode;
		int
			isFunction,
			isData,
			isStatic;
		functionTypeCode2 fTypeCode2;
		functionTypeCode sTypeCode;
		
		qualifiedTypeCode(){}
		void parse(std::string &str)
		{
			this->isFunction = this->isData = this->isStatic = 0;
			if(consume(str,"Q"))
			{
				this->isFunction = 1;
				this->fTypeCode2.parse(str);
			}
			else if(consume(str,"2"))
			{
				this->isData = 1;
				dataTypeCode dTC;
				dTC.parse(str);
				this->dataType = dTC.toString();
			}
			else if(consume(str,"S")) //static -- please review/check for rules, if any
			{
				this->isStatic = 1;
				sTypeCode.parse(str);
			}
			else if(consume(str,"Y")) //complex
			{
				this->isStatic = 1;
				sTypeCode.parse(str);
			}
		}
		std::string toString(std::string name)
		{
			if(this->isFunction) return this->fTypeCode2.toString(name);
		else	if(this->isData) return this->dataType.append(" ").append(name);
		else	if(this->isStatic) return this->sTypeCode.toString(name);
		}
	};

	/* See Wikipedia article for more details: https://en.wikipedia.org/wiki/Microsoft_Visual_C%2B%2B_Name_Mangling
	Operators
	Operator names are represented by a question mark followed by a single character rather than being spelled out. Examples:

		myclass::myclass          ?0
		myclass::~myclass         ?1
		myclass::operator new     ?2
		myclass::operator delete  ?3
		myclass::operator=        ?4
		myclass::operator+        ?H
		myclass::operator++       ?E
	*/
	/* BasicName:
		'?' operatorCode
		string '@
	*/
	struct basicName{
			int hasOperator;
			int hasTemplate;
			std::string operatorCode;
			std::string nameFragment;
			std::string templateStr;
			basicName(){}
			void parse(std::string &str)
			{
				this->hasOperator = 0;
				this->hasTemplate = 0;
				this->nameFragment = "";
				this->templateStr = "";
				
				if(consume(str,"?$")) //Template
				{
					templateArg t;
					t.parse(str);
					this->hasTemplate = 1;
					this->templateStr = t.toString();
					return;
				}
				else if(consume(str,"?")) //Operator
				{
					char i = str[0];
					this->operatorCode = getNextOpCode(str);
					
					if(i=='0'||i=='1') this->operatorCode = this->operatorCode.append("\\q");

					this->hasOperator = 1;
					return;
				}
				else //Name fragment
				{
					char c = consume1(str);

					int index = (int) (c-'0');
					if(index >= 0 && index <= 9 && this->nameFragment.length()==0)
					{
						std::vector<std::string> gbr = getGlobalBackRef();
						this->nameFragment = gbr[index];
						return;
					}

					while(c!='@')
					{
						this->nameFragment += c;
						c = consume1(str);
					}
					global_backref.insert(global_backref.end(),this->nameFragment);
					return;
				}
			}
			std::string toString()
			{
				if(this->hasOperator) return this->operatorCode;
				else if(this->hasTemplate) return this->templateStr;
				else return this->nameFragment;
			}
	};
	
	/* Name with Template Arguments
		Name fragments starting with ?$ have template arguments. This kind of name looks like this:

		    Prefix ?$
		    Name terminated by @
		    Template argument list

		For example, we assume the following prototype.
	*/
		templateArg::templateArg(){}
		void templateArg::parse(std::string &str)
		{
			basicName bN;
			bN.parse(str);
			this->name = bN.toString();
			
			argumentList aL;
			aL.parse(str);
			this->arguments = aL.toString();
		}
		std::string templateArg::toString()
		{
			return this->name.append("<").append(this->arguments).append(">");
		}

	/* Qualification:
		( string '@' )* 
	*/
	struct qualification{
		std::vector<std::string> contents;
		qualification(){}
		void parse(std::string &str)
		{
			while(str[0]!='@')
			{
				basicName bN;
				bN.parse(str);
				std::string add = bN.toString();
				this->contents.insert(this->contents.end(),add);
			}
		}
		std::string toString()
		{
			std::reverse(this->contents.begin(),this->contents.end());
			return implode("::",this->contents);
		}
	};
}
