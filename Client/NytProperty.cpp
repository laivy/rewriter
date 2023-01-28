#include "Stdafx.h"
#include "NytProperty.h"

NytProperty::NytProperty() : m_type{ NytDataType::GROUP }, m_data{}
{

}

NytProperty::NytProperty(NytDataType type, const std::any& data) : m_type{ type }, m_data{ data }
{

}