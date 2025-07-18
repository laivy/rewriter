export module rewriter.tool.engine.delegates;

import std;
import rewriter.common.delegate;
import rewriter.library.resource;

export namespace Delegates
{
	extern Delegate<int, int> OnWindowResized;
	extern Delegate<const std::shared_ptr<Resource::Property>&> OnPropAdded;
	extern Delegate<const std::shared_ptr<Resource::Property>&> OnPropDeleted;
	extern Delegate<const std::shared_ptr<Resource::Property>&> OnPropModified;
	extern Delegate<const std::shared_ptr<Resource::Property>&> OnPropSelected;
}
