#pragma once

#include <ast.h>

struct neocode_register_file {
  int Vertex[8];
  int Constants[96];
  int Temp[16];
  int Output[8];

  int AllocOutput() {
    for (int i = 0; i < 8; ++i) {
      if (!Output[i]) {
        Output[i] = 1;
        return i;
      }
    }

    return -1;
  }

  int AllocVertex() {
    for (int i = 0; i < 8; ++i) {
      if (!Vertex[i]) {
        Vertex[i] = 1;
        return i;
      }
    }

    return -1;
  }

  int AllocTemp() {
    for (int i = 0; i < 16; ++i) {
      if (!Temp[i]) {
        Temp[i] = 1;
        return i + 0x10;
      }
    }

    return -1;
  }

  int AllocConstant() {
    for (int i = 0; i < 96; ++i) {
      if (!Constants[i]) {
        Constants[i] = 1;
        return i + 0x20;
      }
    }

    return -1;
  }

  void Free(int Register) {
    if (Register < 0x8) Vertex[Register] = 0;

    if (Register >= 0x10 && Register < 0x20) Temp[Register - 0x10] = 0;

    // if (Register >= 0x20)
    //   Constants[Register - 0x20] = 0;
  }

  void FreeAllTemp() {
    for (int i = 0; i < 16; ++i) {
      Temp[i] = 0;
    }
  }
};

struct neocode_constant {
  enum {
    FLOAT,
    INT,
    BOOLEAN,
  };

  template <typename T>
  struct vv4 {
    vv4() {}
    vv4(T x, T y, T z, T w) : X(x), Y(y), Z(z), W(w) {}
    T X, Y, Z, W;
  };

  neocode_constant(int t, vv4<int> i)
      : Type(t), Integer(i), Float(vv4<float>()), Bool(0) {}
  neocode_constant(int t, vv4<float> f)
      : Type(t), Integer(vv4<int>()), Float(f), Bool(0) {}
  neocode_constant(int t, int b)
      : Type(t), Integer(vv4<int>()), Float(vv4<float>()), Bool(b) {}
  neocode_constant(int i = 0) : Type(i) {}

  int Type = 0;
  vv4<int> Integer;
  vv4<float> Float;

  int Bool = 0;
};

struct neocode_variable {
  enum {
    INPUT_UNIFORM = -1,
    OUTPUT_POSITION = 1,
    OUTPUT_QUATERNION,
    OUTPUT_COLOR,
    OUTPUT_TEXCOORD0,
    OUTPUT_TEXCOORD0W,
    OUTPUT_TEXCOORD1,
    OUTPUT_TEXCOORD2,
    OUTPUT_UNK,
    OUTPUT_VIEW,
    OUTPUT_DUMMY,
  };

  neocode_variable(const std::string &name = "", const std::string &_tn = "",
                   int t = 0, int r = 0, int rt = 0,
                   neocode_constant c = neocode_constant(0), int s = 0) {
    Name = name;
    TypeName = _tn;
    Type = t;
    Register = r;
    RegisterType = rt;
    Const = c;
    Swizzle = s;
  }

  std::string Name;
  std::string TypeName;
  int Type;
  int Register;
  int RegisterType;
  neocode_constant Const;
  int Swizzle;
};

struct neocode_instruction {
  enum { EMPTY, MOV, MUL, RSQ, RCP, NOP, END, EX2, LG2, DP4 };

  int Type;
  neocode_variable Dst;
  neocode_variable Src1;
  neocode_variable Src2;
  std::string ExtraData;

  neocode_instruction() {
    Dst.Swizzle = 0;
    Src1.Swizzle = 0;
    Src2.Swizzle = 0;
  }
};

struct neocode_program;

struct neocode_function {
  neocode_program *Program;
  int ReturnType;
  std::string Name;
  std::vector<neocode_variable> Variables;
  std::vector<neocode_instruction> Instructions;

  neocode_function(neocode_program *P) : Program(P) {}
  neocode_variable *GetVariable(std::string Name);
};

struct neocode_program {
  std::vector<neocode_function> Functions;
  std::vector<neocode_variable> Globals;
  neocode_register_file Registers;
};

struct cg_neo {
  symtable *SymbolTable;
  void BuildStatement(neocode_function *Function, ast_node *ASTNode);
  neocode_instruction BuildInstruction(neocode_function *Function,
                                       ast_node *ASTNode);
  neocode_function BuildFunction(neocode_program *Program, ast_node *ASTNode);
};

neocode_program CGNeoBuildProgramInstance(ast_node *ASTNode, symtable *S);
void CGNeoGenerateCode(neocode_program *Program, std::ostream &os);
