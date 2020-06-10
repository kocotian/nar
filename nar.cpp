#include <iostream>
#include <sstream>
#include <fstream>
#include <cstring>
#include <filesystem>

std::string version = "2.0";

int main(int argc, char** argv)
{
	if(argc > 1)
	{
		/* ARCHIVE :: -a :: nar -a input [input2...] > output.nar */
		
		if(!strcmp(argv[1], "-a"))
		{
			std::string buffer = "";
			std::ifstream i;

			buffer += ":::[[[\nformat:\nnar2\n]]]:::\n";

			for(int f = 2; f < argc; f++)
			{
				buffer += "F:\n/";
				buffer += argv[f];
				buffer += "\n";

				i.open(argv[f], std::ios::in | std::ios::binary);
				if(i.good() == true)
				{
					std::string buff;
					while(getline(i, buff))
					{
						if(buff != std::string(1, char(28)))
						{
							buffer += buff;
							buffer += "\n";
						}
						else
						{
							buffer += "\\";
							buffer += char(28);
							buffer += "\n";
						}
					}
					i.close();
				}
				else
				{
					std::cerr << argv[0] << ": Failed to open '" << argv[f] << "' file.\n\r";
					return -1;
				}
				buffer += char(28);
				buffer += "\n";
			}

			buffer += char(4);
			for(int bpos = 0; bpos < buffer.length(); bpos++)
			{
				std::cout << (char)(255 - buffer[bpos]);
			}
		}

		/* UNARCHIVE :: -u :: nar -u input.nar */

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
				std::string format = "bad";

				while(getline(isdbuf, data))
				{
					if(data == ":::[[[")
					{
						std::cout << ":: Opening header...\n";
						inHeader = true;
						std::string d = "";
						do
						{
							getline(isdbuf, d);
							if(d == "format:")
							{
								getline(isdbuf, format);
								if(format == "nar2")
								std::cout << ":: Detected correct format (" << format << ")!\n";
								else if(format == "nar")
								{
									std::cerr << argv[0] << ": Detected nar1 format. You must convert it to newer nar2 format. Write \"" << argv[0] << " -c " << argv[2] << " > newfilename.nar\" and try again.\n";
									return -1;
								}
								else
								{
									std::cerr << argv[0] << ": Detected wrong format (" << format << "). This can be later nar version - download latest niggarchiver and try again.\n";
									return -1;
								}
							}
						}
						while(d != "]]]:::");// || i.eof());
						if(d == "]]]:::")
						{
							std::cout << ":: Closing header...\n";
							inHeader = false;
						}
					}
					else if(data == "F:")
					{
						std::cout << ":: Opening file header...\n";
						inFileHeader = true;
						std::string path = "";
						getline(isdbuf, path);

						std::string dir, file = ""; int lastslash = 0;
						for(int i = 0; i < path.length(); i++)
						{
							if(path[i] == '/') lastslash = i;
						}
						for(int i = 0; i < lastslash; i++)
						dir += path[i];
						if(!std::filesystem::exists("." + path))
						{
							std::filesystem::create_directories("./" + dir);
							std::ofstream o;
							o.open("." + path, std::ios::out | std::ios::binary);
							if(o.good())
							{
								std::cout << ":: Saving data to file " << path << ".\n";
								std::string fline = "";
								while(fline != std::string(1, char(28)))
								{
									getline(isdbuf, fline);
									if(fline != "\\" + char(28))
									{
										if(fline != std::string(1, char(28))) o << fline << "\n";
									}
									else
									{
										o << char(28) << "\n";
									}
								}
								o.close();
							}
							else
							{
								std::cerr << argv[0] << ": Failed to create '" << "." + path << "' file.\n\r";
								i.close();
							}
						}
						else
						{
							std::cerr << argv[0] << ": Unable to create \"" << path << "\". File exists.\n\r";
						}
					}
				}
				i.close();
				if(format == "bad") std::cerr << argv[0] << ": File is not nar archive or file is broken.\n\r";
			}
			else
			{
				std::cerr << argv[0] << ": Failed to open '" << argv[2] << "' file.\n\r";
				return -1;
			}
		}

		/* CONVERT :: -c :: nar -c input.nar > output.nar */

		else if(!strcmp(argv[1], "-c"))
		{
			std::ifstream i;
			i.open(argv[2], std::ios::in | std::ios::binary);
			if(i.good())
			{
				char cbuf;
				std::string fbuf;
				std::string buffer;
				while(i.get(cbuf))
				{
					int revn = 255 - cbuf;
					char rbuf = revn;
					fbuf += rbuf;
				}
				std::istringstream isdbuf(fbuf);
				std::string data = "";
				bool inHeader, inFileHeader, inFileContent = false;
				std::string format = "bad";

				while(getline(isdbuf, data))
				{
					/* header // nar1 -> latest */
					if(data == ":::[[[")
					{
						inHeader = true;
						std::string d = "";
						do
						{
							getline(isdbuf, d);
							if(d == "format:")
							{
								getline(isdbuf, format);
								if(format == "nar")
								{
									buffer += ":::[[[\nformat:\nnar2\n]]]:::\n";
								}
								else if(format == "nar2")
								{
									std::cerr << argv[0] << ": Format nar2 detected. Conversion stopped because nar2 is latest format.\n";
									return 0;
								}
								else
								{
									std::cerr << argv[0] << ": Detected unknown format (" << format << "). This format can be later nar version - download latest niggarchiver and try again.\n";
									return -1;
								}
							}
						}
						while(d != "]]]:::");
						if(d == "]]]:::")
						{
							inHeader = false;
						}
					}
					else if(data == ":::[FILE" && format == "nar")
					{
						buffer += "F:\n";
						inFileHeader = true;
						std::string path = "";
						getline(isdbuf, path);
						getline(isdbuf, data);
						buffer += path + "\n";
						if(data == ":::BOF{")
						{
							std::string fline = "";
							while(fline != "}EOF:::")
							{
								getline(isdbuf, fline);
								if(fline != "\\}EOF:::")
								{
									if(fline != "}EOF:::")
									{
										if(fline != std::string(1, char(28)))
										buffer += fline + "\n";
										else
										buffer += "\\" + fline + "\n";
									}
								}
								else
								{
									buffer += "}EOF:::\n";
								}
							}
							buffer += char(28);
							buffer += "\n";
						}
					}
					else if(data == ":::[[[EOA]]]:::" && format == "nar")
					buffer += char(4);
				}
				i.close();
				for(int bpos = 0; bpos < buffer.length(); bpos++)
				{
					std::cout << (char)(255 - buffer[bpos]);
				}
				if(format == "bad") std::cerr << argv[0] << ": File is not nar archive or file is broken.\n\r";
			}
			else
			{
				std::cerr << argv[0] << ": Failed to open '" << argv[2] << "' file.\n\r";
				return -1;
			}
		}

		/* LIST :: -l :: nar -l input.nar */

		else if(!strcmp(argv[1], "-l"))
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
				std::string format = "bad";

				while(getline(isdbuf, data))
				{
					if(data == ":::[[[")
					{
						inHeader = true;
						std::string d = "";
						do
						{
							getline(isdbuf, d);
							if(d == "format:")
							{
								getline(isdbuf, format);
								if(format == "nar2")
								std::cout << ":: File in format " << format << ".\n";
								else if(format == "nar")
								{
									std::cerr << argv[0] << ": Detected nar1 format. You must convert it to newer nar2 format. Write \"" << argv[0] << " -c " << argv[2] << " > newfilename.nar\" and try again.\n";
									return -1;
								}
								else
								{
									std::cerr << argv[0] << ": Detected wrong format (" << format << "). This can be later nar version - download latest niggarchiver and try again.\n";
									return -1;
								}
							}
						}
						while(d != "]]]:::");// || i.eof());
						if(d == "]]]:::")
						{
							inHeader = false;
						}
					}
					else if(data == "F:")
					{
						inFileHeader = true;
						std::string path = "";
						getline(isdbuf, path);

						std::cout << ":: File: " << path << ".\n";
						std::string fline = "";
						while(fline != std::string(1, char(28)))
						{
							getline(isdbuf, fline);
						}
					}
				}
				i.close();
				if(format == "bad") std::cerr << argv[0] << ": File is not nar archive or file is broken.\n\r";
				else std::cout << "\r";
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
			std::cout << "Usage:\n    -a  Archive data\n    -u  Unarchive data\n    -l  Displays archive content\n    -c  Converts archive to latest format\n    -h  Show this help\n\nExamples:\n";
			std::cout << argv[0] << " -a * > foo.nar - Archives all data in current directory to archive foo.nar\n";
			std::cout << argv[0] << " -a document.pdf > document.nar - Archive document.pdf file to archive document.nar\n";
			std::cout << argv[0] << " -u photos.nar - Unarchive all content of photos.nar archive\n";
			std::cout << argv[0] << " -l example.nar - Display content of example.nar archive\n";
			std::cout << argv[0] << " -c bar.nar > foo.nar - Converts bar.nar archive with older nar format to latest available\n\r";
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
