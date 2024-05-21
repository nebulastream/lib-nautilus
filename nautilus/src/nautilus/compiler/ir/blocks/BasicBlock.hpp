
#pragma once

#include "nautilus/compiler/ir/blocks/BasicBlockArgument.hpp"
#include "nautilus/compiler/ir/operations/Operation.hpp"
#include <memory>
#include <vector>

namespace nautilus::compiler::ir {

class BasicBlock {
public:
	/**
	 * @brief BasicBlock used for control flow in NES IR
	 * @param Operations: A list of Operations that are executed in the BasicBlock.
	 * @param nextBlocks : The BasicBlock that is next in the control flow of the execution.
	 */
	explicit BasicBlock(const std::string& identifier, int32_t scopeLevel,
	                    std::vector<std::unique_ptr<Operation>>& operations,
	                    std::vector<std::unique_ptr<BasicBlockArgument>>& arguments);

	virtual ~BasicBlock();

	[[nodiscard]] const std::string& getIdentifier() const;

	void setIdentifier(const std::string& identifier);

	[[nodiscard]] uint32_t getScopeLevel() const;

	void setScopeLevel(uint32_t scopeLevel);

	/**
	 * @brief Get the number of edges that lead back from the loop body to the loop header.
	 */
	[[nodiscard]] uint32_t getNumLoopBackEdges();

	/**
	 * @brief Increment counter for edges that lead back from the loop body to the loop header.
	 */
	void incrementNumLoopBackEdge();

	/**
	 * @brief Check if the counter for edges that lead back from the loop body to the loop header is > 0.
	 */
	[[nodiscard]] bool isLoopHeaderBlock();

	[[nodiscard]] const std::vector<std::unique_ptr<Operation>>& getOperations() const;

	[[nodiscard]] Operation* getOperationAt(size_t index);

	[[nodiscard]] Operation* getTerminatorOp();

	[[nodiscard]] const std::vector<std::unique_ptr<BasicBlockArgument>>& getArguments() const;

	template <typename T, typename... Args>
	T* addOperation(Args&&... operation) {
		auto op = std::make_unique<T>(std::forward<Args>(operation)...);
		auto& ref = this->operations.emplace_back(std::move(op));
		return as<T>(ref);
	}

	BasicBlock* addOperation(std::unique_ptr<Operation> operation);

	BasicBlock* addNextBlock(BasicBlock* nextBlock);

	void addNextBlock(BasicBlock* nextBlock, const std::vector<Operation*>& inputArguments);

	BasicBlock* addTrueBlock(BasicBlock* thenBlock);

	BasicBlock* addFalseBlock(BasicBlock* elseBlock);

	void replaceOperation(size_t operationIndex, Operation* operation);

	void removeOperation(Operation* operation);

	void removeArgument(std::shared_ptr<BasicBlockArgument> argument);

	void addOperationBefore(Operation* before, std::unique_ptr<Operation>& operation);

	void addPredecessor(BasicBlock* predecessor);

	std::vector<BasicBlock*>& getPredecessors();

	uint64_t getIndexOfArgument(Operation* arg);

	// void popOperation();
	void replaceTerminatorOperation(Operation* newTerminatorOperation);

	[[nodiscard]] std::pair<const BasicBlock*, const BasicBlock*> getNextBlocks();

private:
	std::string identifier;
	uint32_t scopeLevel;
	uint32_t numLoopBackEdges;
	std::vector<std::unique_ptr<Operation>> operations;
	std::vector<std::unique_ptr<BasicBlockArgument>> arguments;
	std::vector<BasicBlock*> predecessors;
};

using BasicBlockPtr = std::shared_ptr<BasicBlock>;

} // namespace nautilus::compiler::ir
