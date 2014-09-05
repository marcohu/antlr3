namespace antlr3 {

template< class ImplTraits >
TreeParser<ImplTraits>::TreeParser( ANTLR_UINT32 sizeHint, TreeNodeStreamType* ctnstream,
											RecognizerSharedStateType* state)
											:RecognizerType( sizeHint, state )
{
	/* Install the tree node stream
	*/
	setTreeNodeStream(ctnstream);

}

template< class ImplTraits >
TreeParser<ImplTraits>::~TreeParser()
{
	this->get_rec()->get_state()->get_following().clear();
}

template< class ImplTraits >
typename TreeParser<ImplTraits>::TreeNodeStreamType* TreeParser<ImplTraits>::get_ctnstream() const
{
	return m_ctnstream;
}

template< class ImplTraits >
typename TreeParser<ImplTraits>::IntStreamType* TreeParser<ImplTraits>::get_istream() const
{
	return m_ctnstream;
}

template< class ImplTraits >
typename TreeParser<ImplTraits>::IntStreamType* TreeParser<ImplTraits>::get_parser_istream() const
{
	return m_ctnstream;
}

template< class ImplTraits >
typename TreeParser<ImplTraits>::RecognizerType* TreeParser<ImplTraits>::get_rec()
{
	return this;
}

template< class ImplTraits >
typename TreeParser<ImplTraits>::RecognizerType const* TreeParser<ImplTraits>::get_rec() const
{
	return this;
}

template< class ImplTraits >
void TreeParser<ImplTraits>::fillExceptionData( ExceptionBaseType* ex )
{
	ex->set_token(new TokenType(m_ctnstream->LT(1)));	    /* The "clone" of the Current input tree node */
	ex->set_line( ex->get_token()->get_line() );
	ex->set_charPositionInLine( ex->get_token()->get_charPositionInLine() );
	ex->set_index( m_ctnstream->index() );

	// Are you ready for this? Deep breath now...
	//
	{
		auto tnode = ex->get_token();
		if (tnode->get_token() == NULL)
		{
			ex->set_streamName("-unknown source-" );
		}
		else
		{
			if ( tnode->get_token()->get_input() == NULL)
			{
				ex->set_streamName("");
			}
			else
			{
				ex->set_streamName( tnode->get_token()->get_input()->get_fileName() );
			}
		}
		ex->set_message("Unexpected node");
	}
}

template< class ImplTraits >
void TreeParser<ImplTraits>::displayRecognitionError( ANTLR_UINT8** tokenNames, ExceptionBaseType* ex )
{
	typename ImplTraits::StringStreamType errtext;
	// See if there is a 'filename' we can use
	//
	if( ex->get_streamName().empty() )
	{
		if(ex->get_token()->get_type() == ImplTraits::CommonTokenType::TOKEN_EOF)
		{
			errtext << "-end of input-(";
		}
		else
		{
			errtext << "-unknown source-(";
		}
	}
	else
	{
		errtext << ex->get_streamName() << "(";
	}
	// Next comes the line number
	errtext << get_psrstate()->get_exception()->get_line() << ") ";

	errtext << " : error " << get_psrstate()->get_exception()->getType()
			<< " : " << get_psrstate()->get_exception()->get_message();

	errtext << ", at offset " << get_psrstate()->get_exception()->get_token()->get_charPositionInLine()
			<< ", near " << get_psrstate()->get_exception()->get_token()->toStringTree();

	ex->displayRecognitionError( tokenNames, errtext );
	ImplTraits::displayRecognitionError( errtext.str() );
}

template< class ImplTraits >
void
TreeParser<ImplTraits>::setTreeNodeStream(TreeNodeStreamType* input)
{
	m_ctnstream = input;
    this->get_rec()->reset();
    m_ctnstream->reset();
}

template< class ImplTraits >
typename TreeParser<ImplTraits>::TreeNodeStreamType*
TreeParser<ImplTraits>::getTreeNodeStream()
{
	return m_ctnstream;
}

template< class ImplTraits >
void TreeParser<ImplTraits>::exConstruct()
{
	new ANTLR_Exception<ImplTraits, MISMATCHED_TREE_NODE_EXCEPTION, TreeNodeStreamType>( this->get_rec(), "" );
}

template< class ImplTraits >
void TreeParser<ImplTraits>::mismatch(ANTLR_UINT32 ttype, BitsetListType* follow)
{
	exConstruct();
    this->recoverFromMismatchedToken(ttype, follow);
}

template< class ImplTraits >
typename TreeParser<ImplTraits>::TokenType*
TreeParser<ImplTraits>::getMissingSymbol( IntStreamType* istream, ExceptionBaseType *e, ANTLR_UINT32 expectedTokenType, BitsetListType*	follow)
{
	StringType text("<missing ");
	text.append((const char *)this->get_rec()->get_state()->get_tokenName(expectedTokenType));
	text.append(">");
	// Create the token text that shows it has been inserted
	// this token is NOT yet owner by anybody
	//
	CommonTokenType* token = new CommonTokenType();
	token->set_tokText( text );
	token->set_type(expectedTokenType);

	TreeType *node = new TreeType(token);

	//TreeTypePtr node = e->get_input()->getTreeAdaptor()->create(expectedTokenType, text.c_str());

	// Finally return the pointer to our new node
	return	node;
}

template<class ImplTraits>
typename TreeParser<ImplTraits>::RecognizerSharedStateType*
TreeParser<ImplTraits>::get_psrstate() const
{
	return get_rec()->get_state();
}

template<class ImplTraits>
bool
TreeParser<ImplTraits>::hasException() const
{
    return get_psrstate()->get_error();
}

template< class ImplTraits>
const typename TreeParser<ImplTraits>::CommonTokenType*
TreeParser<ImplTraits>::matchToken( ANTLR_UINT32 ttype, BitsetListType* follow )
{
	auto node = get_rec()->match(ttype, follow);
	return (node ? node->get_token() : NULL);
}

template< class ImplTraits>
ANTLR_INLINE void
TreeParser<ImplTraits>::consume()
{
	this->get_istream()->consume();
}

template<class ImplTraits>
void
TreeParser<ImplTraits>::followPush(const BitsetListType& follow)
{
	get_psrstate()->get_following().push(follow);
}

template<class ImplTraits>
void
TreeParser<ImplTraits>::followPop()
{
	get_psrstate()->get_following().pop();
}

template<class ImplTraits>
void
TreeParser<ImplTraits>::precover()
{
	return this->get_rec()->recover();
}

template<class ImplTraits>
void
TreeParser<ImplTraits>::preporterror()
{
	return get_rec()->reportError();
}
template<class ImplTraits>
ANTLR_UINT32
TreeParser<ImplTraits>::LA(ANTLR_INT32 i)
{
	return this->get_istream()->LA(i);
}

template< class ImplTraits>
void
TreeParser<ImplTraits>::set_perror_recovery( bool val )
{
	this->get_psrstate()->set_errorRecovery(val);
}

template< class ImplTraits>
bool
TreeParser<ImplTraits>::hasFailed() const
{
	return this->get_psrstate()->get_failed();
}

template< class ImplTraits>
bool
TreeParser<ImplTraits>::get_failedflag() const
{
	return this->get_psrstate()->get_failed();
}

template< class ImplTraits>
void
TreeParser<ImplTraits>::set_failedflag( bool failed )
{
	this->get_psrstate()->set_failed(failed);
}

template< class ImplTraits>
ANTLR_INT32
TreeParser<ImplTraits>::get_backtracking() const
{
	return this->get_psrstate()->get_backtracking();
}

template< class ImplTraits>
void
TreeParser<ImplTraits>::set_backtracking(ANTLR_INT32 level)
{
	return this->get_psrstate()->set_backtracking(level);
}

template<class ImplTraits>
typename TreeParser<ImplTraits>::TokenStreamType*
TreeParser<ImplTraits>::get_strstream() const
{
	return m_ctnstream;
}


template<class ImplTraits, class UPDOWN>
TreeVisitor<ImplTraits, UPDOWN>::TreeVisitor(TreeAdaptorType* adaptor, TreeFilter<ImplTraits>*)
	: m_adaptor(adaptor)
{

}

template<class ImplTraits, class UPDOWN>
template<class TreeVisitorAction>
typename TreeVisitor<ImplTraits, UPDOWN>::TreeType const*
TreeVisitor<ImplTraits, UPDOWN>::visit(TreeType const* t, TreeVisitorAction* action)
{

    // System.out.println("visit "+((Tree)t).toStringTree());
    bool isNil = m_adaptor->isNilNode(t);
    if ( action != NULL && !isNil )
    	t = action->pre(t); // if rewritten, walk children of new t

	for (ANTLR_UINT32 i = 0; i < m_adaptor->getChildCount(t); i++)
    {
		TreeTypePtr &child = m_adaptor->getChild(const_cast<TreeType*>(t), i);
        auto *visitResult = visit(child.get(), action);
        //Object childAfterVisit = m_adaptor->getChild(t, i);
        //if ( visitResult !=  childAfterVisit ) { // result & child differ?
        //    adaptor->setChild(t, i, visitResult);
        //}
    }

    if ( action != NULL && !isNil )
		t = action->post(t);

	return t;
}

template<class ImplTraits>
TreeFilter<ImplTraits>::TreeFilter(ANTLR_UINT32 sizeHint, TreeNodeStreamType* ctnstream, RecognizerSharedStateType* state)
	: BaseType(sizeHint, ctnstream, state)
{
}

template<class ImplTraits>
typename TreeFilter<ImplTraits>::SuperType *
TreeFilter<ImplTraits>::get_super()
{
	return static_cast<SuperType*>(this);
}

template<class ImplTraits>
void
TreeFilter<ImplTraits>::applyOnce(const TreeTypePtr& tree, Rules* rule)
{
	applyOnce(tree.get(), rule);
}

template<class ImplTraits>
void
TreeFilter<ImplTraits>::applyOnce(const TreeType* tree, Rules* rules)
{
    if ( tree == NULL ) return;

    // share TreeParser object but not parsing-related state
    BaseType::set_state(new RecognizerSharedStateType());
    //TODO BaseType::m_ctnstream = new TreeNodeStreamType(originalAdaptor, t);
    //BaseType::m_ctnstream->setTokenStream(originalTokenStream);
    BaseType::set_backtracking(1);
    rules->rule();
    BaseType::set_backtracking(0);
}

template<class ImplTraits>
void
TreeFilter<ImplTraits>::bottomup()
{
}

template<class ImplTraits>
void
TreeFilter<ImplTraits>::downup(const TreeTypePtr &tree)
{
    TreeVisitor<ImplTraits, BottomUp> *v = new TreeVisitor<ImplTraits, BottomUp>(new TreeAdaptorType(), this);
    BottomUp *actions = new BottomUp(this);
    v->visit(tree.get(), actions);
}

template<class ImplTraits>
void
TreeFilter<ImplTraits>::downup(const TreeType* tree)
{
}

template<class ImplTraits>
void
TreeFilter<ImplTraits>::topdown()
{
}

}
