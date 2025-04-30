
// Generated from XMLLexer.g4 by ANTLR 4.13.2

#pragma once


#include "antlr4-runtime.h"




class  XMLLexer : public antlr4::Lexer {
public:
  enum {
    COMMENT = 1, CDATA = 2, DTD = 3, EntityRef = 4, CharRef = 5, SEA_WS = 6, 
    OPEN = 7, XMLDeclOpen = 8, TEXT = 9, CLOSE = 10, SPECIAL_CLOSE = 11, 
    SLASH_CLOSE = 12, SLASH = 13, EQUALS = 14, STRING = 15, Name = 16, S = 17, 
    PI = 18
  };

  enum {
    INSIDE = 1, PROC_INSTR = 2
  };

  explicit XMLLexer(antlr4::CharStream *input);

  ~XMLLexer() override;


  std::string getGrammarFileName() const override;

  const std::vector<std::string>& getRuleNames() const override;

  const std::vector<std::string>& getChannelNames() const override;

  const std::vector<std::string>& getModeNames() const override;

  const antlr4::dfa::Vocabulary& getVocabulary() const override;

  antlr4::atn::SerializedATNView getSerializedATN() const override;

  const antlr4::atn::ATN& getATN() const override;

  // By default the static state used to implement the lexer is lazily initialized during the first
  // call to the constructor. You can call this function if you wish to initialize the static state
  // ahead of time.
  static void initialize();

private:

  // Individual action functions triggered by action() above.

  // Individual semantic predicate functions triggered by sempred() above.

};

