#pragma once

class Node;

namespace Global
{
	extern Event<Node*> OnNodeAdd;
	extern Event<Node*> OnNodeDelete;
	extern Event<Node*> OnNodeSelect;
}