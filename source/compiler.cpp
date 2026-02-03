#include <nsl.h>

#include <cstdlib>
#include <cstring>
#include <fstream>
#include <nsl/ast.hpp>
#include <nsl/codegen_shbin.hpp>
#include <nsl/parser.hpp>

static void (*UserErrorHandler)(const char *Msg) = nullptr;

static void ErrorCallback(const std::string &ErrMsg,
                          const std::string &OffendingLine, int LineNumber,
                          int LineOffset) {
  std::string Msg = "error:" + std::to_string(LineNumber) + ":" +
                    std::to_string(LineOffset) + ErrMsg + "\n" + OffendingLine +
                    "\n";
  if (UserErrorHandler) UserErrorHandler(Msg.c_str());
}

static char *SlurpFile(const char *FilePath, long *FileSize) {
  std::ifstream is(FilePath);
  if (!is) return nullptr;
  is.seekg(0, std::ios::end);
  long Length = is.tellg();
  is.seekg(0, std::ios::beg);
  char *Buffer = new char[Length + 1];
  memset(Buffer, 0, Length);
  is.read(Buffer, Length);
  is.close();

  if (FileSize) *FileSize = Length;
  return Buffer;
}

extern "C" {

void NslSetErrorHandler(void (*ErrorFunc)(const char *)) {
  UserErrorHandler = ErrorFunc;
}

char *NslCompileShaderSource(const char *Src, int *BinSize) {
  symtable SymbolTable;
  lexer_state Lexer;
  LexerInit(&Lexer, (char *)Src, (char *)Src + strlen(Src) + 1, &SymbolTable);
  parser Parser = parser(Lexer);
  Parser.ErrorFunc = ErrorCallback;
  parse_node RootNode = Parser.ParseTranslationUnit();

  ast_node ASTRoot = ast::BuildTranslationUnit(RootNode, &SymbolTable);
  neocode_program Program = CGNeoBuildProgramInstance(&ASTRoot, &SymbolTable);
  std::stringstream ss;
  CGShbinGenerateCode(&Program, ss);
  char *Shbin = (char *)malloc(ss.str().length() + 1);
  memcpy(Shbin, ss.str().c_str(), ss.str().length());
  *BinSize = ss.str().length();
  return Shbin;
}

char *NslCompileShaderFile(const char *Path, int *BinSize) {
  long Size;
  char *Src = SlurpFile(Path, &Size);
  return NslCompileShaderSource(Src, BinSize);
}
}
