#include <iostream>
#include <string>
#include <fstream>

// Custom string encoding
std::string encode(std::string text)
{
	if (text.length() >= 4)
	{
		std::string reverse = "";
		std::string div = "";
		std::string order = "";

		// Reverse the string - "abcd" will be "dcba" 

		for (int i = text.length() - 1; i >= 0; i--)
			reverse += text[i];

		std::cout << "\ne_Reverse:\t" << reverse << "\n";


		// Divide the string by 2 and switch there location - "Hello World" will be "World Hello"

		int s = (text.length() - 1) / 2;

		for (size_t i = s; i < text.length(); i++)
			div += reverse[i];

		for (int i = 0; i < s; i++)
			div += reverse[i];

		std::cout << "\ne_Divide:\t" << div << "\n";


		// Reorder characters. "abcd" will be "badc" - Switch the current char with the next char

		for (size_t i = 0; i < text.length(); i += 2)
		{
			if (i != text.length() - 1)
			{
				order += div[i + 1];
				order += div[i];
			}
			else
			{
				order += div[i];
			}
		}

		std::cout << "\ne_order:\t" << order << "\n";

		return order;
	}
	return "";
}

// Custom string decoding - This is using to obfuscate strings from static analysis
std::string decode(std::string encoded_text)
{
	if (encoded_text.length() >= 4)
	{
		std::string reverse = "";
		std::string div = "";
		std::string order = "";

		// Reorder characters. "badc" will be "abcd" - Switch the current char with the next char
		for (size_t i = 0; i < encoded_text.length(); i += 2)
		{
			if (i != encoded_text.length() - 1)
			{
				order += encoded_text[i + 1];
				order += encoded_text[i];
			}
			else
			{
				order += encoded_text[i];
			}
		}

		std::cout << "\nd_Reorder:\t" << order << "\n";

		// Divide the string by 2 and switch there location - "World Hello" will be "Hello World"
		int s = (encoded_text.length()) / 2;

		for (size_t i = s + 1; i < encoded_text.length(); i++) // Put the second half first in the new string
			div += order[i];

		for (int i = 0; i <= s; i++) // put the first half second in the new string
			div += order[i];

		std::cout << "\nd_Divided:\t" << div << "\n";

		// Reverse the string - "dcba" will be "abcd"
		for (int i = encoded_text.length() - 1; i >= 0; i--)
			reverse += div[i];

		std::cout << "\nd_Reverse:\t" << reverse << "\n";

		return reverse;
	}

	return "";
}

int main()
{
	// Original String
	std::string text = "STRING TO ENCODE";
	std::cout << "\nOriginal String:\t" << text << "\n\n";

	// Encoding
	std::string e = encode(text);
	std::cout << "\nEncoded String:\t;" << e << ";\n\n";

	std::cout << "\n\n\n-----------------------------------------------\n\n\n";
	std::string to_decode = e;
	std::cout << "To Decode: ;" << to_decode << ";\n\n";

	// Decoding
	std::string result = decode(to_decode);
	std::cout << "\nDecoded String:\t;" << result << ";\n\n";

	// Results
	if (result == text)
		std::cout << "\n\nSUCCESS!\n\n";
	else
		std::cout << "\n\nFAILED!\n\n";

	return 0;
}