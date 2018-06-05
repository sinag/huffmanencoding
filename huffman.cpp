#include <iostream>
#include <string>
#include <unistd.h>
#include <map>
#include <fstream>
#include <queue>
#include <bitset>
#include <vector>
#include <stdlib.h>
#include <sys/stat.h>
#include <sstream>

struct GlobalArgs
{
	unsigned int BlockSize = 1;
	std::string InputFile = "";
	std::string OutputFile = "";
	std::string HuffmanTable = "";
	std::string Operation = "";
	bool Encode = false;
	bool Decode = false;
	bool Verbosity = false;
	int Correction = 0;
} GlobalArgs;

struct Node
{
	int Frequency = 0;
	std::vector<int> Characters;
	Node* Left = nullptr;
	Node* Right = nullptr;

	Node(int _Frequency, std::vector<int> _Character, Node* _Left = nullptr, Node* _Right = nullptr)
	{
		Frequency = _Frequency;
		Characters = _Character;
		Left = _Left;
		Right = _Right;
	}
};

struct Compare
{
    bool operator()(Node* Left, Node* Right)
    {
        return (Left->Frequency > Right->Frequency);
    }
};

static const char *OptString = "edi:o:t:s:vh";

static void ShowUsage()
{
    std::cerr << "Huffman v1.0" << std::endl;
    std::cerr << "BOUN SWE 501 Project" << std::endl;
    std::cerr << "yunus@gulsen.org" << std::endl;
    std::cerr << std::endl;
    std::cerr << "Usage: [OPTION] [Parameter]" << std::endl;
    std::cerr << std::endl;
    std::cerr << "Options:" << std::endl;
    std::cerr << "   -e: Encode file" << std::endl;
    std::cerr << "   -d: Decode file" << std::endl;
    std::cerr << std::endl;
    std::cerr << "Parameters:" << std::endl;
    std::cerr << "   -i [FILE]: Input file" << std::endl;
    std::cerr << "   -o [FILE]: Output file" << std::endl;
    std::cerr << "   -t [FILE]: Huffman table" << std::endl;
    std::cerr << "   -s [NUMBER]: Block size (default:1)" << std::endl;
    std::cerr << "   -v: Print verbose output" << std::endl;
    std::cerr << "   -h: Print help" << std::endl;
    std::cerr << std::endl;
    std::cerr << "Example:" << std::endl;
    std::cerr << "   Encode input.txt with block size 3, store huffman table as huffman.txt and encoded output as output.txt " << std::endl;
    std::cerr << "   >huffman -e -i input.txt -h huffman.txt -o output.txt -s 3" << std::endl;
    std::cerr << std::endl;
    std::cerr << "   Decode input.txt with huffman table huffman.txt and store decoded output as output.txt with verbose output " << std::endl;
    std::cerr << "   >huffman -d -i input.txt -h huffman.txt -o output.txt -v" << std::endl;
}

int ProcessArgs(int* argc, char** argv)
{
    if ((*argc) < 2)
    {
        ShowUsage();
        return 1;
    }
    else
    {
		bool error = false;
		int opt = 0;
		opt = getopt((*argc), (char **)argv, OptString);
		while( opt != -1 )
		{
			switch( opt )
			{
				case 'v':
					GlobalArgs.Verbosity = true;
					if(GlobalArgs.Verbosity) std::cout << "Verbosity: True" << std::endl;
					break;
				case 'e':
					GlobalArgs.Encode = true;
					if(GlobalArgs.Verbosity) std::cout << "Operation: Encode" << std::endl;
					break;
				case 'd':
					GlobalArgs.Decode = true;
					if(GlobalArgs.Verbosity) std::cout << "Operation: Decode" << std::endl;
					break;
				case 'o':
					GlobalArgs.OutputFile = optarg;
					if(GlobalArgs.Verbosity) std::cout << "Output: " << optarg << std::endl;
					break;
				case 'i':
					GlobalArgs.InputFile = optarg;
					if(GlobalArgs.Verbosity) std::cout << "Input: " << optarg << std::endl;
					break;
				case 't':
					GlobalArgs.HuffmanTable = optarg;
					if(GlobalArgs.Verbosity) std::cout << "Huffman Table: " << optarg << std::endl;
					break;
				case 's':
					GlobalArgs.BlockSize = atoi(optarg);
					if(GlobalArgs.Verbosity) std::cout << "Block Size: " << GlobalArgs.BlockSize << std::endl;
					break;
				case 'h':
					ShowUsage();
					break;
				default:
					error = true;
					return 1;
					break;
			}
			opt = getopt((*argc), (char **)argv, OptString);
		}

		if(!error)
		{
			if (GlobalArgs.Encode && GlobalArgs.Decode)
			{
				error = true;
			}
			if (!GlobalArgs.Encode && !GlobalArgs.Decode)
			{
				error = true;
			}
			if (GlobalArgs.Encode || GlobalArgs.Decode)
			{
				if(GlobalArgs.InputFile.empty() || GlobalArgs.OutputFile.empty() || GlobalArgs.HuffmanTable.empty())
				{
					error = true;
				}
				else
				{
					if(GlobalArgs.Encode)
					{
						GlobalArgs.Operation = "Encode";
						if(GlobalArgs.BlockSize<1)
						{
							error = true;
						}
					}
					else
					{
						GlobalArgs.Operation = "Decode";
					}
				}
			}

			if (error)
			{
				std::cerr << argv[0] << " parameter sequence not valid" << std::endl;
				return 1;
			}
		}
    }
    return 0;
}

