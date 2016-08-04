#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "clang/ASTMatchers/ASTMatchers.h"
#include "clang/Frontend/FrontendActions.h"
#include "clang/Tooling/CommonOptionsParser.h"
#include "clang/Tooling/Tooling.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/raw_ostream.h"

using namespace llvm;
using namespace clang;
using namespace clang::ast_matchers;
using namespace clang::tooling;

class CastAlert : public MatchFinder::MatchCallback {
public:
  virtual void run(const MatchFinder::MatchResult &Result) {
    if (const CStyleCastExpr *CE =
            Result.Nodes.getNodeAs<clang::CStyleCastExpr>("cast"))
      Found.insert(CE);
  }

  SmallPtrSet<const CStyleCastExpr *, 4> Found;
};

static llvm::cl::OptionCategory CastMatcherCategory("cast-matcher options");
static cl::extrahelp CommonHelp(CommonOptionsParser::HelpMessage);
static cl::extrahelp MoreHelp("\nThis tool helps the detection of explicit"
                              "C-style casts.");

int main(int argc, const char **argv) {
  CommonOptionsParser OptionsParser(argc, argv, CastMatcherCategory);
  ClangTool Tool(OptionsParser.getCompilations(),
                 OptionsParser.getSourcePathList());

  CastAlert Alert;
  MatchFinder Finder;
  Finder.addMatcher(
      cStyleCastExpr(unless(isExpansionInSystemHeader())).bind("cast"), &Alert);

  auto ret = Tool.run(newFrontendActionFactory(&Finder).get());

  PrintingPolicy policy{LangOptions{}};
  for (auto *CE : Alert.Found) {
    llvm::outs() << "Cast found: \n";
    CE->printPretty(llvm::outs(), nullptr, policy);
    llvm::outs() << '\n';
  }

  if (Alert.Found.size())
    return 1;
  return ret;
}
