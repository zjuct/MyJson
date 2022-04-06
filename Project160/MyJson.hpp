#ifndef _MYJSON_HPP_
#define _MYJSON_HPP_

#include<iostream>
#include<vector>
#include<string>
#include<algorithm>
#include<sstream>
namespace Json
{

class JsonUtil {
public:
	//���ã���ȡ�ַ�����left��right��Χ�Ĳ���
	static std::string Trim(std::string& str, char left, char right) {
		size_t leftpos = str.find_first_of(left);
		size_t rightpos = str.find_last_of(right);
		return str.substr(leftpos + 1, rightpos - leftpos - 1);
	}
};


//��������ת���ĸ�����,��stringת��Ϊ��Ӧ������
class Value {
public:
	Value() = delete;
	Value(const std::string& str) :_Val(str) {

	}

	//����stringstreamʵ������ת��
	template<typename T>
	T TransferInto() {
		std::stringstream ss;
		ss << _Val;
		T val;
		ss >> val;
		return val;
	}


private:
	std::string _Val;
};

//����Json�ַ���,����ת��Ϊ��ֵ���ַ�������
class ParseJson {
public:
	ParseJson() = default;
	~ParseJson() = default;

	void ParseObj(std::string Json) {
		Json = JsonUtil::Trim(Json, '{', '}');
		size_t i = 0;
		size_t offset = 0;
		for (;i < Json.length();i++) {
			if (isspace(Json[i])) {
				continue;
			}
			char ch = Json[i];
			std::string str;
			//��ȡ��Ӧ�ļ�ֵ��
			if (ch == '{') {
				str = FetchObject(Json, i, offset);
			}
			else if (ch == '[') {
				str = FetchArray(Json, i, offset);
			}
			else if (ch == '\"') {
				str = FetchString(Json, i, offset);
			}
			else if (isdigit(ch) || ch == '-') {
				str = FetchNum(Json, i, offset);
			}
			else {
				continue;
			}
			//����ֵ�Է�������
			if (!str.empty()) {
				_KeyValue.push_back(str);
				str.clear();
			}
			i += offset;       //������ֵ��
		}
	}

	std::vector<std::string> GetKeyValue() {
		//���ؽ����
		return _KeyValue;
	}

	
private:

	//���ã���offset��ʼ���ҵ���һ����Ϊ�ո���ַ����������±�
	size_t GetFirstNotSpace(const std::string& str, size_t offset) {
		size_t i = offset;
		for (;i < str.length();i++) {
			if (!isspace(str[i])) {
				return i;
			}
		}
		return -1;
	}
	

	std::string FetchObject(std::string str, size_t startpos, size_t& offset);
	std::string FetchArray(std::string str, size_t startpos, size_t& offset);
	std::string FetchString(std::string str, size_t startpos, size_t& offset);
	std::string FetchNum(std::string str, size_t startpos, size_t& offset);

