
#include "nautilus/compiler/ir/operations/ArithmeticOperations/DivOperation.hpp"
#include <string>

namespace nautilus::compiler::ir {
DivOperation::DivOperation(OperationIdentifier identifier, Operation* leftInput, Operation* rightInput) : BinaryOperation(OperationType::DivOp, identifier, leftInput->getStamp(), leftInput, rightInput) {
}

bool DivOperation::classof(const Operation* Op) {
	return Op->getOperationType() == OperationType::DivOp;
}

} // namespace nautilus::compiler::ir
