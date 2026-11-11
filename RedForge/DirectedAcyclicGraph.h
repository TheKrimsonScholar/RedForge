#pragma once

#include <vector>
#include <queue>
#include <unordered_set>
#include <unordered_map>
#include <algorithm>
#include <functional>
#include <cassert>

template<typename T>
struct DAG;
template<typename T>
struct DAGNode
{
private:
	std::unordered_set<T> inValues;
	std::unordered_set<T> outValues;

	friend struct DAG<T>;
};
template<typename T>
struct DAG
{
private:
	std::unordered_map<T, DAGNode<T>> nodes;
	std::unordered_set<T> rootValues;
	std::unordered_set<T> leafValues;

public:
	void AddNode(const T& value);
	void RemoveNode(const T& value, bool alsoRemoveIsolatedNodes = false);
	void AddConnection(const T& outValue, const T& inValue, bool addNodesIfNotPresent = true);
	void RemoveConnection(const T& outValue, const T& inValue, bool removeIsolatedNodes = false);

	// Returns true iff downstreamNode lies in the descendants of upstreamNode. Includes the case where downstreamNode == upstreamNode.
	bool IsNodeDownstreamFrom(const T& downstreamNode, const T& upstreamNode) const;
	// Returns true iff upstreamNode lies in the ancestors of downstreamNode. Includes the case where upstreamNode == downstreamNode.
	bool IsNodeUpstreamFrom(const T& upstreamNode, const T& downstreamNode) const;

	std::unordered_set<T> GetAllDescendants(const T& node) const;
	std::unordered_set<T> GetAllAncestors(const T& node) const;

	std::vector<T> GetTopologicallySortedListBFS(std::function<bool(const T&, const T&)> innerSortFunc = [](const T&, const T&) -> bool { return false; }) const;

	std::unordered_set<T> GetInValuesOfNode(const T& nodeValue) const;
	std::unordered_set<T> GetOutValuesOfNode(const T& nodeValue) const;
};

template<typename T>
inline void DAG<T>::AddNode(const T& value)
{
	// Create a new node; since it will have no incoming or outgoing edges, it is both a root node and a leaf node by default
	nodes.emplace(value, DAGNode<T>());
	rootValues.insert(value);
	leafValues.insert(value);
}
template<typename T>
inline void DAG<T>::RemoveNode(const T& value, bool alsoRemoveIsolatedNodes)
{
	if(nodes.find(value) == nodes.end())
		return;

	// Remove all connections
	std::unordered_set<T> inValues = nodes.at(value).inValues;
	std::unordered_set<T> outValues = nodes.at(value).outValues;
	for(const T& inValue : inValues)
		RemoveConnection(inValue, value, alsoRemoveIsolatedNodes);
	for(const T& outValue : outValues)
		RemoveConnection(value, outValue, alsoRemoveIsolatedNodes);

	// Remove the node
	nodes.erase(value, DAGNode<T>());
	rootValues.insert(value);
	leafValues.insert(value);
}
template<typename T>
inline void DAG<T>::AddConnection(const T& outValue, const T& inValue, bool addNodesIfNotPresent)
{
	// If we require both nodes to be already present in the graph, and either node is not present, return
	if(!addNodesIfNotPresent)
		if(nodes.find(outValue) == nodes.end() || nodes.find(inValue) == nodes.end())
			return;

	// Add the connection from both ends
	nodes[outValue].outValues.insert(inValue);
	nodes[inValue].inValues.insert(outValue);

	// If inValue was a root node (no incoming edges), it is no longer a root node after this connection is added
	if(rootValues.find(inValue) != rootValues.end())
		rootValues.erase(inValue);
	// If outValue was a leaf node (no outgoing edges), it is no longer a leaf node after this connection is added
	if(leafValues.find(outValue) != leafValues.end())
		leafValues.erase(outValue);
	
	// If inValue has no outgoing edges, it's a leaf node
	if(nodes[inValue].outValues.empty())
		leafValues.insert(inValue);
	// If outValue has no incoming edges, it's a root node
	if(nodes[outValue].inValues.empty())
		rootValues.insert(outValue);
}
template<typename T>
inline void DAG<T>::RemoveConnection(const T& outValue, const T& inValue, bool removeIsolatedNodes)
{
	// If either node doesn't exist, there's nothing to remove
	if(nodes.find(outValue) == nodes.end() || nodes.find(inValue) == nodes.end())
		return;

	// Remove both ends of the connection
	nodes[outValue].outValues.erase(inValue);
	nodes[inValue].inValues.erase(outValue);

	// If inValue has no outgoing edges, it's a leaf node
	if(nodes[inValue].outValues.empty())
		leafValues.insert(inValue);
	// If outValue has no incoming edges, it's a root node
	if(nodes[outValue].inValues.empty())
		rootValues.insert(outValue);

	if(removeIsolatedNodes)
	{
		// If either node has no incoming or outgoing edges, remove it from the graph entirely
		if(nodes[inValue].inValues.empty() && nodes[inValue].outValues.empty())
		{
			nodes.erase(inValue);
			rootValues.erase(inValue);
			leafValues.erase(inValue);
		}
		if(nodes[outValue].inValues.empty() && nodes[outValue].outValues.empty())
		{
			nodes.erase(outValue);
			rootValues.erase(outValue);
			leafValues.erase(outValue);
		}
	}
}