std::map <std::vector<int>, int> CreateFrequencyTable()
{
	std::map <std::vector<int>, int> Frequencies;
	std::map <std::vector<int>, int>::iterator Iterator;
	try
	{
		std::ifstream InputFile;
		std::ios_base::iostate State = InputFile.exceptions();
		InputFile.exceptions ( std::ifstream::failbit | std::ifstream::badbit );
		InputFile.open(GlobalArgs.InputFile);
		if (InputFile.is_open())
		{
			InputFile.exceptions(State);
			int next = 0 ;
			std::vector<int> Key;
			while ((next = InputFile.get()) != EOF)
			{
				Key.push_back(next);
				if (Key.size()>GlobalArgs.BlockSize-1)
				{
					Iterator = Frequencies.find(Key);
					if (Iterator != Frequencies.end())
					{
						Iterator->second += 1 ;
						Key.clear();
					}
			        else
			        {
			        	Frequencies[Key] = 1 ;
			        	Key.clear();
			        }
				}
			}
			if (Key.size()>0)
			{
				Frequencies[Key] = 1 ;
			}
			InputFile.close();
		}
		else
		{
			std::cerr << "Unable to open file";
		}
	}
	catch(std::exception& e )
	{
		std::cerr << "Exception: Opening file " << GlobalArgs.InputFile << " failed. File either doesn't exist or is not accessible. " << e.what() << std::endl ;
	}
	return Frequencies;
}

void CreateCodeWordMap(std::map <std::vector<int>, std::string>* CodeWordTable, struct Node* Root, std::string CodeWord = "")
{
    if (!Root) return;
    if (!Root->Characters.empty())
    {
    	(*CodeWordTable)[Root->Characters] = CodeWord;
    }
    CreateCodeWordMap(CodeWordTable, Root->Left, CodeWord + "0");
    CreateCodeWordMap(CodeWordTable, Root->Right, CodeWord + "1");
}

void PrintCodeWordMap(std::map<std::vector<int>, std::string>* CodeWordTable)
{
	std::cout << "CodeWords Table" << std::endl;
	for (auto CodeWord : (*CodeWordTable))
	{
		std::cout << "[ ";
		for (auto Symbol : CodeWord.first)
		{
			std::cout << Symbol << " ";
		}
		std::cout << "]" << " " << CodeWord.second << std::endl;
	}
}

void CreateHuffmanTree(std::map<std::vector<int>, int>* Frequencies, std::map<std::vector<int>, std::string>* CodeWordTable)
{
	std::priority_queue<Node*, std::vector<Node*>, Compare > MinHeap;
	for ( auto Symbol : (*Frequencies) )
	{
		MinHeap.push(new Node(Symbol.second, Symbol.first));
	}
	Node* Top;
	Node* Left;
	Node* Right;
    while(MinHeap.size() != 1)
    {
		Left = MinHeap.top();
		MinHeap.pop();
		Right = MinHeap.top();
		MinHeap.pop();
		std::vector<int> Characters;
		Top = new Node(Left->Frequency + Right->Frequency,Characters);
		Top->Left  = Left;
		Top->Right = Right;
		MinHeap.push(Top);
    }
    CreateCodeWordMap(CodeWordTable,MinHeap.top());
    if(GlobalArgs.Verbosity) PrintCodeWordMap(CodeWordTable);
}

