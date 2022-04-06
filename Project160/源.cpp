#include<iostream>
#include<string>
#include"MyJson.hpp"
using namespace std;

int main()
{
	string jsonstring = "\
{\
	\"name\":\"lier  gou\",\
	\"age\" : 26.9,\
	\"data\" : [\
	{\
		\"one\":\"chenone\",\
		\"two\" : {\
			\"love1\":\"2233\",\
			\"love2\":44444\
		}\
	},\
	{\
		\"one\":\"chen22\",\
		\"two\" : {\
			\"love1\":\"8899\",\
			\"love2\":10000\
		}\
	}\
	],\
	\"lang\":\"2cpp\"\
}\
";

	Json::MyJson mj;
	mj.ReadJson(jsonstring);
	Json::ValueArray va = mj.Get<Json::ValueArray>("data");
	for (size_t i = 0;i < va.count();i++) {
		Json::MyJson line = va.Enter(i);
		Json::ValueObject obj = line.Get<Json::ValueObject>("two");
		cout << line.Get<string>("one") << endl;
		cout << obj.Get<string>("love1") << endl;
		cout << obj.Get<int>("love2") << endl;

	}
	//va.foreach();
	//cout << jsonstring << endl;
}