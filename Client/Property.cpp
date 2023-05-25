#include "Stdafx.h"
#include "Image.h"
#include "Property.h"

Property::Property() : 
	m_type{ Type::GROUP }, 
	m_data{} 
{ }

Property::Property(Type type, const std::any& data) : 
	m_type{ type },
	m_data{ data }
{ }

Property::~Property()
{
	switch (m_type)
	{
	case Type::INT:
		delete std::any_cast<int*>(m_data);
		break;
	case Type::INT2:
		delete std::any_cast<INT2*>(m_data);
		break;
	case Type::FLOAT:
		delete std::any_cast<float*>(m_data);
		break;
	case Type::STRING:
		delete std::any_cast<std::string*>(m_data);
		break;
	case Type::D2DImage:
	case Type::D3DImage:
		delete std::any_cast<Image*>(m_data);
		break;
	}
}
