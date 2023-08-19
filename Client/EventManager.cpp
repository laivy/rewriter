#include "Stdafx.h"
#include "EventManager.h"
#include "ObjectManager.h"
#include "WndManager.h"

void EventManager::Clear()
{
	OnSceneChange.Clear();
}

void EventManager::Event::Clear()
{
	m_callbacks.clear();
}

void EventManager::Event::operator()()
{
	for (const auto& c : m_callbacks)
		c();
}