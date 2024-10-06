
#include <nautilus/compiler/ir/operations/ArithmeticOperations/SubOperation.hpp>
#include <string>

namespace nautilus::compiler::ir {
SubOperation::SubOperation(OperationIdentifier identifier, Operation* leftInput, Operation* rightInput)
    : BinaryOperation(OperationType::SubOp, identifier, leftInput->getStamp(), leftInput, rightInput) {
}

bool SubOperation::classof(const Operation* Op) {
	return Op->getOperationType() == OperationType::SubOp;
}

} // namespace nautilus::compiler::ir
