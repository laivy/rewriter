#include "Stdafx.h"
#include "Delegates.h"
#include "Resource.h"

namespace Resource
{
	void Initialize(const Initializer& initializer)
	{
		Delegates::OnInitialize.Broadcast(initializer);
	}

	void Uninitialize()
	{
		Delegates::OnUninitialize.Broadcast();
	}
}
