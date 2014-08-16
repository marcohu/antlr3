#include "UserTestTraits.hpp"
#include "t047treeparserLexer.hpp"
#include "t047treeparserParser.hpp"
//#include "t047treeparserWalker.hpp"

#include <sys/types.h>

#include <iostream>
#include <sstream>
#include <fstream>

using namespace Antlr3Test;
using namespace std;

int testValid(string const& data);

static t047treeparserLexer* lxr;

int main (int argc, char *argv[])
{
	testValid("0");
	return 0;
}

int testValid(string const& data)
{
	auto input = new t047treeparserLexerTraits::InputStreamType((const ANTLR_UINT8 *)data.c_str(),
								    antlr3::ENC_8BIT,
								    data.length(), //strlen(data.c_str()),
								    (ANTLR_UINT8*)"t047");
	if (lxr == NULL)
		lxr = new t047treeparserLexer(input);
	else
		lxr->setCharStream(input);

	std::cout << "testValid: \"" << data << '"' <<std::endl;

	t047treeparserLexer::CommonTokenType *token0 = lxr->nextToken();
	t047treeparserLexer::CommonTokenType *token1 = lxr->nextToken();

	std::cout << token0->getText() << std::endl;
	std::cout << token1->getText() << std::endl;
	
	delete lxr; lxr = NULL;
	delete input;
	return 0;
}
