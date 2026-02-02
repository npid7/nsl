#pragma once

#include <string>
#include <vector>

struct symtable_entry {
  symtable_entry(const std::string &name, int st, int ts = 0, int q = 0,
                 int d = 0) {
    Name = name;
    SymbolType = st;
    TypeSpecifier = ts;
    Qualifier = q;
    Definition = d;
  }
  symtable_entry()
      : Name(""),
        SymbolType(0),
        TypeSpecifier(0),
        Qualifier(0),
        Definition(0) {}
  std::string Name;
  int SymbolType = 0;
  int TypeSpecifier = 0;
  int Qualifier = 0;
  int Definition = 0;
};

struct symtable {
  std::vector<symtable_entry> symbols;
  std::vector<symtable> StackedTables;

  void OpenScope();
  void CloseScope();
  int GetIndex(std::string Name);
  symtable_entry *Insert(std::string Name, int Type);
  symtable_entry *Lookup(std::string Name);
  symtable_entry *FindFirstOfType(int T);

  symtable();
};