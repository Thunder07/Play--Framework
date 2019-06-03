#pragma once

#include "boost_filesystem_def.h"

namespace Framework
{
	namespace PathUtils
	{
		ghc::filesystem::path		GetAppResourcesPath();
		ghc::filesystem::path		GetRoamingDataPath();
		ghc::filesystem::path		GetPersonalDataPath();
		ghc::filesystem::path		GetCachePath();
		
#ifdef _WIN32
		ghc::filesystem::path		GetPathFromCsidl(int);
#endif	//WIN32
#if defined(__ANDROID__)
		void						SetFilesDirPath(const char*);
#endif

		void						EnsurePathExists(const ghc::filesystem::path&);

		std::string					GetNativeStringFromPath(const ghc::filesystem::path&);
		ghc::filesystem::path		GetPathFromNativeString(const std::string&);
	};
};
