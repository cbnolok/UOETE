#include "uostringdictionary.h"

#include <fstream>

#include "uoppackage/uoppackage.h"
#include "uoppackage/uopfile.h"

UOStringDictionary::UOStringDictionary() :
    m_unk1(0), m_stringsCount(0), m_unk2(0)
{
}

bool UOStringDictionary::load(const std::string &path)
{
    uopp::UOPPackage package;
    package.load(path);

    std::ifstream fin = package.getOpenedStream();
    uopp::UOPFile* file = package.getFileByIndex(0,0);
    if (!file->readData(fin, nullptr))
        return false;
    auto* decompressedDataVec = new std::vector<char>();
    if (!file->unpack(decompressedDataVec, nullptr))
        return false;
    fin.close();

    const char* READMEM_dataPtr = decompressedDataVec->data();
    size_t READMEM_dataOffset = 0;

#define READMEM(dest, size) \
    memcpy(static_cast<void*>(&(dest)), static_cast<const void*>(READMEM_dataPtr+READMEM_dataOffset), (size)); \
    READMEM_dataOffset += (size);

    READMEM(m_unk1, 8);
    READMEM(m_stringsCount, 4);
    READMEM(m_unk2, 4);
    m_strings.resize(m_stringsCount);
    unsigned int stringLen = 0;
    for (unsigned int i = 0; i < m_stringsCount; ++i)
    {
        READMEM(stringLen, 2);
        m_strings[i].resize(stringLen);
        READMEM(*const_cast<char*>(m_strings[i].data()), stringLen);
    }

#undef READMEM
    return true;
}

std::string UOStringDictionary::getString(unsigned int stringIndex) const
{
    if (stringIndex >= m_strings.size())
        return std::string();
    return m_strings[stringIndex];
}

