/*
 * Copyright (c) 2008,2009, Yale Laboratory of Networked Systems
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 * 
 *     * Redistributions of source code must retain the above copyright notice,
 *       this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright notice,
 *       this list of conditions and the following disclaimer in the documentation
 *       and/or other materials provided with the distribution.
 *     * Neither the name of Yale University nor the names of its contributors may
 *       be used to endorse or promote products derived from this software without
 *       specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */


#ifndef PATRICIA_TRIE_H
#define PATRICIA_TRIE_H

#include <map>
#include <string>
#include <list>
#include <iostream>
#include <algorithm>
#include <p4p/ip_addr.h>
#include <p4p/detail/compiler.h>
#include <p4p/detail/patricia.h>

namespace p4p {
namespace detail {

const unsigned int MAX_PREFIX_BITLEN = std::max(sizeof(struct in6_addr), sizeof(struct in_addr)) * 8;

template <class T>
class p4p_common_cpp_ex_EXPORT PatriciaTrie
{
public:
	PatriciaTrie();
	virtual ~PatriciaTrie();

	bool add(const IPPrefix& prefix, const T& val);
	bool remove(const IPPrefix& prefix, const T& val);

	bool remove(const T& val);
	void clear();

	const T* lookup(const IPPrefix& address, IPPrefix* prefix = NULL) const;

	bool has_prefixes(const T& val) const;

	void get_prefixes(const T& val, IPPrefixVector& result) const;
	void enumerate(std::vector<std::pair<T, std::vector<IPPrefix> > >& result) const;

	template <class OutputIterator>
	void enumerate_values(OutputIterator out) const;

	void copy_from(const PatriciaTrie<T>& rhs);

private:
	typedef std::map<T, unsigned int> ValCache;
	typedef std::pair<T, unsigned int> ValCacheEntry;

	union Address
	{
		in_addr addr4;
		in6_addr addr6;
	};

	struct _patricia_tree_t* tree_;

