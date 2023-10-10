#include "Stdafx.h"
#include "Image.h"
#include "Property.h"

Property::Property() : 
	m_type{ Type::GROUP }, 
	m_data{} 
{

}

Property::Property(Type type, const Data& data) :
	m_type{ type },
	m_data{ data }
{

}
