#pragma once

#include "nautilus/compiler/ir/IRGraph.hpp"
#include "nautilus/compiler/ir/blocks/BasicBlock.hpp"
#include <memory>
#include <unordered_set>

namespace nautilus::compiler::ir {

/**
 * @brief Generates a string representation of the IR object
 */
class IRDumpHandler {

public:
	virtual ~IRDumpHandler();

	static std::shared_ptr<IRDumpHandler> create(std::ostream& out);
	static std::shared_ptr<IRDumpHandler> create(std::ostream& out, const std::string& fileName);

	explicit IRDumpHandler(std::ostream& out, const std::string& fileName);

	/**
	 * @brief Dump the NESIR of the funcOp into the 'out' stringstream.
	 * @param funcOp: FunctionOperation that exists on the top level of a NESIR module.
	 */
	void dump(FunctionOperation* funcOp);

private:
	std::ostream& out;
	// We keep track of visited blocks to avoid multi or infinite dumping.
	std::unordered_set<std::string> visitedBlocks;
	size_t currentLine;
	std::string fileName;

	/**
	 * @brief Traverses the NESIR to find a BB that is on the same or higher 'blockScopeLevel' compared to the initial
	 * 'basicBlock'. Note: There is always a 'next block', since we always have a return block at the very end of a
	 * function.
	 * @param basicBlock: Initially the block that we want to find the next BB for. Replaced while recursively
	 * traversing NESIR.
	 * @param blockScopeLevel: The scopeLevel of the initial BB that we are searching the next same/higher level BB for.
	 * @return IR::BasicBlockPtr: SharedPtr to the next block that resides on the same or on a higher level.
	 */
	const BasicBlock* getNextLowerOrEqualLevelBasicBlock(const BasicBlock* basicBlock, int blockScopeLevel);

	/**
	 * @brief Handle dumping terminator operations(LoopOp, BranchOp, IfOp, ReturnOp) to the 'out' stringstream.
	 *
	 * @param terminatorOp: Terminator operation that we append to the 'out' stringstream.
	 * @param scopeLevel: scopeLevel of the BasicBlock that is terminated by the terminator operation.
	 */
	void dumpHelper(Operation* terminatorOp, int32_t scopeLevel);

	/**
	 * @brief Handle dumping BasicBlocks to the 'out' stringstream. Print all operations, then handle the terminatorOp.
	 *
	 * @param basicBlock: The basicBlock that is dumped to the 'out' stringstream.
	 */
	void dumpHelper(const BasicBlock* basicBlock);
};

} // namespace nautilus::compiler::ir
