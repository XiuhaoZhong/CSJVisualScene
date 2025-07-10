#ifndef __CSJSTRINGTOOL_H__
#define __CSJSTRINGTOOL_H__

#include <string>

class CSJStringTool {
public:
    CSJStringTool() = default;
    ~CSJStringTool() = default;

    static std::string wstring2string(std::wstring& wstr);
    static std::wstring string2wstring(std::string& str);
};

#endif // __CSJSTRINGTOOL_H__