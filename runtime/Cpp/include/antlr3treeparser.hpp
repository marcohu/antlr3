#ifndef	ANTLR3TREEPARSER_HPP
#define	ANTLR3TREEPARSER_HPP

// [The "BSD licence"]
// Copyright (c) 2005-2009 Gokulakannan Somasundaram, ElectronDB

//
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions
// are met:
// 1. Redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer.
// 2. Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the
//    documentation and/or other materials provided with the distribution.
// 3. The name of the author may not be used to endorse or promote products
//    derived from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
// IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
// OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
// IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
// INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
// NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
// THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

/** Internal structure representing an element in a hash bucket.
 *  Stores the original key so that duplicate keys can be rejected
 *  if necessary, and contains function can be supported If the hash key
 *  could be unique I would have invented the perfect compression algorithm ;-)
 */
namespace antlr3 {

template<class ImplTraits>
class  TreeParser : public ImplTraits::template RecognizerType< typename ImplTraits::TreeNodeStreamType >
{
public:
	typedef typename ImplTraits::template RecognizerType< typename ImplTraits::TreeNodeStreamType > BaseType;
	typedef typename ImplTraits::template RecognizerType< typename ImplTraits::TreeNodeStreamType > RecognizerType;

	typedef typename ImplTraits::TreeNodeStreamType TreeNodeStreamType;
 	typedef typename ImplTraits::TreeNodeStreamType TokenStreamType;
	typedef TreeNodeStreamType StreamType;
	typedef TreeNodeStreamType IntStreamType;
	typedef typename ImplTraits::TreeType TreeType;
	typedef typename ImplTraits::TreeTypePtr TreeTypePtr;
	typedef TreeType TokenType;
	typedef typename ImplTraits::template ExceptionBaseType<TreeNodeStreamType> ExceptionBaseType;
	typedef typename RecognizerType::RecognizerSharedStateType RecognizerSharedStateType;
	typedef Empty TokenSourceType;
	typedef typename ImplTraits::BitsetListType BitsetListType;
	typedef typename ImplTraits::StringType StringType;
	typedef typename ImplTraits::CommonTokenType CommonTokenType;
	
private:
	/** Pointer to the common tree node stream for the parser
	 */
	TreeNodeStreamType* m_ctnstream;

public:

	TreeParser( ANTLR_UINT32 sizeHint, TreeNodeStreamType* ctnstream,
											RecognizerSharedStateType* state);
	TreeNodeStreamType* get_ctnstream() const;
	IntStreamType* get_istream() const;

	RecognizerType* get_rec();
	RecognizerType const* get_rec() const;

	//same as above. Just that get_istream exists for lexer, parser, treeparser
	//get_parser_istream exists only for parser, treeparser. So use it accordingly
	IntStreamType* get_parser_istream() const;

	/** Set the input stream and reset the parser
	 */
	void setTreeNodeStream(TreeNodeStreamType* input);
	
	/** Return a pointer to the input stream
	 */
	TreeNodeStreamType* getTreeNodeStream();
	
	TokenType* getMissingSymbol( IntStreamType* istream,
				     ExceptionBaseType* e,
				     ANTLR_UINT32 expectedTokenType,
				     BitsetListType* follow);
	
	/** Pointer to a function that knows how to free resources of an ANTLR3 tree parser.
	 */
	~TreeParser();

	void fillExceptionData( ExceptionBaseType* ex );
	void displayRecognitionError( ANTLR_UINT8** tokenNames, ExceptionBaseType* ex );
	void exConstruct();
	void mismatch(ANTLR_UINT32 ttype, BitsetListType* follow);
	
	bool hasException() const;
	bool hasFailed() const;
	bool get_failedflag() const;
	void set_failedflag( bool failed );
	ANTLR_INT32 get_backtracking() const;
	void set_backtracking(ANTLR_INT32);


	const CommonTokenType* matchToken( ANTLR_UINT32 ttype, BitsetListType* follow );
	void matchAnyToken();

	RecognizerSharedStateType* get_psrstate() const;
	void consume();
	void followPush( const BitsetListType& follow );
	void followPop();
	void precover();
	void preporterror();
	ANTLR_UINT32 LA(ANTLR_INT32 i);
	void set_perror_recovery( bool val );

	TokenStreamType* get_strstream() const;
};

template<class ImplTraits, class UPDOWN>
class TreeVisitor
{
public:
	typedef typename ImplTraits::TreeType TreeType;
	typedef typename ImplTraits::TreeTypePtr TreeTypePtr;
	typedef typename ImplTraits::TreeAdaptorType TreeAdaptorType;

    TreeVisitor(TreeAdaptorType* adaptor, TreeFilter<ImplTraits>*);

    /** Visit every node in tree t and trigger an action for each node
     *  before/after having visited all of its children.
     *  Execute both actions even if t has no children.
     *  If a child visit yields a new child, it can update its
     *  parent's child list or just return the new child.  The
     *  child update code works even if the child visit alters its parent
     *  and returns the new tree.
     *
     *  Return result of applying post action to this node.
     */
    template<class TreeVisitorAction>
    TreeType const* visit(TreeType const* t, TreeVisitorAction* action);
protected:
    TreeAdaptorType *m_adaptor;
};

template<class ImplTraits>
class TreeFilter : public TreeParser<ImplTraits>
{
public:
	typedef TreeParser<ImplTraits> BaseType;
	typedef typename BaseType::RecognizerType BaseRecognizerType;
	typedef typename ImplTraits::TreeParserType SuperType;

	typedef typename BaseType::TreeNodeStreamType TreeNodeStreamType;
	typedef typename BaseType::RecognizerSharedStateType RecognizerSharedStateType;
	typedef typename BaseType::TreeTypePtr TreeTypePtr;
	typedef typename BaseType::TreeType TreeType;

	typedef typename ImplTraits::TreeAdaptorType TreeAdaptorType;

	TreeFilter( ANTLR_UINT32 sizeHint, TreeNodeStreamType* ctnstream, RecognizerSharedStateType* state);

	SuperType* get_super();

    // methods the downup strategy uses to do the up and down rules.
    // to override, just define tree grammar rule topdown and turn on
    // filter=true.
	void downup(TreeTypePtr const&);
	void downup(TreeType const*);

	// To be overridden by the grammar
	void topdown();
	void bottomup();

	class Rules
	{
	public:
		virtual TreeType const* pre(TreeType const*) = 0;
		virtual TreeType const* post(TreeType const*) = 0;
		virtual void rule() = 0;
	};

	class TopDown : public Rules
    {
		public:
		TopDown(TreeFilter *f)
    		: m_parent(f)
    	{};

		void rule()
		{
			m_parent->get_super()->topdown();
        }

		TreeType const* pre(TreeType const*);
		TreeType const* post(TreeType const*);

		TreeFilter *m_parent;
    };

	class BottomUp : public Rules
    {
    	public:
		BottomUp(TreeFilter *f)
    		: m_parent(f)
    	{};

    	void rule()
    	{
            m_parent->get_super()->bottomup();
        }
    	TreeType const* pre(TreeType const* tree)
    	{
    		m_parent->applyOnce(tree, this);
    		return tree;
    	};
		TreeType const* post(TreeType const* tree)
		{
			m_parent->applyOnce(tree, this);
			return tree;
		};

		TreeFilter *m_parent;
    };

	void applyOnce(TreeTypePtr const&, Rules*);
	void applyOnce(TreeType const*, Rules*);

};

}

#include "antlr3treeparser.inl"

#endif
