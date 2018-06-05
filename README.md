About
=
This is a sample huffman encoder/decoder I have written for SWE 501 class @ Bogazici University. 

This is the first and only thing I have written in C++, so any advice and suggestions to make it more efficient, reliable, standard or just beautifull will be greatly appreciated.

Compile (C++ 11)
=
>g++ -O0 -g3 -Wall -c -fmessage-length=0 -o huffman.o huffman.cpp

>g++ -o huffman huffman.o

Usage
=
Usage: [OPTION] [Parameter]


### Options:

   -e: Encode file
   
   -d: Decode file
   

### Parameters:

   -i [FILE]: Input file
   
   -o [FILE]: Output file
   
   -t [FILE]: Huffman table
   
   -s [NUMBER]: Block size (default:1)
   
   -v: Print verbose output
   
   -h: Print help
   

Sample
=
   Encode input.txt with block size 3, store huffman table as huffman.txt and encoded output as output.txt
   
   >huffman -e -i input.txt -h huffman.txt -o output.txt -s 3
   

   Decode input.txt with huffman table huffman.txt and store decoded output as output.txt with verbose output
   
   >huffman -d -i input.txt -h huffman.txt -o output.txt -v
