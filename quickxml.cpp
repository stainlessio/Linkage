#include "stdafx.h"
#include "quickxml.h"
#include <new>

static const CString Dummy = "";

class QuickXMLNodeImplementation
{
	public:
	class QuickXMLNode *m_pFirstChild;
	class QuickXMLNode *m_pNext;
	CString m_NameOrData;
	CString m_Attributes;
	bool m_bLink;
};

QuickXMLNode::QuickXMLNode( bool bLink )
{
	m_pImplementation = new QuickXMLNodeImplementation;
	if( m_pImplementation != 0 )
	{
		m_pImplementation->m_bLink = bLink;
		m_pImplementation->m_pFirstChild = 0;
		m_pImplementation->m_pNext = 0;
	}
}

void QuickXMLNode::Clean( void )
{
	if( m_pImplementation == 0 )
		return;
	// Must free any children
	QuickXMLNode * pChild = m_pImplementation->m_pFirstChild;
	while( pChild != 0 )
	{
		QuickXMLNode *pNext = pChild->m_pImplementation->m_pNext;
		delete  pChild;
		pChild = pNext;
	}
	m_pImplementation->m_pFirstChild = 0;
}

QuickXMLNode::~QuickXMLNode()
{
	Clean();
	if( m_pImplementation != 0 )
	{
		delete m_pImplementation;
		m_pImplementation = 0;
	}
}

bool QuickXMLNode::Parse( const char *pText )
{
	if( m_pImplementation == 0 )
		return false;

	bool bSuccess = Parse( pText, false ) != 0;
	if( !bSuccess )
		Clean();
	return bSuccess;
}

const char *QuickXMLNode::Parse( const char *pText, bool bExpectEndTag )
{
	if( m_pImplementation == 0 )
		return false;

	m_pImplementation->m_pFirstChild = 0;
	QuickXMLNode *pLastChild = 0;

	while( pText != 0 && *pText != '\0' )
	{
		const char *pTag = pText + strcspn( pText, "<" );
		if( *pTag == '\0' )
			return bExpectEndTag ? 0 : pText;
		const char *pTagData = pTag + 1;
		if( *pTagData == '\0' )
			return 0;

		const char *pLinkData = pText + strspn( pText, " \t\r\n" );
		if( pLinkData != pTag )
		{
			// Save the data as a data Link
			QuickXMLNode *pNewNode = new QuickXMLNode( false );
			pNewNode->m_pImplementation->m_NameOrData.Append( pText, pTag - pText );
			if( pLastChild == 0 )
				m_pImplementation->m_pFirstChild = pNewNode;
			else
				pLastChild->m_pImplementation->m_pNext = pNewNode;
			pLastChild = pNewNode;
		}

		if( *pTagData == '/' )
		{
			// Is this the end tag we want? It had better be!
			++pTagData;
			const char * pEnd = pTagData + strcspn( pTagData, ">" );
			if( *pEnd == '\0' )
				return 0;
			CString EndTag;
			EndTag.Append( pTagData, pEnd - pTagData );
			if( m_pImplementation->m_NameOrData != EndTag )
				return 0;
			// Done. Return pointer after this end tag.
			return pEnd + 1;
		}
		else
		{
			// Get the Link name
			++pTag;
			const char * pEnd = pTag + strcspn( pTag, "><" );
			if( *pEnd == '\0' || *pEnd == '<' )
				return 0;

			if( pEnd[-1] == '/' )
				--pEnd;

			const char * pNameEnd = pTag + strcspn( pTag, " " );
			if( pNameEnd > pEnd )
				pNameEnd = pEnd;

			CString Tag;

			// Done. Return pointer after this end tag.
			// Now parse the new Link.
			QuickXMLNode *pNewNode = new QuickXMLNode( true );
			pNewNode->m_pImplementation->m_NameOrData.Append( pTag, pNameEnd - pTag );
			if( pNameEnd != pEnd )
			{
				++pNameEnd;
				pNewNode->m_pImplementation->m_Attributes.Append( pNameEnd, pEnd - pNameEnd );
			}
			if( pLastChild == 0 )
				m_pImplementation->m_pFirstChild = pNewNode;
			else
				pLastChild->m_pImplementation->m_pNext = pNewNode;
			pLastChild = pNewNode;

			if( *pEnd == '/' )
				pText = pEnd + 2;
			else
				pText = pNewNode->Parse( pEnd+1, true );
		}
	}
	return pText;
}

class QuickXMLNode* QuickXMLNode::FindChildByName( const char *pName )
{
	if( m_pImplementation == 0 )
		return 0;

	QuickXMLNode *pNode = m_pImplementation->m_pFirstChild;
	while( pNode != 0 )
	{
		if( pNode->m_pImplementation->m_NameOrData == pName )
			return pNode;
		pNode = pNode->m_pImplementation->m_pNext;
	}
	return 0;
}

const char* QuickXMLNode::FindChildDataByName( const char *pName )
{
	if( m_pImplementation == 0 )
		return 0;

	QuickXMLNode *pNode = FindChildByName( pName );
	if( pNode == 0 )
		return 0;
	pNode = pNode->m_pImplementation->m_pFirstChild;
	while( pNode != 0 )
	{
		if( !pNode->m_pImplementation->m_bLink )
			return pNode->m_pImplementation->m_NameOrData;
		pNode = pNode->m_pImplementation->m_pNext;
	}
	return 0;
}

class QuickXMLNode* QuickXMLNode::GetFirstChild( void )
{
	if( m_pImplementation == 0 )
		return 0;

	return m_pImplementation->m_pFirstChild;
}

bool QuickXMLNode::IsLink( void )
{
	if( m_pImplementation == 0 )
		return false;

	return m_pImplementation->m_bLink;
}

const CString & QuickXMLNode::GetText( void )
{
	if( m_pImplementation == 0 )
		return Dummy;

	return m_pImplementation->m_NameOrData;
}

class QuickXMLNode* QuickXMLNode::GetNextSibling( void )
{
	if( m_pImplementation == 0 )
		return 0;

	return m_pImplementation->m_pNext;
}

const CString & QuickXMLNode::GetAttribute( const char *pAttribute )
{
	static CString Value;

	if( m_pImplementation == 0 )
		return Dummy;

	CString Search = pAttribute;
	Search += "=\"";

	int Found = m_pImplementation->m_Attributes.Find( Search );
	if( Found < 0 )
		return Dummy;

	Found += Search.GetLength();

	int End = m_pImplementation->m_Attributes.Find( "\"", Found );
	if( End < 0 )
		return Dummy;

	Value = m_pImplementation->m_Attributes.Mid( Found, End - Found );

	return Value;
}
