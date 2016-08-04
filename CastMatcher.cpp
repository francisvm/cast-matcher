#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "clang/ASTMatchers/ASTMatchers.h"
#include "clang/Basic/Diagnostic.h"
#include "clang/Frontend/FrontendActions.h"
#include "clang/Tooling/CommonOptionsParser.h"
#include "clang/Tooling/Tooling.h"
#include "llvm/Support/CommandLine.h"

using namespace clang;
using namespace clang::ast_matchers;
using namespace clang::tooling;

class CastAlert : public MatchFinder::MatchCallback {
public:
  virtual void run(const MatchFinder::MatchResult &Result) {
    if (const CStyleCastExpr *CE =
            Result.Nodes.getNodeAs<clang::CStyleCastExpr>("cast")) {

      auto &SourceManager = *Result.SourceManager;

      // Report it as an error.
      auto &DiagEngine = SourceManager.getDiagnostics();
      auto DiagID = DiagEngine.getDiagnosticIDs()->getCustomDiagID(
          DiagnosticIDs::Error, "explicit cast detected");
      // Add a removal hint.
      DiagEngine.Report(CE->getLocStart(), DiagID)
          << FixItHint::CreateRemoval({CE->getLocStart(), CE->getLocEnd()});
    }
  }
};

static llvm::cl::OptionCategory CastMatcherCategory("cast-matcher options");
static llvm::cl::extrahelp CommonHelp(CommonOptionsParser::HelpMessage);
static llvm::cl::extrahelp MoreHelp("\nThis tool helps the detection of explicit"
                              "C-style casts.");

int main(int argc, const char **argv) {
  CommonOptionsParser OptionsParser(argc, argv, CastMatcherCategory);
  CastAlert Alert;
  ClangTool Tool(OptionsParser.getCompilations(),
                 OptionsParser.getSourcePathList());

  MatchFinder Finder;

  // Match all explicit casts in the main file (exclude system headers).
  Finder.addMatcher(
      cStyleCastExpr(unless(isExpansionInSystemHeader())).bind("cast"), &Alert);

  return Tool.run(newFrontendActionFactory(&Finder).get());
}