std::string EncodeString(std::map<std::vector<int>, std::string>* CodeWordTable)
{
	std::string result="";
	try
	{
		std::ifstream InputFile;
		std::ios_base::iostate State = InputFile.exceptions();
		InputFile.exceptions (std::ifstream::failbit | std::ifstream::badbit);
		InputFile.open(GlobalArgs.InputFile);
		if (InputFile.is_open())
		{
			InputFile.exceptions(State);
			int next = 0 ;
			std::vector<int> Key;
			while ((next = InputFile.get()) != EOF)
			{
				Key.push_back(next);
				if (Key.size()>GlobalArgs.BlockSize-1)
				{
					result.append(CodeWordTable->find(Key)->second);
					Key.clear();
				}
			}
			if (Key.size()>0)
			{
				result.append(CodeWordTable->find(Key)->second);
			}
			InputFile.close();
		}
		else
		{
			std::cerr << "Unable to open input file " << GlobalArgs.InputFile;
		}
	}
	catch(std::exception& e )
	{
		std::cerr << "Exception: Opening file " << GlobalArgs.InputFile << " failed. File either doesn't exist or is not accessible. " << e.what() << std::endl;
	}
	if(GlobalArgs.Verbosity) std::cout << "Encoded String" << std::endl << result << std::endl;
	return result;
}

std::vector<std::string> SplitBitString(std::string* BitString, int Size)
{
	int NumSubstrings = (*BitString).length() / Size;
	std::vector<std::string> Result;
	for (auto i = 0; i < NumSubstrings; i++)
	{
		Result.push_back((*BitString).substr(i * Size, Size));
	}
	if ((*BitString).length() % Size != 0)
	{
		std::string leftover = (*BitString).substr(Size * NumSubstrings);
		Result.push_back(leftover);
		GlobalArgs.Correction = leftover.size();
		if(GlobalArgs.Verbosity) std::cout << "Leftover: " << leftover << " Correction: " << GlobalArgs.Correction << std::endl;
	}
	if(GlobalArgs.Verbosity)
	{
		std::cout << "BitSplits" << std::endl;
		for (auto BitSplit : Result)
		{
			std::cout << BitSplit << std::endl;
		}
	}
	return Result;
}

std::vector<std::bitset<8>> CreateBitSetArray(std::vector<std::string>* BitStrings)
{
	if(GlobalArgs.Verbosity) std::cout << "BitSets" << std::endl;
	std::vector<std::bitset<8>> Result;
	for(auto BitString : (*BitStrings))
	{
	    std::bitset<8> BitSet(BitString);
	    unsigned char c = ( BitSet.to_ulong() & 0xFF);
	    if(GlobalArgs.Verbosity) std::cout << static_cast<int>(c) << std::endl;
	    Result.push_back(BitSet);
	}
	return Result;
}

bool WriteHuffmanTable(std::map <std::vector<int>, std::string>* CodeWordTable)
{
	try
	{
		std::ofstream OutputFile;
		std::ios_base::iostate State = OutputFile.exceptions();
		OutputFile.exceptions (std::ofstream::failbit | std::ofstream::badbit);
		OutputFile.open(GlobalArgs.HuffmanTable, std::ios::out | std::ios::binary);
		if (OutputFile.is_open())
		{
			OutputFile.exceptions(State);
			OutputFile << GlobalArgs.Correction << std::endl;
			OutputFile << GlobalArgs.BlockSize << std::endl;
			for(auto CodeWord : (*CodeWordTable))
			{
				OutputFile << CodeWord.second;
				for (auto Symbol : CodeWord.first)
				{
					OutputFile << "_" << Symbol;
				}
				OutputFile << std::endl;
			}
			OutputFile.close();
			if(GlobalArgs.Verbosity) std::cout << "Huffman table written successfully" << std::endl;
		}
		else
		{
			std::cerr << "Unable to create huffman table file " << GlobalArgs.OutputFile;
			return false;
		}
	}
	catch(std::exception& e )
	{
		std::cerr << "Exception: Opening file " << GlobalArgs.HuffmanTable << " failed. File either doesn't exist or is not accessible. " << e.what() << std::endl;
		return false;
	}
	return true;
}

