#pragma once

#include "../Common.h"
#include "AbstractTheory.h"
#include "ParserState.h"

namespace smtrat {
namespace parser {

struct ParserState;

/**
 * Implements the core theory of the booleans.
 */
struct CoreTheory: public AbstractTheory {
	typedef boost::variant<carl::FormulaType> OperatorType;
	
	static void addSimpleSorts(qi::symbols<char, carl::Sort>& sorts);
	static void addConstants(qi::symbols<char, types::ConstType>& constants);
	
	static bool convertTerm(const types::TermType& term, FormulaT& result);
	
	static bool convertArguments(const std::vector<types::TermType>& arguments, std::vector<FormulaT>& result, TheoryError& errors);
	
	CoreTheory(ParserState* state);
	
	bool declareVariable(const std::string& name, const carl::Sort& sort, types::VariableType& result, TheoryError& errors);

	bool handleITE(const FormulaT& ifterm, const types::TermType& thenterm, const types::TermType& elseterm, types::TermType& result, TheoryError& errors);
	bool handleDistinct(const std::vector<types::TermType>& arguments, types::TermType& result, TheoryError& errors);
	
	bool functionCall(const Identifier& identifier, const std::vector<types::TermType>& arguments, types::TermType& result, TheoryError& errors);
};
	
}
}
