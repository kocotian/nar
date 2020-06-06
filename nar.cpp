#include <iostream>
#include <sstream>
#include <fstream>
#include <cstring>

std::string version = "1.0";

int main(int argc, char** argv)
{
	if(argc > 1)
	{
		if(!strcmp(argv[1], "-a"))
		{
			std::string buffer = "";
			std::ifstream i;

			buffer += ":::[[[\n";
			buffer += "format:\nnar\n";
			buffer += "]]]:::\n";

			for(int f = 2; f < argc; f++)
			{
				buffer += ":::[FILE\n/";
				buffer += argv[f];
				buffer += "\n";

				buffer += ":::BOF{\n";
					i.open(argv[f], std::ios::in | std::ios::binary);
					if(i.good() == true)
					{
						std::string buff;
						while(getline(i, buff))
						{
							if(buff != "}EOF:::")
							{
								buffer += buff;
								buffer += "\n";
							}
							else buffer += "\\}EOF:::\n";
						}
						i.close();
					}
					else
					{
						std::cerr << argv[0] << ": Failed to open '" << argv[f] << "' file.\n\r";
						return -1;
					}
				buffer += "}EOF:::";
				
				buffer += "\n]:::\n";
			}

			buffer += ":::[[[EOA]]]:::";
			for(int bpos = 0; bpos < buffer.length(); bpos++)
			{
				std::cout << (char)(255 - buffer[bpos]);
			}
		}
		else if(!strcmp(argv[1], "-u"))
		{
			std::ifstream i;
			i.open(argv[2], std::ios::in | std::ios::binary);
			if(i.good())
			{
				char cbuf;
				std::string buffer;
				while(i.get(cbuf))
				{
					int revn = 255 - cbuf;
					char rbuf = revn;
					buffer += rbuf;
				}
				std::istringstream isdbuf(buffer);
				std::string data = "";
				bool inHeader, inFileHeader, inFileContent = false;
				std::string format = "nar";

				while(getline(isdbuf, data))
				{
					//getline(i, data);
					if(data == ":::[[[")
					{
						std::cout << "Opening header...\n";
						inHeader = true;
						std::string d = "";
						do
						{
							getline(isdbuf, d);
							if(d == "format:")
							{
								getline(isdbuf, format);
								std::cout << ">> Format set to " << format << "!\n";
							}
						}
						while(d != "]]]:::");// || i.eof());
						if(d == "]]]:::")
						{
							std::cout << "Closing header...\n";
							inHeader = false;
						}
					}
					else if(data == ":::[FILE")
					{
						std::cout << "Opening file header...\n";
						inFileHeader = true;
						std::string path = "";
						getline(isdbuf, path);
						getline(isdbuf, data);
						if(data == ":::BOF{")
						{
							std::ofstream o;
							o.open("." + path, std::ios::out | std::ios::binary);
							if(o.good())
							{
								std::cout << ">> Saving data to file " << path << ".\n";
								std::string fline = "";
								while(fline != "}EOF:::")
								{
									getline(isdbuf, fline);
									if(fline != "\\}EOF:::")
									{
										if(fline != "}EOF:::") o << fline << "\n";
									}
									else
									{
										o << "}EOF:::" << "\n";
									}
								}
								o.close();
							}
							else
							{
								std::cerr << argv[0] << ": Failed to create '" << "." + path << "' file.\n\r";
								return -1;
								i.close();
							}
						}
					}
				}
				i.close();
			}
			else
			{
				std::cerr << argv[0] << ": Failed to open '" << argv[2] << "' file.\n\r";
				return -1;
			}
		}
		else if(!strcmp(argv[1], "-h") || !strcmp(argv[1], "--help"))
		{
			std::cout << "Niggarchiver v" << version << '\n';
			std::cout << "Usage:\n    -a  Archive data\n    -u  Unarchive data\n    -h  Show this help\n\nExamples:\n    ";
			std::cout << argv[0] << " -a * > foo.nar - Archives all data in current directory to archive foo.nar\n    ";
			std::cout << argv[0] << " -a document.pdf > document.nar - Archive document.pdf file to archive document.nar\n    ";
			std::cout << argv[0] << " -u photos.nar - Unarchive all content of photos.nar archive\n\r";
		}
		else
		{
			std::cerr << argv[0] << ": bad option -- '" << argv[1] << "'\nWrite \"" << argv[0] << " -h\" for help\n\r";
		}
	}
	else
	{
		std::cerr << argv[0] << ": expected argument\nWrite \"" << argv[0] << " -h\" for help\n\r";
	}
}