	std::vector<std::string> _KeyValue;
};

inline std::string ParseJson::FetchObject(std::string str, size_t startpos, size_t& offset) {
	//�����ֵ���Ƕ�������
	int bracket = 0;
	std::string ResultStr;
	size_t i = GetFirstNotSpace(str, startpos);
	for (;i < str.length();++i) {
		char ch = str[i];
		if (ch == '{') {
			++bracket;
		}
		else if (ch == '}') {
			--bracket;
		}
		ResultStr.push_back(ch);
		if (bracket == 0) {
			break;
		}
	}
	offset = i - startpos;
	return ResultStr;
}

inline std::string ParseJson::FetchArray(std::string str, size_t startpos, size_t& offset) {
	//�����ֵ������������
	int bracket = 0;
	std::string ResultStr;
	size_t i = GetFirstNotSpace(str, startpos);
	for (;i < str.length();++i) {
		char ch = str[i];
		if (ch == '[') {
			++bracket;
		}
		else if (ch == ']') {
			--bracket;
		}
		ResultStr.push_back(ch);
		if (bracket == 0) {
			break;
		}
	}
	offset = i - startpos;
	return ResultStr;
}

inline std::string ParseJson::FetchString(std::string str, size_t startpos, size_t& offset) {
	//������ͨ��ֵ�Ե����
	int bracket = 0;
	std::string ResultStr;
	size_t i = GetFirstNotSpace(str, startpos);
	for (;i < str.length();++i) {
		char ch = str[i];
		if (ch == '\"') {
			++bracket;
		}
		if (bracket % 2 == 0 && (ch == ':' || ch == ',')) {
			break;
		}
		ResultStr.push_back(ch);
	}
	offset = i - startpos;
	return JsonUtil::Trim(ResultStr, '\"', '\"');
}

inline std::string ParseJson::FetchNum(std::string str, size_t startpos, size_t& offset) {
	//�����ֵ��Ϊ���ֵ����
	std::string ResultStr;
	size_t i = GetFirstNotSpace(str, startpos);
	for (;i < str.length();++i) {
		char ch = str[i];
		if (ch == ',') {
			break;
		}
		ResultStr.push_back(ch);
	}
	offset = i - startpos;
	return ResultStr;
}

class MyJson {
public:
	MyJson() = default;
	~MyJson() = default;

	//��ȡJson�ַ���
	void ReadJson(std::string& _Json) {
		this->_Json = _Json;
		ParseJson pj;
		pj.ParseObj(_Json);
		_Result = pj.GetKeyValue();
	}

	template<typename T>
	T Get(const std::string& key) {
		//ͨ��������ΪTraitʵ����ȡ
		return Get(key, T());
	}

	//��ȡ��ͨ��ֵ�Ե�Get�汾
	template<typename T>
	T Get(const std::string& key, T trait) {
		typename std::vector<std::string>::iterator it = find(_Result.begin(), _Result.end(), key);
		if (it == _Result.end()) {
			return trait;
		}
		return Value(*(++it)).TransferInto<T>();
	}

	//���string����ʽ���廯
	template<>
	std::string Get(const std::string& key, std::string trait) {
		typename std::vector<std::string>::iterator it = find(_Result.begin(), _Result.end(), key);
		if (it == _Result.end()) {
			return trait;
		}
		return *(++it);
	}

	void foreach() {
		if (_Result.empty()) {
			std::cout << "��" << std::endl;
		}
		for_each(_Result.begin(), _Result.end(), [=](const std::string& str) {
			std::cout << str << std::endl;
			});
	}

protected:

	std::string _Json;
	std::vector<std::string> _Result;

};

//�����ֵ��Ϊ��������
class ValueArray:public MyJson  {
public:
	ValueArray():MyJson() {

	}

	ValueArray(std::string& str){
		ParseJson pj;
		replace(str);
		pj.ParseObj(str);
		_Result = pj.GetKeyValue();
	}

	MyJson Enter(size_t i) {
		std::string str = _Result[i];
		MyJson mj;
		mj.ReadJson(str);
		return mj;
	}

	size_t count() {
		return _Result.size();
	}
private:
	
	void replace(std::string& str) {
		size_t firstpos = str.find_first_of('[');
		size_t lastpos = str.find_last_of(']');
		str[firstpos] = '{';
		str[lastpos] = '}';
	}
};

//�����ֵ��Ϊ��������
class ValueObject :public MyJson {
public:
	ValueObject() :MyJson() {

	}

	ValueObject(std::string& str) {
		ParseJson pj;
		pj.ParseObj(str);
		_Result = pj.GetKeyValue();
	}
};

template<>
ValueArray MyJson::Get(const std::string& key, ValueArray trait) {
	typename std::vector<std::string>::iterator it = find(_Result.begin(), _Result.end(), key);
	if (it == _Result.end()) {
		return trait;
	}
	return ValueArray(*(++it));
}

template<>
ValueObject MyJson::Get(const std::string& key, ValueObject trait) {
	typename std::vector<std::string>::iterator it = find(_Result.begin(), _Result.end(), key);
	if (it == _Result.end()) {
		return trait;
	}
	return ValueObject(*(++it));
}
}

#endif