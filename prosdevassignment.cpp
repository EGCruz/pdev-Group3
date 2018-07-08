
#include <iostream>
#include "standardfilesystem.h"
#include "reflection.h"
#include "sampleobject.h"
#include "archive.h"

void fileSystemExample();
void reflectionExample();
void archiveExample();

int main()
{
	fileSystemExample();
	reflectionExample();
	archiveExample();
    return 0;
}

void fileSystemExample()
{
	std::cout << "FILE SYSTEM EXAMPLE" << std::endl;
	std::cout << "======================================================" << std::endl;
	StandardFileSystem* fs = new StandardFileSystem("D:/Files");

	// If newfile.txt exists, delete it
	if (fs->fileExists("newfile.txt"))
	{
		fs->deleteFile("newfile.txt");
	}

	// Create newfile.txt
	const char* writeBuffer = "Hello World";
	IFile* createdFile = fs->createFile("newfile.txt");
	if (!createdFile)
	{
		std::cout << "Failed to create a file" << std::endl;
		return;
	}

	if (!createdFile->write(writeBuffer, strlen(writeBuffer) + 1 /* add 1 to length for null terminator */))
	{
		std::cout << "Failed to write to file" << std::endl;
	}
	// Close the file
	delete createdFile;
	std::cout << "Successfully created file" << std::endl;

	// Read newfile.txt
	IFile* openedFile = fs->openFile("newfile.txt");
	if (!openedFile)
	{
		std::cout << "Failed to open file" << std::endl;
	}
	size_t size = openedFile->length();
	char* destBuffer = new char[size];
	if (!openedFile->read(destBuffer, size))
	{
		std::cout << "Failed to read from file!" << std::endl;
	}
	else
	{
		std::cout << "File Contents:" << std::endl;
		std::cout << destBuffer << std::endl;
	}
	// Close the file
	delete openedFile;
	std::cout << "======================================================" << std::endl;

}

void showVars(const SampleObject& obj)
{
	const TypeDesc& desc = obj.typeDescription();
	std::cout << desc.name() << " Contents" << std::endl;
	int intVar;
	bool boolVar;
	float floatVar;
	double doubleVar;
	int intArray[12];

	if (obj.getMemberValue("MyIntVariable", intVar))
		std::cout << "  MyIntVariable = " << intVar << std::endl;
	else
		std::cout << "  Cannot read MyIntVariable" << std::endl;

	if (obj.getMemberValue("MyBoolVariable", boolVar))
		std::cout << "  MyBoolVariable = " << (boolVar ? "true" : "false") << std::endl;
	else
		std::cout << "  Cannot read MyBoolVariable" << std::endl;

	if (obj.getMemberValue("MyFloatVariable", floatVar))
		std::cout << "  MyFloatVariable = " << floatVar << std::endl;
	else
		std::cout << "  Cannot read MyFloatVariable" << std::endl;

	if (obj.getMemberValue("MyDoubleVariable", doubleVar))
		std::cout << "  MyDoubleVariable = " << doubleVar << std::endl;
	else
		std::cout << "  Cannot read MyDoubleVariable" << std::endl;

	if (obj.getMemberArray("MyArray", intArray, 12))
	{
		std::cout << "  MyArray = {";
		for (int i = 0; i < 12; i++)
		{
			std::cout << " " << intArray[i] << " ";
		}
		std::cout << "}" << std::endl;
	}
	else
		std::cout << "  Cannot read MyArray" << std::endl;

}

void reflectionExample()
{
	std::cout << "REFLECTION EXAMPLE" << std::endl;
	std::cout << "======================================================" << std::endl;

	SampleObject obj;
	const TypeDesc& typeDesc = SampleObject::staticTypeDescription();
	// List member variables of SampleObject
	std::cout << "Member Variables of " << typeDesc.name() << std::endl;
	for (const MemberDesc& desc : typeDesc.memberDescs())
	{
		std::cout << "  " << desc.name() << std::endl;
	}
	std::cout << std::endl;
	// Set variables directly
	obj.m_MyIntVariable = 0;
	obj.m_MyBoolVariable = true;
	obj.m_MyFloatVariable = 2.0f;
	obj.m_MyDoubleVariable = 3.0;
	for (int i = 0; i < 12; i++)
		obj.m_MyArray[i] = i;

	// Print out variables
	showVars(obj);

	std::cout << std::endl;

	// Set variables using accessors
	obj.setMemberValue("MyIntVariable", 12);
	obj.setMemberValue("MyBoolVariable", false);
	obj.setMemberValue("MyFloatVariable", 23.0f);
	obj.setMemberValue("MyDoubleVariable", 10.0);

	int tempArray[12];
	for (int i = 0; i < 12; i++)
		tempArray[i] = 11 - i;
	obj.setMemberArray("MyArray", tempArray, 12);
	// Print out variables
	showVars(obj);
	std::cout << std::endl;

	// Test Save
	std::cout << "Testing Save..." << std::endl;
	StandardFileSystem fs("D:/test");
	obj.Save(fs, "sampleObj.dat");

	// Test Load
	std::cout << "Testing Load..." << std::endl;
	SampleObject newObj;
	newObj.Load(fs, "sampleObj.dat");
	showVars(newObj);
	std::cout << "======================================================" << std::endl;
}

void archiveExample()
{
	std::cout << "ARCHIVE EXAMPLE" << std::endl;
	std::cout << "======================================================" << std::endl;

	// Create/open a new archive file
	{
		Archive newArchive;
		newArchive.openArchive("D:/Files/newfile.archive");
		{
			// Write "Hello Archive!" to test1.txt
			const char fileContents[] = "Hello Archive!";
			newArchive.writeFile("test1.txt", fileContents, strlen(fileContents) + 1);
		}
		{
			// Write "Test2 Hello!" to test2.txt
			const char fileContents[] = "Test2 Hello!";
			newArchive.writeFile("test2.txt", fileContents, strlen(fileContents) + 1);
		}
		{
			// Write "Test3 Hello!" to test3.txt
			const char fileContents[] = "Test3 Hello!";
			newArchive.writeFile("test3.txt", fileContents, strlen(fileContents) + 1);
		}
		newArchive.close();
	}
	{
		Archive archive;
		archive.openArchive("D:/Files/newfile.archive");
		// Read the written files

		// Read "test1.txt"
		Archive::ArchiveFileBuffer* file1 = archive.readFile("test1.txt");
		if (file1)
			std::cout << "test1.txt contents: " << file1->buffer() << std::endl;
		else
			std::cout << "Cannot read test1.txt!" << std::endl;
		delete file1;

		// Read "test2.txt"
		Archive::ArchiveFileBuffer* file2 = archive.readFile("test2.txt");
		if (file2)
			std::cout << "test2.txt contents: " << file2->buffer() << std::endl;
		else
			std::cout << "Cannot read test2.txt!" << std::endl;
		delete file2;

		// Read "test3.txt"
		Archive::ArchiveFileBuffer* file3 = archive.readFile("test3.txt");
		if (file3)
			std::cout << "test3.txt contents: " << file3->buffer() << std::endl;
		else
			std::cout << "Cannot read test3.txt!" << std::endl;
		delete file3;

		// Test file deletion
		archive.deleteFile("test3.txt");
		if (!archive.fileExists("test3.txt"))
            std::cout << "test3.txt does not exist anymore" << std::endl;
        else
            std::cout << "test3.txt is still there!" << std::endl;

		// Test archive compaction
		archive.compact();
		archive.close();
	}
	std::cout << "======================================================" << std::endl;
}

