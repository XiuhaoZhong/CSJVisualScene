#include "CSJStringTool.h"

#include <iostream>

#ifdef __APPLE__
#include <CoreFoundation/CoreFoundation.h>
static std::wstring utf8_to_wstring(const std::string& utf8_str);
static std::string wstring_to_utf8(const std::wstring& wstr);
#elif __WIN32
#endif


std::string CSJStringTool::wstring2string(std::wstring &wstr) {
#ifdef __APPLE__
    return wstring_to_utf8(wstr);
#else
    return std::string();
#endif
}

std::wstring CSJStringTool::string2wstring(std::string &str) {
#ifdef __APPLE__
    return utf8_to_wstring(str);
#else
    return std::wstring();
#endif
}

#ifdef __APPLE__
static std::wstring utf8_to_wstring(const std::string& utf8_str) {
    if (utf8_str.size() == 0) {
        return std::wstring();
    }

    CFStringRef cf_str = CFStringCreateWithCString(kCFAllocatorDefault,
                                                utf8_str.c_str(),
                                                kCFStringEncodingUTF8);
    if (!cf_str) {
        return std::wstring();
    }

    CFIndex length = CFStringGetLength(cf_str);
    std::wstring wstr(length, L'\0');

    CFStringGetCharacters(cf_str, CFRangeMake(0, length), reinterpret_cast<UniChar *>(&wstr[0]));
    CFRelease(cf_str);

    std::wcout << L"path transfor: " << wstr << std::endl;

    return wstr;
}

static std::string wstring_to_utf8(const std::wstring& wstr) {
    if (wstr.size() == 0) {
        return std::string();
    }

    CFStringRef cf_str = CFStringCreateWithCharacters(kCFAllocatorDefault,
                                                      reinterpret_cast<const UniChar*>(wstr.data()),
                                                      wstr.size());
    if (!cf_str) {
        return std::string();
    }

    CFIndex max_size = CFStringGetMaximumSizeForEncoding(CFStringGetLength(cf_str),
                                                         kCFStringEncodingUTF8) + 1;
    std::string utf8_str(max_size, '\0');
    Boolean success = CFStringGetCString(cf_str, &utf8_str[0], max_size, kCFStringEncodingUTF8);
    CFRelease(cf_str);

    if (!success) {
        return std::string();
    }

    utf8_str.resize(strlen(utf8_str.c_str()));
    return utf8_str;
}

#else

#endif

