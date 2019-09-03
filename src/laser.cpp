#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LegacyPassManager.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Type.h>
#include <llvm/IR/Verifier.h>

#include <llvm/Transforms/InstCombine/InstCombine.h>
#include <llvm/Transforms/IPO.h>
#include <llvm/Transforms/IPO/AlwaysInliner.h>
#include <llvm/Transforms/Scalar.h>
#include <llvm/Transforms/Scalar/GVN.h>

#include <memory>

int
main(int argc, char** argv)
{
  llvm::LLVMContext llvm_context;
  llvm::IRBuilder<> ir_builder(llvm_context);
  auto the_module = std::make_unique<llvm::Module>("top", llvm_context);
  auto the_pm = std::make_unique<llvm::legacy::PassManager>();

  the_pm->add(llvm::createAlwaysInlinerLegacyPass());
  the_pm->add(llvm::createFunctionInliningPass());
  the_pm->add(llvm::createInstructionCombiningPass());
  the_pm->add(llvm::createReassociatePass());
  the_pm->add(llvm::createGVNPass());
  the_pm->add(llvm::createCFGSimplificationPass());

  // == puts ==

  llvm::ArrayRef<llvm::Type*> puts_args_ref({ ir_builder.getInt8Ty()->getPointerTo() });
  auto puts_type = llvm::FunctionType::get(ir_builder.getInt32Ty(),
                                           puts_args_ref,
                                           false);
  auto puts_function = the_module->getOrInsertFunction("puts", puts_type);

  // == hello ==

  // function type of hello: () -> void
  auto hello_type = llvm::FunctionType::get(llvm::Type::getVoidTy(llvm_context),
                                            false);

  // declare hello function
  auto hello_function = llvm::Function::Create(hello_type,
                                               llvm::Function::ExternalLinkage,
                                               "hello",
                                               the_module.get());

  // define hello function
  auto hello_body = llvm::BasicBlock::Create(llvm_context,
                                             "entry",
                                             hello_function);
  ir_builder.SetInsertPoint(hello_body);
  auto hello_world_str = ir_builder.CreateGlobalStringPtr("hello world.\n");
  ir_builder.CreateCall(puts_function, hello_world_str);
  ir_builder.CreateRetVoid();             // ret void

  llvm::verifyFunction(*hello_function);

  // == main ==

  // function type of main: () -> i32
  auto main_type = llvm::FunctionType::get(llvm::Type::getInt32Ty(llvm_context),
                                           false);

  // declare main function
  auto main_function = llvm::Function::Create(main_type,
                                              llvm::Function::ExternalLinkage,
                                              "main",
                                              the_module.get());

  // define main function
  auto main_body = llvm::BasicBlock::Create(llvm_context,
                                            "entry",
                                            main_function);
  ir_builder.SetInsertPoint(main_body);
  ir_builder.CreateCall(hello_function); // call hello
  auto result = ir_builder.CreateAdd(ir_builder.getInt32(11), ir_builder.getInt32(31));
  ir_builder.CreateRet(result);          // ret i32 42

  llvm::verifyFunction(*main_function);

  the_pm->run(*the_module);
  the_module->print(llvm::errs(), nullptr);

  return 0;
}
