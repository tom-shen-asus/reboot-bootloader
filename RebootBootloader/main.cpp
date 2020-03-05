// Copyright (c) Microsoft. All rights reserved.


//
// rebootbootloader
//
//   Utility to reboot bootloader from the command line.
//

#include <ppltasks.h>
#include <collection.h>
#include <string>
#include <vector>
#include <sstream>
#include <iostream>
#include <fileapi.h>
#include <fstream>

std::string execute(std::string cmd)
{
	std::string file_name = "result.txt";
	std::system((cmd + " > " + file_name).c_str()); // redirect output to file

	// open file for input, return string containing characters in the file
	std::ifstream file(file_name);
	return { std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>() };
}


int main(Platform::Array<Platform::String^>^ args) {
	HANDLE hFile;
	char buffer[64] = "bootonce-bootloader";
	std::string emmcBoot("Harddisk0");
	std::string sdBoot("Harddisk1");
	std::string boot = execute("wmic path Win32_BootConfiguration get Caption");
	std::cout << boot << '\n';

	if (boot.find(emmcBoot) != std::string::npos) {
		std::cout << "emmc boot" << '\n';
		hFile = CreateFile(
			L"\\\\?\\GLOBALROOT\\Device\\Harddisk0\\Partition3",// eMMC misc partiton path
			GENERIC_ALL,
			FILE_SHARE_READ | FILE_SHARE_WRITE,
			NULL,
			OPEN_EXISTING,
			FILE_FLAG_NO_BUFFERING | FILE_FLAG_RANDOM_ACCESS,
			NULL);
	} else if (boot.find(sdBoot) != std::string::npos) {
		std::cout << "sd boot" << '\n';
		hFile = CreateFile(
			L"\\\\?\\GLOBALROOT\\Device\\Harddisk1\\Partition3",// eMMC misc partiton path
			GENERIC_ALL,
			FILE_SHARE_READ | FILE_SHARE_WRITE,
			NULL,
			OPEN_EXISTING,
			FILE_FLAG_NO_BUFFERING | FILE_FLAG_RANDOM_ACCESS,
			NULL);
	}
	else {
		std::cout << "unknow boot" << '\n';
		hFile = CreateFile(
			L"\\\\?\\GLOBALROOT\\Device\\Harddisk0\\Partition3",// eMMC misc partiton path
			GENERIC_ALL,
			FILE_SHARE_READ | FILE_SHARE_WRITE,
			NULL,
			OPEN_EXISTING,
			FILE_FLAG_NO_BUFFERING | FILE_FLAG_RANDOM_ACCESS,
			NULL);

	}


	if (hFile != INVALID_HANDLE_VALUE)
	{
		DWORD errCode = GetLastError();
		if (errCode == 0)
			std::wcerr << L"OK: Find a misc partition" << std::endl;
		else
			std::wcerr << L"OK: Something error, status code: " << errCode << std::endl;

		if (WriteFile(hFile, buffer, 512, NULL, NULL)) {
			std::wcerr << L"OK: Ready reboot bootloader " << std::endl;
			system("c:\\windows\\system32\\shutdown /r /t 0 \n\n");
		} else {
			errCode = GetLastError();
			std::wcerr << L"OK:  write command fail , status code: " << errCode << std::endl;

		}
	} else {
		DWORD createFileError = GetLastError();
		std::wcerr << L"Could not find a misc partition! Giving up. Error code: " << createFileError;
	}

	if (hFile != NULL) CloseHandle(hFile);


	return 0;
}