#pragma once
#include <string>

namespace core {
	int fcreate(std::string filename);                       //create file
	int fopen(std::string filename);                         //open file
	int fdelete(int file);                                    //delete file

	int fread();                                             //read file
	int fwrite();                                            //write file
	int fappend();                                           //append file

	int fseek();                                             //set position on file
	
	int fget_attributes();
	int fset_attributes();
	int frename();



}