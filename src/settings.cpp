#include "settings.h"
#include <fstream>
#include <iostream>

namespace {
	std::string sectionNameDefault = "General";
}

CSettings::CSettings()
{

}

CSettings::~CSettings()
{

}

void CSettings::loadFromFile(const std::string& fileName)
{
	sections_.clear();
	std::ifstream file(fileName, std::ios::in);
	if (file) {
		std::string curSectionName = sectionNameDefault;
		while (!file.eof()) {
			std::string str;
			std::getline(file, str);
			if (!str.empty()) {
				char first = *(str.begin());
				char last = *(str.rbegin());
				if (first == '[' && last == ']') {
					curSectionName = str.substr(1, str.length() - 2);
					sections_[curSectionName] = TSection();
				} else {
					int pos = str.find('=');
					std::string key = str;
					std::string value = "";
					if (pos != std::string::npos) {
						key = str.substr(0, pos);
						value = str.substr(pos + 1, str.length() - key.length() - 1);
					}
					TSection& section = sections_[curSectionName];
					section[key] = value;
				}
			}
		}
		file.close();
	}
}

const std::string& CSettings::getString(const std::string& section, const std::string& key, const std::string& defValue) const
{
	std::map<std::string, TSection>::const_iterator sec = sections_.find(section);
	if (sec != sections_.end()) {
		TSection::const_iterator val = sec->second.find(key);
		if (val != sec->second.end()) {
			if (!val->second.empty()) {
				return val->second;
			} else {
				return defValue;
			}
		} else {
			return defValue;
		}
	} else {
		return defValue;
	}
}

TKeys CSettings::getSection(const std::string& section) const
{
	std::map<std::string, TSection>::const_iterator sec = sections_.find(section);
	if (sec != sections_.end()) {
		TKeys keys;
		for (TSection::const_iterator p = sec->second.begin(); p != sec->second.end(); ++p) {
			keys.push_back(p->first);
		}
		return keys;
	} else {
		return TKeys();
	}
}