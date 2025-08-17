#include "Stdafx.h"
#include "Delegates.h"
#include "Resource.h"

namespace Resource
{
	void Initialize(const Initializer& initializer)
	{
		OnInitialize.Notify(initializer);
	}

	void Uninitialize()
	{
		OnUninitialize.Notify();
	}
}