template<typename T>
inline bool DAG<T>::IsNodeDownstreamFrom(const T& downstreamNode, const T& upstreamNode) const
{
	// If we've reached downstreamNode, return true
	if(upstreamNode == downstreamNode)
		return true;

	// Recursively check all nodes under upstreamNode
	for(const T& outNode : nodes.at(upstreamNode).outValues)
		if(IsNodeDownstreamFrom(downstreamNode, outNode))
			return true;

	return false;
}
template<typename T>
inline bool DAG<T>::IsNodeUpstreamFrom(const T& upstreamNode, const T& downstreamNode) const
{
	// If we've reached upstreamNode, return true
	if(downstreamNode == upstreamNode)
		return true;

	// Recursively check all nodes above downstreamNode
	for(const T& inNode : nodes.at(downstreamNode).inValues)
		if(IsNodeUpstreamFrom(upstreamNode, inNode))
			return true;

	return false;
}

template<typename T>
inline std::unordered_set<T> DAG<T>::GetAllDescendants(const T& node) const
{
	std::unordered_set<T> descendants;
	// Recursively traverse all nodes under the given node
	for(const T& outNode : nodes.at(node).outValues)
	{
		descendants.insert(outNode);

		std::unordered_set<T> outNodeDescendants = GetAllDescendants(outNode);
		descendants.insert(outNodeDescendants.begin(), outNodeDescendants.end());
	}

	return descendants;
}
template<typename T>
inline std::unordered_set<T> DAG<T>::GetAllAncestors(const T& node) const
{
	std::unordered_set<T> ancestors;
	// Recursively traverse all nodes above the given node
	for(const T& inNode : nodes.at(node).inValues)
	{
		ancestors.insert(inNode);

		std::unordered_set<T> inNodeAncestors = GetAllAncestors(inNode);
		ancestors.insert(inNodeAncestors.begin(), inNodeAncestors.end());
	}

	return ancestors;
}

template<typename T>
inline std::vector<T> DAG<T>::GetTopologicallySortedListBFS(std::function<bool(const T&, const T&)> innerSortFunc) const
{
	/* Kahn's algorithm */

	std::unordered_map<T, size_t> nodeInDegrees;
	std::vector<T> sortedRootNodes(rootValues.begin(), rootValues.end());
	// The initial list of nodes is sorted using the inner sort function to ensure consistent ordering for sibling nodes in the hierarchy (if necessary)
	std::sort(sortedRootNodes.begin(), sortedRootNodes.end(), innerSortFunc);
	// The root nodes are the initial zero in-degree nodes
	std::queue<T> zeroInDegreeNodes;
	for(const T& rootNode : sortedRootNodes)
		zeroInDegreeNodes.push(rootNode);

	// Find the in-degrees of each node
	for(const std::pair<T, DAGNode<T>>& node : nodes)
		nodeInDegrees[node.first] = node.second.inValues.size();

	std::vector<T> sortedNodes;
	while(!zeroInDegreeNodes.empty())
	{
		T node = zeroInDegreeNodes.front();
		zeroInDegreeNodes.pop();

		sortedNodes.push_back(node);
		std::vector<T> sortedOutNodes(nodes.at(node).outValues.begin(), nodes.at(node).outValues.end());
		// The list of out nodes is sorted using the inner sort function to ensure consistent ordering for sibling nodes in the hierarchy (if necessary)
		std::sort(sortedOutNodes.begin(), sortedOutNodes.end(), innerSortFunc);
		for(const T& outNode : sortedOutNodes)
		{
			nodeInDegrees[outNode]--;
			if(nodeInDegrees[outNode] == 0)
				zeroInDegreeNodes.push(outNode);
		}
	}

	assert(sortedNodes.size() == nodes.size() && "Graph cannot be topologically flattened because it contains a cycle.");

	return sortedNodes;
}

template<typename T>
inline std::unordered_set<T> DAG<T>::GetInValuesOfNode(const T& nodeValue) const
{
	return nodes.at(nodeValue).inValues;
}
template<typename T>
inline std::unordered_set<T> DAG<T>::GetOutValuesOfNode(const T& nodeValue) const
{
	return nodes.at(nodeValue).outValues;
}