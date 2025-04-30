
// Generated from XMLParser.g4 by ANTLR 4.13.2

#pragma once


#include "antlr4-runtime.h"
#include "XMLParser.h"



/**
 * This class defines an abstract visitor for a parse tree
 * produced by XMLParser.
 */
class  XMLParserVisitor : public antlr4::tree::AbstractParseTreeVisitor {
public:

  /**
   * Visit parse trees produced by XMLParser.
   */
    virtual std::any visitDocument(XMLParser::DocumentContext *context) = 0;

    virtual std::any visitProlog(XMLParser::PrologContext *context) = 0;

    virtual std::any visitContent(XMLParser::ContentContext *context) = 0;

    virtual std::any visitElement(XMLParser::ElementContext *context) = 0;

    virtual std::any visitReference(XMLParser::ReferenceContext *context) = 0;

    virtual std::any visitAttribute(XMLParser::AttributeContext *context) = 0;

    virtual std::any visitChardata(XMLParser::ChardataContext *context) = 0;

    virtual std::any visitMisc(XMLParser::MiscContext *context) = 0;


};