	/*
	 * Avoid storing values within the nodes directly, so
	 * store them in a cache here. The nodes in the tree simply
	 * point to these elements. Also store a reference count so
	 * we can clean up as entries get removed.
	 */
	ValCache val_cache_;
};

template <class T>
PatriciaTrie<T>::PatriciaTrie()
{
	/*
	 * Allow for IPv6 addresses
	 */
	tree_ = New_Patricia(MAX_PREFIX_BITLEN);
}

template <class T>
PatriciaTrie<T>::~PatriciaTrie()
{
	Destroy_Patricia(tree_, NULL);
}

template <class T>
bool PatriciaTrie<T>::add(const IPPrefix& prefix, const T& val)
{
	prefix_t* p = New_Prefix(prefix.get_family(), prefix.get_address(), prefix.get_length());

	/*
	 * Check if it exists already
	 */
	patricia_node_t* node = patricia_search_exact(tree_, p);
	if (node)
	{
		bool result = val == *(const T*)(node->data);
		Deref_Prefix(p);
		return result;
	}

	/*
	 * Add node to the tree
	 */
	node = patricia_lookup(tree_, p);
	Deref_Prefix(p);

	/*
	 * Add val to the cache if it isn't there already.
	 */
	std::pair<typename ValCache::iterator, bool> val_res = val_cache_.insert(std::make_pair(val, 0));
	++val_res.first->second; /* Increment reference count */
	node->data = (void*)&val_res.first->first; /* Let node point to correct val */
	return true;
}

template <class T>
bool PatriciaTrie<T>::remove(const IPPrefix& prefix, const T& val)
{
	prefix_t* p = New_Prefix(prefix.get_family(), prefix.get_address(), prefix.get_length());

	/*
	 * Locate the node to remove
	 */
	patricia_node_t* node = patricia_search_exact(tree_, p);
	Deref_Prefix(p);
	if (!node)
		return false;

	/*
	 * Check that it refers to the correct val
	 */
	if (*(const T*)node->data != val)
		return false;

	/*
	 * Remove the node
	 */
	patricia_remove(tree_, node);

	/*
	 * Update val cache and remove node if needed
	 */
	typename ValCache::iterator itr = val_cache_.find(val);
	if (--itr->second == 0)
		val_cache_.erase(val);
	return true;
}

template <class T>
bool PatriciaTrie<T>::remove(const T& val)
{
	/*
	 * Get the val's iterator in our cache. We'll compare against
	 * the pointer for speed.
	 */
	typename ValCache::iterator itr = val_cache_.find(val);
	if (itr == val_cache_.end())
		return false;
	void* val_ptr = (void*)&itr->first;

	/*
	 * Gather a list of all the nodes to remove
	 */
	std::list<patricia_node_t*> removed_nodes;
	patricia_node_t *node;

	PATRICIA_WALK (tree_->head, node) {
		if (node->data == val_ptr)
			removed_nodes.push_back(node);
	} PATRICIA_WALK_END;

	/*
	 * Remove each of the nodes
	 */
	for (std::list<patricia_node_t*>::const_iterator itr = removed_nodes.begin(); itr != removed_nodes.end(); ++itr)
		patricia_remove(tree_, *itr);

	/*
	 * Remove from val cache
	 */
	val_cache_.erase(itr);
	return true;
}

template <class T>
void PatriciaTrie<T>::clear()
{
	Destroy_Patricia(tree_, NULL);
	tree_ = New_Patricia(MAX_PREFIX_BITLEN);
	val_cache_.clear();
}

template <class T>
const T* PatriciaTrie<T>::lookup(const IPPrefix& address, IPPrefix* prefix) const
{
	prefix_t* p = New_Prefix(address.get_family(), address.get_address(), address.get_length());

	patricia_node_t* node = patricia_search_best(tree_, p);
	Deref_Prefix(p);

	if (!node)
		return NULL;

	/* Return the matching prefix if non-NULL */
	if (prefix)
		*prefix = IPPrefix(node->prefix->family, &node->prefix->add, node->prefix->bitlen);

	return (const T*)node->data;
}

template <class T>
bool PatriciaTrie<T>::has_prefixes(const T& val) const
{
	return val_cache_.find(val) != val_cache_.end();
}

template <class T>
void PatriciaTrie<T>::get_prefixes(const T& val, IPPrefixVector& result) const
{
	patricia_node_t *node;

	result.clear();

	PATRICIA_WALK (tree_->head, node) {
	do {

		const T& node_val = *(const T*)node->data;
		if (node_val != val)
			continue;

		int family = node->prefix->family;
		Address addr;
		if (family == AF_INET)
			addr.addr4 = node->prefix->add.sin;
		else if (family == AF_INET6)
			addr.addr6 = node->prefix->add.sin6;
		else
			continue;
		result.push_back(IPPrefix(family, &addr, node->prefix->bitlen));
	} while (0);
	} PATRICIA_WALK_END;
}

template <class T>
void PatriciaTrie<T>::enumerate(std::vector<std::pair<T, std::vector<IPPrefix> > >& result) const
{
	/*
	 * If result is initially empty, we fill in results for all vals
	 */
	if (result.empty())
	{
		/* Pre-allocate memory needed for PIDs */
		result.reserve(val_cache_.size());

		/* Fill in the vals */
		for (typename ValCache::const_iterator itr = val_cache_.begin(); itr != val_cache_.end(); ++itr)
			result.push_back(std::make_pair(itr->first, std::vector<IPPrefix>()));
	}

	/*
	 * Assign an index in the resulting vector to each val and reserve
	 * memory for each list of prefixes
	 */
	std::map<T, unsigned int> val_to_idx;
	for (unsigned int i = 0; i < result.size(); ++i)
	{
		const T& pid = result[i].first;

		/* Add Val->Index mapping for use when we're walking the tree */
		val_to_idx[pid] = i;

		/* Determine number of entries for the val and pre-allocate memory */
		typename ValCache::const_iterator itr = val_cache_.find(pid);
		result[i].second.reserve(itr != val_cache_.end() ? itr->second : 0);
	}

	/*
	 * Walk the tree and put the prefixes in the vector for the appropriate Val
	 */
	patricia_node_t *node;
	PATRICIA_WALK (tree_->head, node) {
	do {
		const T* val = (const T*)node->data;

		/* Locate index for this val in the results; ignore if val not in result list */
		typename std::map<T, unsigned int>::const_iterator val_to_idx_itr = val_to_idx.find(*val);
		if (val_to_idx_itr == val_to_idx.end())
			continue;

		/* Add prefix to list */
		int family = node->prefix->family;
		Address addr;
		if (family == AF_INET)
			addr.addr4 = node->prefix->add.sin;
		else if (family == AF_INET6)
			addr.addr6 = node->prefix->add.sin6;
		else
			continue;
		result[val_to_idx_itr->second].second.push_back(IPPrefix(family, &addr, node->prefix->bitlen));

	} while (0);
	} PATRICIA_WALK_END;
}

template <class T>
template <class OutputIterator>
void PatriciaTrie<T>::enumerate_values(OutputIterator out) const
{
	for (typename ValCache::const_iterator itr = val_cache_.begin(); itr != val_cache_.end(); ++itr)
		*out++ = itr->first;
}

template <class T>
void PatriciaTrie<T>::copy_from(const PatriciaTrie<T>& rhs)
{
	clear();

	patricia_node_t *node;
	PATRICIA_WALK (rhs.tree_->head, node) {

		/* copy entry from our tree */
		prefix_t* prefix = New_Prefix(node->prefix->family, &node->prefix->add, node->prefix->bitlen);

		/* Add to new tree */
		patricia_node_t* ins_node = patricia_lookup(tree_, prefix);

		/* Add the val to the new cache */
		std::pair<typename ValCache::iterator, bool> val_res = val_cache_.insert(std::make_pair(*((const T*)node->data), 0));
		++val_res.first->second; /* Increment reference count */
		ins_node->data = (void*)&val_res.first->first; /* Let node point to correct val */

		Deref_Prefix(prefix);
	} PATRICIA_WALK_END;
}

prefix_t* make_prefix(const char* address, unsigned short prefix_length = USHRT_MAX);
void unmake_prefix(const prefix_t* prefix, std::string& address, unsigned int& prefix_length);

}; // namespace detail
}; // namespace p4p

#endif
