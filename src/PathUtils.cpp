#include "PathUtils.h"
#include "Utf8.h"

#include <boost/system/error_code.hpp>

using namespace Framework;

#ifdef _WIN32

#if WINAPI_FAMILY_ONE_PARTITION(WINAPI_FAMILY, WINAPI_PARTITION_APP)

ghc::filesystem::path PathUtils::GetPersonalDataPath()
{
	auto localFolder = Windows::Storage::ApplicationData::Current->LocalFolder;
	return ghc::filesystem::path(localFolder->Path->Data());
}

#else	// !WINAPI_PARTITION_APP

#include <shlobj.h>

ghc::filesystem::path PathUtils::GetPathFromCsidl(int csidl)
{
	wchar_t userPathString[MAX_PATH];
	if(FAILED(SHGetFolderPathW(NULL, csidl | CSIDL_FLAG_CREATE, NULL, 0, userPathString)))
	{
		throw std::runtime_error("Couldn't get path from csidl.");
	}
	return ghc::filesystem::wpath(userPathString, ghc::filesystem::native);
}

ghc::filesystem::path PathUtils::GetRoamingDataPath()
{
	return GetPathFromCsidl(CSIDL_APPDATA);
}

ghc::filesystem::path PathUtils::GetPersonalDataPath()
{
	return GetPathFromCsidl(CSIDL_PERSONAL);
}

ghc::filesystem::path PathUtils::GetAppResourcesPath()
{
	return ghc::filesystem::path(".");
}

ghc::filesystem::path PathUtils::GetCachePath()
{
	return GetPathFromCsidl(CSIDL_LOCAL_APPDATA);
}

#endif	// !WINAPI_PARTITION_APP

#endif	// _WIN32

#if defined(_POSIX_VERSION)

#if defined(__APPLE__)

#include <CoreFoundation/CoreFoundation.h>
#include <Foundation/Foundation.h>

ghc::filesystem::path PathUtils::GetRoamingDataPath()
{
	NSArray *paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
	std::string directory = [[paths objectAtIndex: 0] fileSystemRepresentation];
	return ghc::filesystem::path(directory);
}

ghc::filesystem::path PathUtils::GetAppResourcesPath()
{
	NSBundle* bundle = [NSBundle mainBundle];
	NSString* bundlePath = [bundle resourcePath];
	return ghc::filesystem::path([bundlePath fileSystemRepresentation]);
}

ghc::filesystem::path PathUtils::GetPersonalDataPath()
{
	return GetRoamingDataPath();
}

ghc::filesystem::path PathUtils::GetCachePath()
{
	NSArray* paths = NSSearchPathForDirectoriesInDomains(NSCachesDirectory, NSUserDomainMask, YES);
	std::string directory = [[paths objectAtIndex: 0] fileSystemRepresentation];
	return ghc::filesystem::path(directory);
}

#elif defined(__ANDROID__)

static ghc::filesystem::path s_filesDirPath;

ghc::filesystem::path PathUtils::GetAppResourcesPath()
{
	//This won't work for Android
	return ghc::filesystem::path();
}

ghc::filesystem::path PathUtils::GetRoamingDataPath()
{
	return s_filesDirPath;
}

ghc::filesystem::path PathUtils::GetPersonalDataPath()
{
	return s_filesDirPath;
}

ghc::filesystem::path PathUtils::GetCachePath()
{
	throw std::runtime_error("Not implemented.");
}

void PathUtils::SetFilesDirPath(const char* filesDirPath)
{
	s_filesDirPath = filesDirPath;
}

#elif defined(__linux__) || defined(__FreeBSD__)

// TODO: is this an appropriate translation?
ghc::filesystem::path PathUtils::GetAppResourcesPath()
{
	return ghc::filesystem::path(getenv("HOME")) / ".local/share";
}

ghc::filesystem::path PathUtils::GetRoamingDataPath()
{
	return ghc::filesystem::path(getenv("HOME")) / ".local/share";
}

ghc::filesystem::path PathUtils::GetPersonalDataPath()
{
	return ghc::filesystem::path(getenv("HOME")) / ".local/share";
}

ghc::filesystem::path PathUtils::GetCachePath()
{
	return ghc::filesystem::path(getenv("HOME")) / ".cache";
}

#else	// !DEFINED(__ANDROID__) || !DEFINED(__APPLE__) || !DEFINED(__linux__) || !DEFINED(__FreeBSD__)

#include <pwd.h>

ghc::filesystem::path PathUtils::GetPersonalDataPath()
{
	passwd* userInfo = getpwuid(getuid());
	return ghc::filesystem::path(userInfo->pw_dir);
}

#endif	// !DEFINED(__APPLE__)

#endif	// !DEFINED(_POSIX_VERSION)

void PathUtils::EnsurePathExists(const ghc::filesystem::path& path)
{
	typedef ghc::filesystem::path PathType;
	PathType buildPath;
	for(PathType::iterator pathIterator(path.begin());
		pathIterator != path.end(); pathIterator++)
	{
		buildPath /= (*pathIterator);
		std::error_code existsErrorCode;
		bool exists = ghc::filesystem::exists(buildPath, existsErrorCode);
		if(existsErrorCode)
		{
#ifdef _WIN32
			if(existsErrorCode.value() == ERROR_ACCESS_DENIED)
			{
				//No problem, it exists, but we just don't have access
				continue;
			}
			else if(existsErrorCode.value() == ERROR_FILE_NOT_FOUND)
			{
				exists = false;
			}
#else
			if(existsErrorCode.value() == ENOENT)
			{
				exists = false;
			}
#endif
			else
			{
				throw std::runtime_error("Couldn't ensure that path exists.");
			}
		}
		if(!exists)
		{
			ghc::filesystem::create_directory(buildPath);
		}
	}
}

////////////////////////////////////////////
//NativeString <-> Path Function Utils
////////////////////////////////////////////

template <typename StringType>
static std::string GetNativeStringFromPathInternal(const StringType&);

template <>
std::string GetNativeStringFromPathInternal(const std::string& str)
{
	return str;
}

template <>
std::string GetNativeStringFromPathInternal(const std::wstring& str)
{
	return Framework::Utf8::ConvertTo(str);
}

template <typename StringType>
static StringType GetPathFromNativeStringInternal(const std::string&);

template <>
std::string GetPathFromNativeStringInternal(const std::string& str)
{
	return str;
}

template <>
std::wstring GetPathFromNativeStringInternal(const std::string& str)
{
	return Framework::Utf8::ConvertFrom(str);
}

////////////////////////////////////////////
//NativeString <-> Path Function Implementations
////////////////////////////////////////////

std::string PathUtils::GetNativeStringFromPath(const ghc::filesystem::path& path)
{
	return GetNativeStringFromPathInternal(path.native());
}

ghc::filesystem::path PathUtils::GetPathFromNativeString(const std::string& str)
{
	auto cvtStr = GetPathFromNativeStringInternal<ghc::filesystem::path::string_type>(str);
	return ghc::filesystem::path(cvtStr);
}
