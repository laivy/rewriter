#include "Stdafx.h"
#include "NytProperty.h"
#include "NytImage.h"

NytProperty::NytProperty() : m_type{ NytType::GROUP }, m_data{} { }
NytProperty::NytProperty(NytType type, const std::any& data) : m_type{ type }
{
	m_data = data;
}

NytProperty::~NytProperty()
{
	switch (m_type)
	{
	case NytType::INT:
		delete std::any_cast<int*>(m_data);
		break;
	case NytType::INT2:
		delete std::any_cast<INT2*>(m_data);
		break;
	case NytType::FLOAT:
		delete std::any_cast<float*>(m_data);
		break;
	case NytType::STRING:
		delete std::any_cast<std::string*>(m_data);
		break;
	case NytType::D2DImage:
	case NytType::D3DImage:
		delete std::any_cast<NytImage*>(m_data);
		break;
	}
}
