#pragma once
#include "stdafx.h"

class CNullableColor
{
	public:

	CNullableColor() { m_bIsSet = false; m_Color = RGB( 0, 0, 0 ); }
	CNullableColor( const CNullableColor &NullableColor ) { m_bIsSet = NullableColor.m_bIsSet; m_Color = NullableColor.m_Color; }
	CNullableColor( const COLORREF Color ) { m_bIsSet = true; m_Color = Color; }
	virtual ~CNullableColor() {}

	_inline COLORREF GetColor( void ) const { return m_bIsSet ? m_Color : RGB( 0, 0, 0 ); }
	_inline void SetColor( COLORREF Color ) { m_bIsSet = true; m_Color = Color; }
	_inline void operator=( const COLORREF Color ) { SetColor( Color ); }
	_inline bool IsSet( void ) const { return m_bIsSet; }

	// Check to see if this is not set by tesing if( CNullableColor_variable == 0 )...
	_inline bool operator==( int Value ) const { return ( !m_bIsSet && Value == 0 ) ? true : false; }

	// Check to see if this is set by tesing if( CNullableColor_variable != 0 )...
	_inline bool operator!=( int Value ) const { return ( m_bIsSet && Value == 0 ) ? true : false; }

	operator const COLORREF () { return GetColor(); }

	private:
	bool m_bIsSet;
	COLORREF m_Color;
};

