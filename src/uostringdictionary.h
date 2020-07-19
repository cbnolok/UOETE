#ifndef UOSTRINGDICTIONARY_H
#define UOSTRINGDICTIONARY_H

#include <string>
#include <vector>


class UOStringDictionary
{
public:
    UOStringDictionary();
    bool load(const std::string &path);
    std::string getString(unsigned int stringIndex) const;

private:
    unsigned long long m_unk1;    // qword
    unsigned int m_stringsCount;  // dword
    unsigned int m_unk2;          // word
    std::vector<std::string> m_strings;
};

#endif // UOSTRINGDICTIONARY_H