std::map <std::vector<int>, std::string> ReadHuffmanTable()
{
	std::map <std::vector<int>, std::string> CodeWordTable;
	try
	{
		std::ifstream InputFile;
		std::ios_base::iostate State = InputFile.exceptions();
		InputFile.exceptions (std::ifstream::failbit | std::ifstream::badbit);
		InputFile.open(GlobalArgs.HuffmanTable);
		if (InputFile.is_open())
		{
			InputFile.exceptions(State);
			std::vector<int> Key;
			std::string CodeWord;
			InputFile >> GlobalArgs.Correction;
			InputFile >> GlobalArgs.BlockSize;
			std::string Line;
			while (InputFile >> Line)
			{
				std::stringstream Temp;
				Temp << Line;
				std::string Segment;
				bool CodeWordRead = false;
				while(std::getline(Temp, Segment, '_'))
				{
					if(!CodeWordRead)
					{
						CodeWord = Segment;
						CodeWordRead = true;
					}
					else
					{
						Key.push_back(std::stoi(Segment));
					}
				}
				CodeWordTable[Key] = CodeWord;
				Key.clear();
			}
			InputFile.close();
		}
		else
		{
			std::cerr << "Unable to open huffman table file " << GlobalArgs.InputFile;
		}
	}
	catch(std::exception& e )
	{
		std::cerr << "Exception: Opening file " << GlobalArgs.HuffmanTable << " failed. File either doesn't exist or is not accessible. " << e.what() << std::endl;
	}
	if(GlobalArgs.Verbosity)
	{
		std::cout << "Huffman table read successfully" << std::endl;
		std::cout << "Correction: " << GlobalArgs.Correction << std::endl;
		std::cout << "BlockSize: " << GlobalArgs.BlockSize << std::endl;
		PrintCodeWordMap(&CodeWordTable);
	}
	return CodeWordTable;
}

std::map <std::string, std::vector<int>> ReverseCodeMap(std::map <std::vector<int>, std::string>* CodeWordTable)
{
	std::map <std::string, std::vector<int>> Result;
	for (auto CodeWord : (*CodeWordTable))
	{
		Result[CodeWord.second] = CodeWord.first;
	}
	return Result;
}

std::string DecodeString(std::string* EncodedString, std::map <std::vector<int>, std::string>* CodeWordTable)
{
	std::map <std::string, std::vector<int>> ReversedCodeMap = ReverseCodeMap(CodeWordTable);
	std::string Result = "";
	std::string String = "";
	std::map<std::string, std::vector<int>>::iterator Searcher;
	for (auto Char : (*EncodedString))
	{
		String += Char;
		Searcher = ReversedCodeMap.find(String);
		if (Searcher != ReversedCodeMap.end())
		{
			std::vector<int> FoundChars = Searcher->second;
			for(auto CharCode : FoundChars)
			{
				char Char = char(CharCode);
				Result+=Char;
			}
			String = "";
		}
	}
	return Result;
}

bool WriteDecodedFile(std::string* DecodedString)
{
	try
	{
		std::ofstream OutputFile;
		std::ios_base::iostate State = OutputFile.exceptions();
		OutputFile.exceptions (std::ofstream::failbit | std::ofstream::badbit);
		OutputFile.open(GlobalArgs.OutputFile, std::ios::out | std::ios::binary);
		if (OutputFile.is_open())
		{
			OutputFile.exceptions(State);
			OutputFile << (*DecodedString);
			OutputFile.close();
			if(GlobalArgs.Verbosity) std::cout << "Output file written successfully" << std::endl;
		}
		else
		{
			std::cerr << "Unable to create output file " << GlobalArgs.OutputFile;
			return false;
		}
	}
	catch(std::exception& e )
	{
		std::cerr << "Exception: Opening file " << GlobalArgs.OutputFile << " failed. File either doesn't exist or is not accessible. " << e.what() << std::endl;
		return false;
	}
	return true;
}

long GetFileSize(std::string* filename)
{
    struct stat Buffer;
    int rc = stat((*filename).c_str(), &Buffer);
    return rc == 0 ? Buffer.st_size : -1;
}

void PrintStatistics()
{
		long InputSize = GetFileSize(&GlobalArgs.InputFile);
		long OutputSize = GetFileSize(&GlobalArgs.OutputFile);
		std::cout << "Statistics" << std::endl << "Input File Size: " << InputSize << " bytes" << std::endl;
		std::cout << "Output File Size: " << OutputSize << " bytes" << std::endl;
		if (GlobalArgs.Operation == "Encode")
			std::cout << "Compression Ratio: %" << (1-(double(OutputSize)/InputSize))*100 << std::endl;
		else
			std::cout << "Compression Ratio: %" << (1-(double(InputSize)/OutputSize))*100 << std::endl;
}

void PrintFrequencyTable(std::map <std::vector<int>, int>* Frequencies)
{
	std::cout << "Frequency Table" << std::endl;
	for (auto Frequency : (*Frequencies))
	{
		std::cout << "[ ";
		for (auto Symbol : Frequency.first)
		{
			std::cout << Symbol << " ";
		}
		std::cout << "]" << " " << Frequency.second << std::endl;
	}
}

