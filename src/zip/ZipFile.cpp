#include "zip/ZipFile.h"

CZipFile::CZipFile(const char* name) :
m_name(name)
{
    
}

CZipFile::~CZipFile()
{

}

const char* CZipFile::GetName() const
{
    return m_name.c_str();
}
