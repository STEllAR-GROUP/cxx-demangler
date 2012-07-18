std::vector<std::string> global_backref;
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

/*
* datatypes headers
*/
std::string getNextDataType(std::string &s);
std::string getNextOpCode(std::string &s);
std::string getNextContainer(std::string &s);
std::string getRealType(std::string &s);
std::string getNextCallingConvention(std::string &s);
std::string checkExceptions(std::string s, std::string &str);
std::string getNextStorageClass(std::string &s);

/*
* main headers
*/
std::string demangle(std::string s);
std::string parseFunctionTypeCode(std::string &s);
std::string parseReturnValueTypeCode(std::string &s);
std::string parseArgumentList(std::string &s);
std::string parseBasicName(std::string &s);

int main();

/*
* structures
*/
