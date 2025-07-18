module rewriter.tool.engine.delegates;

import std;
import rewriter.common.delegate;
import rewriter.library.resource;

namespace Delegates
{
	Delegate<int, int> OnWindowResized;
	Delegate<const std::shared_ptr<Resource::Property>&> OnPropAdded;
	Delegate<const std::shared_ptr<Resource::Property>&> OnPropDeleted;
	Delegate<const std::shared_ptr<Resource::Property>&> OnPropModified;
	Delegate<const std::shared_ptr<Resource::Property>&> OnPropSelected;
}
