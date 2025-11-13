#include "Pch.h"
#include "Delegates.h"
#include "Manager.h"
#include "Resource.h"

namespace Resource
{
	void Initialize(const Initializer& initializer)
	{
		Manager::Instantiate();
		Delegates::OnInitialize.Broadcast(initializer);
	}

	void Finalize()
	{
		Manager::Destroy();
		Delegates::OnFinalize.Broadcast();
	}
}
