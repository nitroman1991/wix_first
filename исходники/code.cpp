#include <windows.h>
#include <Msiquery.h>
#include <msi.h>
#include <fstream>
#include <string>
#pragma comment(linker, "/EXPORT:TargetFunction=_TargetFunction@4")

#define DEBUG
#define DATALEN 10
#define TIMELEN 8

__declspec(dllexport) bool getpath(CHAR *dest, CHAR *source, CHAR *filename) //выписывает полный путь до файла в нужном нам виде
{   
	CHAR *head_of_dest = dest;
	CHAR *head_of_source = source;
	CHAR *head_of_filename = filename;

	char *ptr = source; //в source будем передавать входную строку с данными из msi
	while(*ptr != '\0' && *ptr != ';') ++ptr;
	if (*ptr == '\0') return false;
	if (*ptr == ';') ++ptr;  //нашли начало пути к папке

	while(*ptr != '\0')
	{
		*dest = *ptr;
		if(*ptr == '\\')
		{
			++dest;
			*dest = '\\'; //удваиваем обратный слэш везде, где встретим
		}
		++dest;
		++ptr;
	}
	
	while (*filename != '\0') //дописываем имя файла
	{
		*dest = *filename;
		++dest;
		++filename;
	}

	*dest = '\0';

	dest = head_of_dest;
	source = head_of_source;
	filename = head_of_filename;

	return true;
}

//extern "C" 
extern "C" __declspec(dllexport) UINT __stdcall TargetFunction(MSIHANDLE hInstall)
{
	CHAR data_from_msi[TIMELEN + DATALEN + MAX_PATH + 2] = {0};
	DWORD dataLen = TIMELEN + DATALEN + MAX_PATH + 2;
    
	MsiGetPropertyA(hInstall, "CustomActionData", data_from_msi, &dataLen);
	
	CHAR filename[11] = "MyFile.txt";
	CHAR path_to_file[MAX_PATH] = {0};
	getpath(path_to_file, data_from_msi, filename);
	
	std::fstream file;
	file.exceptions(std::ios::failbit | std::ios::badbit);
	try
	{
		file.open(path_to_file, std::ios::_Nocreate);
		file.close();
		file.open(path_to_file, std::ios::app);
		std::string str(data_from_msi);
		file.seekg(std::ios::end);
		file << " " << str;
		file.close();
	}
	catch(std::fstream::failure e)
	{
		MessageBoxA(NULL, "Some problems occured when we trying to work with file, installation aborted.", "Error", 0);
		return 1;
	}

	return 0;
}