bool Encode()
{
	std::map <std::vector<int>, int> Frequencies = CreateFrequencyTable();
	if (Frequencies.size()<1)
	{
		std::cerr << "Frequency table is empty" << std::endl;
		return 1;
	}
	if(GlobalArgs.Verbosity)
	{
		PrintFrequencyTable(&Frequencies);
	}

	std::map <std::vector<int>, std::string> CodeWordTable;
	CreateHuffmanTree(&Frequencies, &CodeWordTable);
	if (CodeWordTable.size()<1)
	{
		std::cerr << "CodeWord table is empty" << std::endl;
		return 1;
	}
	std::string EncodedString = EncodeString(&CodeWordTable);
	if (EncodedString.size()<1)
	{
		std::cerr << "Encoded string is empty" << std::endl;
		return 1;
	}
	std::vector<std::string> BitStrings = SplitBitString(&EncodedString,8);
	if (BitStrings.size()<1)
	{
		std::cerr << "BitStrings table is empty" << std::endl;
		return 1;
	}
	std::vector<std::bitset<8>> BitSets = CreateBitSetArray(&BitStrings);
	if (BitSets.size()<1)
	{
		std::cerr << "BitSet table is empty" << std::endl;
		return 1;
	}
	if(GlobalArgs.Verbosity)
	{
		std::cout << "Converted BitSets" << std::endl;
		for (auto BitSet : BitSets)
		{
			std::cout << BitSet << std::endl;
		}
	}
	try
	{
		std::ofstream OutputFile;
		std::ios_base::iostate State = OutputFile.exceptions();
		OutputFile.exceptions (std::ofstream::failbit | std::ofstream::badbit);
		OutputFile.open(GlobalArgs.OutputFile, std::ios::out | std::ios::binary);
		if (OutputFile.is_open())
		{
			OutputFile.exceptions(State);
			for (auto BitSet : BitSets)
			{
				char Byte = (char)BitSet.to_ulong();
				OutputFile << Byte;
			}
			OutputFile.close();
		}
		else
		{
			std::cerr << "Unable to open output file " << GlobalArgs.OutputFile;
			return false;
		}
	}
	catch(std::exception& e )
	{
		std::cerr << "Exception: Opening file " << GlobalArgs.OutputFile << " failed. File either doesn't exist or is not accessible. " << e.what() << std::endl;
		return false;
	}
	if (WriteHuffmanTable(&CodeWordTable)) return true;
	else return false;
	return true;
}

bool Decode()
{
	std::map <std::vector<int>, std::string> CodeWordTable = ReadHuffmanTable();
	if (CodeWordTable.size()<1)
	{
		return false;
	}
	try
	{
		std::string EncodedString="";
		std::ifstream InputFile;
		std::ios_base::iostate State = InputFile.exceptions();
		InputFile.exceptions ( std::ifstream::failbit | std::ifstream::badbit );
		InputFile.open(GlobalArgs.InputFile);
		if (InputFile.is_open())
		{
			InputFile.exceptions(State);
			int next = 0 ;
			while ((next = InputFile.get()) != EOF)
			{
				EncodedString.append(std::bitset<8>(next).to_string());
			}
			InputFile.close();
			if(GlobalArgs.Correction>0)
			{
				EncodedString.erase(EncodedString.size()-8,8-GlobalArgs.Correction);
			}
		}
		else
		{
			std::cerr << "Unable to open file";
			return false;
		}
		if(EncodedString.size()<1)
		{
			std::cerr << "Encoded string is empty" << std::endl;
			return false;
		}
		if(GlobalArgs.Verbosity) std::cout << "Encoded String" << std::endl << EncodedString << std::endl;
		std::string DecodedString = DecodeString(&EncodedString,&CodeWordTable);
		if(GlobalArgs.Verbosity) std::cout << "Decoded String" << std::endl << DecodedString << std::endl;
		if(DecodedString.size()<1)
		{
			std::cerr << "Decoded string is empty" << std::endl;
			return false;
		}
		if (!WriteDecodedFile(&DecodedString))
		{
			return false;
		}
	}
	catch(std::exception& e )
	{
		std::cerr << "Exception: Opening file " << GlobalArgs.InputFile << " failed. File either doesn't exist or is not accessible. " << e.what() << std::endl ;
		return false;
	}
	return true;
}

int main(int argc, char* argv[])
{
   	int ArgResult = ProcessArgs(&argc, argv);
   	if (ArgResult==0)
   	{
   		if (GlobalArgs.Encode == true)
   		{
   			if(Encode())
   			{
   				PrintStatistics();
   				return 0;
   			}
   			else
   			{
   				return 1;
   			}
   		}
   		else
   		{
   			if(Decode())
   			{
   				PrintStatistics();
   				return 0;
   			}
   			else
   			{
   				return 1;
   			}
   		}
   	}
   	else
   	{
   		return 1;
   	}
}
