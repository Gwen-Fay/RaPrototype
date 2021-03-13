/*
 *
 */

#include "NodeSDF.h"


	Stack::Stack()
	{
		//fills stack with available indicies, 0 to max.
		for (int i = (sizeof(stack) / sizeof(int)) - 1; i >= 0; i--)
		{
			push(i);
		}
	}

	void Stack::push(int n)
	{
		if (current < sizeof(stack) / sizeof(int))
		{
			stack[current] = n;
			current++;
		}
	}

	int Stack::pop()
	{
		if (current > 0)
		{
			current--;
			int value = stack[current];
			stack[current] = 0;
			return value;
		}
	}

	bool Stack::full()
	{
		return current >= (sizeof(stack) / sizeof(int));
	}

	bool Stack::empty()
	{
		return (current <= 0);
	}

	int Stack::peak()
	{
		return stack[current];
	}

	//TODO make it so TreSDF actually works
	//TODO move NodeSDF data onto heap

	TreeSDF::TreeSDF()
	{
		roots = new int[200];
		nodes = new NodeSDF[200];
		children = new int[100 * 20];
		isUsed = new bool[200];
		for (int i = 0; i < 200; i++)
		{
			isUsed[i] = false;
			//-1 is the terminator value for the array
			roots[i] = -1;
		}
		for (int i = 0; i < 100 * 20; i++)
		{
			children[i] = -1;
		}

	}

	TreeSDF::~TreeSDF()
	{
		delete[] nodes;
		delete[] isUsed;
		delete[] roots;
		delete[] children;
	}

	NodeSDF* TreeSDF::newNode()
	{
		if (!stack.empty())
		{
			int index = stack.pop();
			if (index >= emptyIndex)
			{
				emptyIndex = index + 1;
			}
			if (lastRoot < arraySize)
			{
				roots[lastRoot] = index;
				lastRoot++;
			}
			isUsed[index] = true;
			NodeSDF* pointer = &nodes[index];
			return pointer;
		}
		else
		{
			return nullptr;
		}
	}

	NodeSDF* TreeSDF::newChild(NodeSDF* parent)
	{
		if (!stack.empty())
		{
			int index = stack.pop();
			if (index >= emptyIndex)
			{
				emptyIndex = index + 1;
			}
			isUsed[index] = true;

			int parentID = parent - nodes;

			if (parent->childArrayID == -1) {
				//set up new childID
				int childID = -1;
				for (int i = 0; i < 100; i++)
				{
					if (children[i * 20] == -1)
					{
						childID = i;
						break;
					}
				}
				if (childID != -1)
				{
					parent->childArrayID = childID;
					nodes[index].parentID = parentID;
					children[childID * 20] = index;
				}
			}
			else
			{
				int childID = -1;
				for (int i = 0; i < 20; i++)
				{
					if (children[parent->childArrayID * 20 + i] == -1)
					{
						childID = parent->childArrayID * 20 + i;
						break;
					}
				}
				if (childID != -1)
				{
					nodes[index].parentID = parentID;
					children[childID] = index;
				}
			}

			NodeSDF* pointer = &nodes[index];

			return pointer;
		}
		else
		{
			return nullptr;
		}
	}


	void TreeSDF::delNode(NodeSDF* node)
	{
		int index = node - nodes;
		if (nodes[index].childArrayID != -1)
		{
			for (int i = 0; i < 20; i++)
			{
				if (children[nodes[index].childArrayID * 20 + i] != -1)
				{

					isUsed[children[nodes[index].childArrayID * 20 + i]] = false;
					children[nodes[index].childArrayID * 20 + i] = -1;
				}
			}
		}

		if (nodes[index].parentID != -1)
		{
			int parentNode = nodes[index].parentID;
			int childID = nodes[parentNode].childArrayID;
			for (int i = 0; i < 20; i++)
			{
				if (children[childID * 20 + i] == index)
				{
					children[childID * 20 + i] = -1;
					break;
				}
			}


		}
		else
		{
			//remove from roots
			bool foundRoot = false;
			for (int i = 0; i < arraySize; i++)
			{
				if (foundRoot == true)
				{
					roots[i - 1] = roots[i];
				}
				if (roots[i] == index)
				{
					foundRoot = true;
					if (i == (arraySize - 1))
					{
						roots[i] = -1;
					}
				}
				if (roots[i] == -1)
				{
					break;
				}
			}
		}

		isUsed[index] = false;
		lastRoot--;

		for (int i = emptyIndex - 1; i >= 0; i--)
		{
			if (isUsed[i] == true)
			{
				emptyIndex = i + 1;
				break;
			}
		}

		if (!stack.full())
		{
			stack.push(index);
		}
	}

	void TreeSDF::delNode(int node)
	{

	}

	NodeSDF* TreeSDF::getNodes()
	{
		return nodes;
	}

	int* TreeSDF::getRoots()
	{
		return roots;
	}

	int TreeSDF::getIndexSize()
	{
		return emptyIndex;
	}

	int* TreeSDF::getChildrenArray()
	{
		return children;
	}
