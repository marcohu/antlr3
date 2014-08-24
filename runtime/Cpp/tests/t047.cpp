#include "UserTestTraits.hpp"
#include "t047treeparserLexer.hpp"
#include "t047treeparserParser.hpp"
#include "t047treeparserWalker.hpp"

#include <sys/types.h>

#include <iostream>
#include <sstream>
#include <fstream>

using namespace Antlr3Test;
using namespace std;

int testValid(string const& data);
int testWalker(string const data);
	
static const char* program =
"            char c;				  \n"
"            int x;				  \n"
"						  \n"
"            void bar(int x);			  \n"
"						  \n"
"            int foo(int y, char d) {		  \n"
"              int i;				  \n"
"              for (i=0; i<3; i=i+1) {		  \n"
"                x=3;				  \n"
"                y=5;				  \n"
"              }				  \n"
"            }					  \n"
;

static const char* expectedResult =
	"(VAR_DEF char c) (VAR_DEF int x) (FUNC_DECL (FUNC_HDR void bar (ARG_DEF int x))) (FUNC_DEF (FUNC_HDR int foo (ARG_DEF int y) (ARG_DEF char d)) (BLOCK (VAR_DEF int i) (for (= i 0) (< i 3) (= i (+ i 1)) (BLOCK (= x 3) (= y 5)))))";

int main (int argc, char *argv[])
{
	testWalker(program);
	return 0;
}

int testWalker(string const data)
{
	auto input = new t047treeparserLexerTraits::InputStreamType((const ANTLR_UINT8 *)data.c_str(),
		antlr3::ENC_8BIT,
		data.length(),
		(ANTLR_UINT8*)"t047");
	auto lxr = new t047treeparserLexer(input);
	auto tstream = new t047treeparserLexerTraits::TokenStreamType(ANTLR_SIZE_HINT, lxr->get_tokSource());
	auto psr = new t047treeparserParser(tstream);
	{
		auto r = psr->program();
		std::cout << r.tree->toStringTree() << std::endl;

	std::cout
		<< data << std::endl
		<< "Result:" << std::endl << r.tree->toStringTree() << std::endl
		<< "Expected:" << std::endl << expectedResult << std::endl
		<< (r.tree->toStringTree() == expectedResult ? "OK" : "Fail") << std::endl;
	
	auto nodes = new t047treeparserWalker::TreeNodeStreamType(r.tree);

        // nodes.setTokenStream(tStream)
	auto walker = new t047treeparserWalker(nodes);
	walker->program();

		// nodes.setTokenStream(tStream)
		auto walker = new t047treeparserWalker(nodes);
		//walker->program();
		delete walker;
	}
	delete lxr;
	delete input;
	return 0;
	
}
