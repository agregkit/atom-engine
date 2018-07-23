#ifndef SettingsH
#define SettingsH

#include <string>
#include <vector>
#include <map>
#include <sstream>

typedef std::map<std::string, std::string> TSection;
typedef std::vector<std::string> TKeys;

template <typename T>
T lexical_cast(const std::string& str)
{
    T var;
    std::stringstream ss;
    ss.str(str);
    ss >> var;
    return var;
}

class CSettings {
public:
	CSettings();
	~CSettings();

	void loadFromFile(const std::string& fileName);

	TKeys getSection(const std::string& section) const;

	const std::string& getString(const std::string& section, const std::string& key, const std::string& defValue) const;

	template<class T> T getValue(const std::string& section, const std::string& key, T defValue) const {
		std::map<std::string, TSection>::const_iterator sec = sections_.find(section);
		if (sec != sections_.end()) {
			TSection::const_iterator val = sec->second.find(key);
			if (val != sec->second.end()) {
				try {
					T value = lexical_cast<T>(val->second);
					return value;
				} catch(...) {
					return defValue;
				}
			} else {
				return defValue;
			}
		} else {
			return defValue;
		}
	}
private:
	std::map<std::string, TSection> sections_;
};

#endif