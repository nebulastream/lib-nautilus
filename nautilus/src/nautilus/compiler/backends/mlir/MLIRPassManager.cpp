

#include "nautilus/compiler/backends/mlir/MLIRPassManager.hpp"
#include "nautilus/exceptions/NotImplementedException.hpp"
#include <mlir/Conversion/ControlFlowToLLVM/ControlFlowToLLVM.h>
#include <mlir/Conversion/FuncToLLVM/ConvertFuncToLLVMPass.h>
#include <mlir/Conversion/SCFToControlFlow/SCFToControlFlow.h>
#include <mlir/ExecutionEngine/OptUtils.h>
#include <mlir/Pass/PassManager.h>
#include <mlir/Transforms/Passes.h>

namespace nautilus::compiler::mlir {

using namespace ::mlir;

/**
 * @brief Takes a LoweringPass Enum and returns the corresponding mlir lowering pass.
 *
 * @param loweringPass: Used to get the correct mlir lowering pass.
 * @return std::unique_ptr<mlir::Pass>: MLIR lowering pass corresponding to supplied Enum.
 */
std::unique_ptr<mlir::Pass> getMLIRLoweringPass(MLIRPassManager::LoweringPass loweringPass) {
	switch (loweringPass) {
	case MLIRPassManager::LoweringPass::LLVM:
		return mlir::createConvertControlFlowToLLVMPass();
	case MLIRPassManager::LoweringPass::SCF:
		return mlir::createConvertSCFToCFPass();
	}
	throw NotImplementedException("pass is not supported");
}

/**
 * @brief Takes a OptimizationPass Enum and returns the corresponding mlir optimization pass.
 *
 * @param optimizationPass: Used to get the correct mlir optimization pass.
 * @return std::unique_ptr<mlir::Pass>: MLIR optimization pass corresponding to supplied Enum.
 */
std::unique_ptr<mlir::Pass> getMLIROptimizationPass(MLIRPassManager::OptimizationPass optimizationPass) {
	switch (optimizationPass) {
	case MLIRPassManager::OptimizationPass::Inline:
		return mlir::createInlinerPass();
	}
	throw NotImplementedException("pass is not supported");
}

int MLIRPassManager::lowerAndOptimizeMLIRModule(mlir::OwningOpRef<mlir::ModuleOp>& module,
                                                const std::vector<LoweringPass>& loweringPasses,
                                                const std::vector<OptimizationPass>& optimizationPasses) {
	mlir::PassManager passManager(module->getContext());

	// Apply optimization passes.
	if (!optimizationPasses.empty()) {
		for (auto optimizationPass : optimizationPasses) {
			passManager.addPass(getMLIROptimizationPass(optimizationPass));
		}
	} else {
		passManager.addPass(mlir::createInlinerPass());
	}
	// Apply lowering passes.
	if (!loweringPasses.empty()) {
		for (auto loweringPass : loweringPasses) {
			passManager.addPass(getMLIRLoweringPass(loweringPass));
		}
	} else {
		passManager.addPass(mlir::createConvertSCFToCFPass());
		passManager.addPass(mlir::createConvertFuncToLLVMPass());
		passManager.addPass(mlir::createConvertControlFlowToLLVMPass());
	}

	// Run passes.
	if (mlir::failed(passManager.run(*module))) {
		llvm::errs() << "MLIRPassManager::lowerAndOptimizeMLIRModule: Failed to apply passes to generated MLIR" << '\n';
		return 1;
	}
	return 0;
}
} // namespace nautilus::compiler::mlir
