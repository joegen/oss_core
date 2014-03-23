#include <assert.h>
#include <stdint.h>

#include <stdexcept>

#include <OpenP2P/FolderSync/Block.hpp>
#include <OpenP2P/FolderSync/BlockId.hpp>
#include <OpenP2P/FolderSync/BlockPath.hpp>
#include <OpenP2P/FolderSync/BlockReader.hpp>
#include <OpenP2P/FolderSync/BlockWriter.hpp>
#include <OpenP2P/FolderSync/Constants.hpp>
#include <OpenP2P/FolderSync/NodeBlockStore.hpp>

namespace OpenP2P {
	
	namespace FolderSync {
		
		NodeBlockStore::NodeBlockStore(Database& database, const BlockId& initialRootId)
			: database_(database), rootId_(initialRootId) { }
		
		NodeBlockStore::~NodeBlockStore() { }
		
		const BlockId& NodeBlockStore::rootId() const {
			return rootId_;
		}
		
		Block NodeBlockStore::getRoot() const {
			return database_.loadBlock(rootId_);
		}
		
		void NodeBlockStore::setRoot(Block block) {
			rootId_ = BlockId::Generate(block);
			return database_.storeBlock(rootId_, std::move(block));
		}
		
		Block NodeBlockStore::getBlock(const BlockPath& path, const Block& parentBlock) const {
			BlockReader reader(parentBlock, NodeBlockIdOffset(path));
			const auto blockId = BlockId::FromReader(reader);
			
			return database_.loadBlock(blockId);
		}
		
		void NodeBlockStore::setBlock(const BlockPath& path, Block& parentBlock, Block block) {
			const auto blockId = BlockId::Generate(block);
			database_.storeBlock(blockId, std::move(block));
			
			BlockWriter writer(parentBlock, NodeBlockIdOffset(path));
			blockId.writeTo(writer);
		}
		
	}
	
}

