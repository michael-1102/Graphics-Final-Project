
// Generated from XMLParser.g4 by ANTLR 4.13.2

#pragma once


#include "antlr4-runtime.h"
#include "XMLParserVisitor.h"


/**
 * This class provides an empty implementation of XMLParserVisitor, which can be
 * extended to create a visitor which only needs to handle a subset of the available methods.
 */
class  XMLParserBaseVisitor : public XMLParserVisitor {
public:

  virtual std::any visitDocument(XMLParser::DocumentContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitProlog(XMLParser::PrologContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitContent(XMLParser::ContentContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitElement(XMLParser::ElementContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitReference(XMLParser::ReferenceContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitAttribute(XMLParser::AttributeContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitChardata(XMLParser::ChardataContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitMisc(XMLParser::MiscContext *ctx) override {
    return visitChildren(ctx);
  }


};

