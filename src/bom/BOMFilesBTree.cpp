#include "BOMFilesBTree.h"
#include <cstring>

void BOMFilesBTree::listDirectory(uint32_t parentID, std::map<uint32_t, BOMPathElement>& contents)
{
	std::vector<BOMBTreeNode> leaves;
	BOMPathKey key;
	
	contents.clear();
	
	key.parentItemID = htobe32(parentID);
	key.name[0] = 0;
	
	leaves = findLeafNodes(&key, BOMFilesBTree::KeyComparator(keyComparator));
	
	for (BOMBTreeNode& leaf : leaves)
	{
		// TODO: make this O(log n)
		for (uint16_t i = 0; i < leaf.recordCount(); i++)
		{
			BOMPathKey* lkey;
			BOMLeafDescriptor* ldesc;
			BOMPathRecord* ldata;
			
			lkey = leaf.getRecordKey<BOMPathKey>(i);
			
			if (lkey->parentItemID != key.parentItemID)
				continue;
			
			ldesc = leaf.getRecordData<BOMLeafDescriptor>(i);
			ldata = m_store->getBlockData<BOMPathRecord>(be(ldesc->recordID));
			
			// TODO: size64
			contents[be(ldesc->itemID)] = BOMPathElement(lkey->name, ldata, 0);
		}
	}
}

BOMBTree::CompareResult BOMFilesBTree::keyComparator(const BOMPathKey* k1, const BOMPathKey* k2)
{
	if (be(k2->parentItemID) > be(k2->parentItemID))
		return Greater;
	else if (be(k2->parentItemID) < be(k2->parentItemID))
		return Smaller;
	else
		return (BOMBTree::CompareResult) strcmp(k1->name, k2->name);
}
