#if !defined( _BITARRAY_H_ )
#define _BITARRAY_H_

#include <new>
#include "memory.h"

static _inline int ByteLength( int Bits ) { return ( Bits + 7 ) / 8; }

class CBitArray
{
	public:
	CBitArray()
	{
		ASSERT( sizeof( unsigned char ) == 1 );
		m_Length = 0;
		m_pData = 0;
	}
	
	~CBitArray()
	{
		if( m_pData != 0 )
			delete [] m_pData;
		m_pData = 0;
		m_Length = 0;
	}
	
	bool SetLength( int NewBitCount )
	{
		if( NewBitCount == m_Length )
			return true;

		int OldBitCount = m_Length;
		int NewByteLength = ByteLength( NewBitCount );
		int OldByteLength = ByteLength( m_Length );
		
		if( NewByteLength > OldByteLength )
		{
			// More bytes are needed!
			unsigned char *pNewData = new unsigned char[NewByteLength];
			if( pNewData == 0 )
				return false;
			memcpy( pNewData, m_pData, OldByteLength );
			memset( pNewData + OldByteLength, 0, NewByteLength - OldByteLength );
			unsigned char *pTemp = m_pData;
			m_pData = pNewData;
			delete pTemp;
		}
		//else if( NewByteLength < OldByteLength - 1 )
		//	memset( m_pData + NewByteLength, 0, OldByteLength - NewByteLength );
		
		/*
		 * If the bit count is going up but the byte count is the same, it is possible for the
		 * new bits to be non-zero. Clear those bits without touching bits already being used.
		 * The only way this is really possible is if the bit count had gone down then up again.
		 */
		if( NewByteLength == OldByteLength && NewBitCount > OldBitCount )
		{
			int LastByte = NewByteLength - 1;
			m_pData[LastByte] = m_pData[LastByte] &  ( 0x00FF >> ( 8 - ( OldBitCount ) % 8 ) );
		}
		m_Length = NewBitCount;
		
		return true;
	}
	
	int FindAndSetBit( void )
	{
		int NewBit = FindBit( 0 );
		if( NewBit >= 0 )
			SetBit( NewBit );
		return NewBit;
	}
	
	int FindBit( int BitOfValue = 0 )
	{
		if( BitOfValue != 0 )
			BitOfValue = 1;
		int Counter = 0;
		int Byte;
		int Bit;
		for( ; Counter < m_Length; ++Counter )
		{
			Byte = Counter / 8;
			Bit = Counter % 8;
			if( ( ( m_pData[Byte] >> Bit ) & 0x01 ) == BitOfValue )
				return Counter;
		}
		
		// If checking for a set bit, there are none so return -1.
		if( BitOfValue != 0 )
			return -1;
			
		SetLength( m_Length + 32 );
		return Counter;
	}
	
	int GetBit( int Index )
	{
		if( Index >= m_Length )
			SetLength( Index + 1 );
			
		int Byte = Index / 8;
		int Bit = Index % 8;
		int TheBitSet = 1 << Bit;
		
		return ( m_pData[Byte] >> Bit ) & 0x01;
	}
	
	void SetBit( int Index ) { return SetBitTo( Index, 1 ); }
	void ClearBit( int Index ) { return SetBitTo( Index, 0 ); }
	
	private:
	unsigned char *m_pData;
	int m_Length;

	void SetBitTo( int Index, int BitOfValue )
	{
		if( Index < 0 )
			return;
			
		if( Index >= m_Length )
			SetLength( Index + 1 );
			
		int Byte = Index / 8;
		int Bit = Index % 8;
		int TheBitSet = 1 << Bit;
		
		if( BitOfValue == 0 )
			m_pData[Byte] &= ~TheBitSet;
		else
			m_pData[Byte] |= TheBitSet;
	}
};

#endif

