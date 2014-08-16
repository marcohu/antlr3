namespace antlr3 {

template<class ImplTraits>
CommonTreeNodeStream<ImplTraits>::CommonTreeNodeStream(ANTLR_UINT32 hint)
{
	init(hint);
}

template<class ImplTraits>
CommonTreeNodeStream<ImplTraits>::CommonTreeNodeStream( TreeTypePtr& tree, ANTLR_UINT32 hint )
{
	init(hint);
	m_root = tree.get();
}

template<class ImplTraits>
void CommonTreeNodeStream<ImplTraits>::init( ANTLR_UINT32 hint )
{
	m_root = NULL;
	m_adaptor = new TreeAdaptorType;
	// Create the node list map
	//
	if (hint == 0)
		hint = DEFAULT_INITIAL_BUFFER_SIZE;
	m_nodes.reserve( DEFAULT_INITIAL_BUFFER_SIZE );

	m_p = -1;
	m_currentChildIndex = 0; 
	m_absoluteNodeIndex = 0;
	m_lookAhead = NULL;
	m_lookAheadLength = 0;
	m_head = 0;
	m_tail = 0;
	m_uniqueNavigationNodes = false;
	m_isRewriter = false;

	m_UP = m_adaptor->create(CommonTokenType::TOKEN_UP, "UP");
	m_DOWN = m_adaptor->create(CommonTokenType::TOKEN_DOWN, "DOWN");
	m_EOF_NODE = m_adaptor->create(CommonTokenType::TOKEN_EOF, "EOF");
	m_INVALID_NODE = m_adaptor->create(CommonTokenType::TOKEN_INVALID, "INVALID");
}

template<class ImplTraits>
CommonTreeNodeStream<ImplTraits>::CommonTreeNodeStream( const CommonTreeNodeStream& ctn )
{
	m_root = ctn.m_root;
	m_adaptor = ctn.m_adaptor;
	m_nodes.reserve( DEFAULT_INITIAL_BUFFER_SIZE );
	m_nodeStack = ctn.m_nodeStack;
	m_p = -1;
	m_currentNode = NULL;
	m_previousNode = NULL;
	m_currentChildIndex = 0; 
	m_absoluteNodeIndex = 0;
	m_lookAhead = NULL;
	m_lookAheadLength = 0;
	m_head = 0;
	m_tail = 0;
	m_uniqueNavigationNodes = false;
	m_isRewriter = true;

	m_UP = m_adaptor->create(CommonTokenType::TOKEN_UP, "UP");
	m_DOWN = m_adaptor->create(CommonTokenType::TOKEN_DOWN, "DOWN");
	m_EOF_NODE = m_adaptor->create(CommonTokenType::TOKEN_EOF, "EOF");
	m_INVALID_NODE = m_adaptor->create(CommonTokenType::TOKEN_INVALID, "INVALID");
}

template<class ImplTraits>
CommonTreeNodeStream<ImplTraits>::~CommonTreeNodeStream()
{
	// If this is a rewriting stream, then certain resources
	// belong to the originating node stream and we do not
	// free them here.
	//
	if	( m_isRewriter != true)
	{
		m_nodeStack.clear();
	}
	
	m_nodes.clear();
}

template<class ImplTraits>
bool
CommonTreeNodeStream<ImplTraits>::isEOF(ElementType o)
{
	return m_adaptor->getType(o) == CommonTokenType::TOKEN_EOF;
}

template<class ImplTraits>
ANTLR_UINT32
CommonTreeNodeStream<ImplTraits>::LA(ANTLR_INT32 i)
{
	TreeType* token = LT(i);
	return m_adaptor->getType(token);
	//return m_adaptor->getType(LT(i));
}

template<class ImplTraits>
typename CommonTreeNodeStream<ImplTraits>::TreeType*	CommonTreeNodeStream<ImplTraits>::getTreeSource()
{
	return m_root;
}

template<class ImplTraits>
typename CommonTreeNodeStream<ImplTraits>::TreeAdaptorType*	CommonTreeNodeStream<ImplTraits>::getTreeAdaptor()
{
	return m_adaptor;
}

template<class ImplTraits>
void  CommonTreeNodeStream<ImplTraits>::setUniqueNavigationNodes(bool uniqueNavigationNodes)
{
	m_uniqueNavigationNodes = uniqueNavigationNodes;
}

template<class ImplTraits>
typename CommonTreeNodeStream<ImplTraits>::StringType CommonTreeNodeStream<ImplTraits>::toString()
{
	return toString(m_root, NULL);
}

template<class ImplTraits>
typename CommonTreeNodeStream<ImplTraits>::StringType CommonTreeNodeStream<ImplTraits>::toString(TreeType* start, TreeType* stop)
{
	StringStreamType buf;
	toStringImpl(start, stop, buf);
	return buf.str();
}

template<class ImplTraits>
typename CommonTreeNodeStream<ImplTraits>::StringType CommonTreeNodeStream<ImplTraits>::toString(TreeTypePtr& start, TreeTypePtr& stop)
{
	StringStreamType buf;
	toStringImpl(start, stop, buf);
	return buf.str();
}

template<class ImplTraits>
void CommonTreeNodeStream<ImplTraits>::toStringImpl(TreeTypePtr& start, TreeTypePtr& stop, StringStreamType &buf)
{
	if (!start->isNilNode() )
	{
		StringType text = start->toString();
		if (text.empty())
		{
			buf << ' ';
			buf << start->getType();
		}
		else
			buf << text;
	}

	if (start == stop)
	{
		return; /* Finished */
	}

	auto &children = start->get_children();

	if (!children.empty() && !start->isNilNode())
	{
		buf << ' ';
		buf << CommonTokenType::TOKEN_DOWN;
	}

	for (auto &child: start->get_children())
	{
		this->toStringImpl(child, stop, buf);
	}

	if (!children.empty() && ! start->isNilNode() )
	{
		buf << ' ';
		buf << CommonTokenType::TOKEN_UP;
	}
}

template<class ImplTraits>
void CommonTreeNodeStream<ImplTraits>::replaceChildren(TreeTypePtr& parent, ANTLR_INT32 startChildIndex, ANTLR_INT32 stopChildIndex, TreeTypePtr& t)
{
	if (parent != NULL)
	{
		TreeAdaptorType* adaptor = this->getTreeAdaptor();
		adaptor->replaceChildren(parent, startChildIndex, stopChildIndex, t);
	}
}

template<class ImplTraits>
void CommonTreeNodeStream<ImplTraits>::addNavigationNode(ANTLR_UINT32 ttype)
{
	if (ttype == CommonTokenType::TOKEN_DOWN)
	{
		if (this->hasUniqueNavigationNodes() == true)
		{
		// 	m_nodes.push_back( this->newDownNode() );
		} else {
			m_nodes.push_back( m_DOWN );
		}
	}
	else
	{
		if (this->hasUniqueNavigationNodes() == true)
		{
		// 	m_nodes.push_back( this->newUpNode() );
		} else {
			m_nodes.push_back( m_UP );
		}
	}
}

template<class ImplTraits>
typename CommonTreeNodeStream<ImplTraits>::TreeTypePtr	CommonTreeNodeStream<ImplTraits>::newDownNode()
{
	return m_adaptor->create(CommonTokenType::TOKEN_DOWN, "DOWN");
}

template<class ImplTraits>
typename CommonTreeNodeStream<ImplTraits>::TreeTypePtr	CommonTreeNodeStream<ImplTraits>::newUpNode()
{
	return m_adaptor->create(CommonTokenType::TOKEN_UP, "UP");	
}

template<class ImplTraits>
bool  CommonTreeNodeStream<ImplTraits>::hasUniqueNavigationNodes() const
{
	 return  m_uniqueNavigationNodes;
}

template<class ImplTraits>
ANTLR_UINT32	CommonTreeNodeStream<ImplTraits>::getLookaheadSize()
{
	return	m_tail < m_head 
	    ?	(m_lookAheadLength - m_head + m_tail)
	    :	(m_tail - m_head);
}

template<class ImplTraits>
void	CommonTreeNodeStream<ImplTraits>::push(ANTLR_INT32 index)
{
	m_nodeStack.push(m_p);	// Save current index
	seek(index);
}

template<class ImplTraits>
ANTLR_INT32	CommonTreeNodeStream<ImplTraits>::pop()
{
	ANTLR_INT32 retVal = m_nodeStack.top();
	m_nodeStack.pop();
	seek(retVal);
	return retVal;
}

template<class ImplTraits>
void
CommonTreeNodeStream<ImplTraits>::reset()
{
	BaseType::reset();

	//it.reset();
    //hasNilRoot = false;
	//level = 0;
	//previousLocationElement = null;

	m_nodeStack.clear();
}

template<class ImplTraits>
void
CommonTreeNodeStream<ImplTraits>::syncAhead(int need)
{
	fillBufferRoot();
}

template<class ImplTraits>
void
CommonTreeNodeStream<ImplTraits>::fillBufferRoot()
{
	// Call the generic buffer routine with the root as the
	// argument
	//
	this->fillBuffer(m_root);
	m_p = 0; // Indicate we are at buffer start
}

template<class ImplTraits>
void
CommonTreeNodeStream<ImplTraits>::fillBuffer(TreeType* t)
{
	bool nilNode = m_adaptor->isNilNode(t);

	// If the supplied node is not a nil (list) node then we
	// add in the node itself to the vector
	//
	if (nilNode == false)
	{
		m_nodes.push_back(t);
	}

	// Only add a DOWN node if the tree is not a nil tree and
	// the tree does have children.
	//
	auto &children = t->get_children();

	if (nilNode == false && !children.empty())
	{
		addNavigationNode( CommonTokenType::TOKEN_DOWN);
	}

	// We always add any children the tree contains, which is
	// a recursive call to this function, which will cause similar
	// recursion and implement a depth first addition
	//
	for (auto &child: children)
	{
		this->fillBuffer(child.get());
	}

	// If the tree had children and was not a nil (list) node, then we
	// we need to add an UP node here to match the DOWN node
	//
	if (nilNode == false && !children.empty())
	{
		addNavigationNode(CommonTokenType::TOKEN_UP);
	}
}

}

