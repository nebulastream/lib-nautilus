#pragma once

#include "nautilus/compiler/ir/blocks/BasicBlock.hpp"
#include "nautilus/compiler/ir/blocks/BasicBlockInvocation.hpp"
#include "nautilus/compiler/ir/operations/LogicalOperations/CompareOperation.hpp"

namespace nautilus::compiler::ir {
class IfOperation : public Operation {
public:
	IfOperation(Operation* booleanValue);

	~IfOperation() override = default;

	Operation* getValue();

	BasicBlockPtr getMergeBlock();

	Operation* getBooleanValue();

	void setBooleanValue(Operation* newBooleanValue);

	void setMergeBlock(BasicBlockPtr mergeBlock);

	BasicBlockInvocation& getTrueBlockInvocation();

	BasicBlockInvocation& getFalseBlockInvocation();

	void setTrueBlockInvocation(BasicBlock* trueBlockInvocation);

	void setFalseBlockInvocation(BasicBlock* falseBlockInvocation);

	bool hasFalseCase();

	std::string toString() override;

private:
	BasicBlockInvocation trueBlockInvocation;
	BasicBlockInvocation falseBlockInvocation;
	std::weak_ptr<BasicBlock> mergeBlock;
};
} // namespace nautilus::compiler::ir
