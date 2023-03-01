#include "Stdafx.h"
#include "NytProperty.h"
#include "NytUI.h"
#include "NytImage.h"

NytProperty::NytProperty() : m_type{ NytDataType::GROUP }, m_data{} { }
NytProperty::NytProperty(NytDataType type, const std::any& data) : m_type{ type }
{
	m_data = std::move(data);
}

NytProperty::~NytProperty()
{
	switch (m_type)
	{
	case NytDataType::INT:
		delete std::any_cast<int*>(m_data);
		break;
	case NytDataType::INT2:
		delete std::any_cast<INT2*>(m_data);
		break;
	case NytDataType::FLOAT:
		delete std::any_cast<float*>(m_data);
		break;
	case NytDataType::STRING:
		delete std::any_cast<std::string*>(m_data);
		break;
	case NytDataType::UI:
		delete std::any_cast<NytUI*>(m_data);
		break;
	case NytDataType::IMAGE:
		delete std::any_cast<NytImage*>(m_data);
		break;
	}
}
