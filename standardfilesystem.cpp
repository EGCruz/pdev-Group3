#include "standardfilesystem.h"
#include <stdio.h>
#include <sys/stat.h>

#include <iostream>

class StandardFile : public IFile
{
public:
	StandardFile(FILE* handle, const char* filename) : m_fileName(filename), m_fileHandle(handle)
	{
	}

	~StandardFile()
	{
		fclose(m_fileHandle);
	}

	const char* filename() const override
	{
		return m_fileName.c_str();
	}

	size_t length() const override
	{
		struct stat st;
		fstat(fileno(m_fileHandle), &st);
		return st.st_size;
	}

	size_t pos() const override
	{
		return ftell(m_fileHandle);
	}

	bool seek(size_t pos) override
	{
		// fseek returns 0 if successful
		return fseek(m_fileHandle, pos, SEEK_SET) == 0;
	}

	bool read(char* buffer, size_t size) override
	{
		size_t bytesRead = fread(buffer, 1, size, m_fileHandle);
		return bytesRead == size;
	}

	bool write(const char* buffer, size_t size) override
	{
		size_t bytesWritten = fwrite(buffer, 1, size, m_fileHandle);
		return bytesWritten == size;
	}
private:
	// Non-copyable, Non-assignable
	StandardFile(const StandardFile& other) {}
	StandardFile& operator = (const StandardFile& other) { return *this; }

	std::string m_fileName;
	FILE* m_fileHandle;
	size_t m_fileLength;
};

StandardFileSystem::StandardFileSystem(const char* rootDirectory) : m_rootDirectory(rootDirectory)
{
	// Make sure we always have a trailing slash
	if (m_rootDirectory[m_rootDirectory.length() -1] != '\\' &&
		m_rootDirectory[m_rootDirectory.length() -1] != '/')
		m_rootDirectory += "\\";
}

StandardFileSystem::~StandardFileSystem()
{

}

IFile * StandardFileSystem::createFile(const char * filename)
{
    std::cout << "Entered createFile (infunction)" << std::endl;
	std::string filePath = convertFilePath(filename);

	if (fileExists(filePath.c_str()))
		return nullptr;

	FILE* f = fopen(filePath.c_str(), "wb");
	if (!f)
		return nullptr;
	return new StandardFile(f, filename);
}

IFile * StandardFileSystem::openFile(const char * filename)
{
	std::string filePath = convertFilePath(filename);

	FILE* f = fopen(filePath.c_str(), "rb");
	if (!f)
		return nullptr;
	return new StandardFile(f, filename);
}

bool StandardFileSystem::fileExists(const char * filename)
{
	std::string filePath = convertFilePath(filename);

	struct stat st;
	int result = stat(filePath.c_str(), &st);
	return result == 0;
}

bool StandardFileSystem::deleteFile(const char* filename)
{
	std::string filePath = convertFilePath(filename);
	return remove(filePath.c_str()) == 0;
}

std::string StandardFileSystem::convertFilePath(const char* path)
{
	return m_rootDirectory + path;
}
