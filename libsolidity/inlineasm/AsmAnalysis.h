/*
	This file is part of solidity.

	solidity is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	solidity is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with solidity.  If not, see <http://www.gnu.org/licenses/>.
*/
/**
 * Analysis part of inline assembly.
 */

#pragma once

#include <libsolidity/interface/Exceptions.h>

#include <libjulia/backends/evm/AbstractAssembly.h>

#include <boost/variant.hpp>

#include <functional>
#include <memory>

namespace dev
{
namespace solidity
{
class ErrorReporter;
namespace assembly
{

struct Literal;
struct Block;
struct Label;
struct FunctionalInstruction;
struct Assignment;
struct VariableDeclaration;
struct Instruction;
struct Identifier;
struct StackAssignment;
struct FunctionDefinition;
struct FunctionCall;
struct Switch;
using Statement = boost::variant<Instruction, Literal, Label, StackAssignment, Identifier, Assignment, FunctionCall, FunctionalInstruction, VariableDeclaration, FunctionDefinition, Switch, Block>;


struct Scope;

struct AsmAnalysisInfo;

/**
 * Performs the full analysis stage, calls the ScopeFiller internally, then resolves
 * references and performs other checks.
 * If all these checks pass, code generation should not throw errors.
 */
class AsmAnalyzer: public boost::static_visitor<bool>
{
public:
	explicit AsmAnalyzer(
		AsmAnalysisInfo& _analysisInfo,
		ErrorReporter& _errorReporter,
		bool _julia = false,
		julia::ExternalIdentifierAccess::Resolver const& _resolver = julia::ExternalIdentifierAccess::Resolver()
	): m_resolver(_resolver), m_info(_analysisInfo), m_errorReporter(_errorReporter), m_julia(_julia) {}

	bool analyze(assembly::Block const& _block);

	bool operator()(assembly::Instruction const&);
	bool operator()(assembly::Literal const& _literal);
	bool operator()(assembly::Identifier const&);
	bool operator()(assembly::FunctionalInstruction const& _functionalInstruction);
	bool operator()(assembly::Label const& _label);
	bool operator()(assembly::StackAssignment const&);
	bool operator()(assembly::Assignment const& _assignment);
	bool operator()(assembly::VariableDeclaration const& _variableDeclaration);
	bool operator()(assembly::FunctionDefinition const& _functionDefinition);
	bool operator()(assembly::FunctionCall const& _functionCall);
	bool operator()(assembly::Switch const& _switch);
	bool operator()(assembly::Block const& _block);

private:
	/// Visits the statement and expects it to deposit one item onto the stack.
	bool expectExpression(Statement const& _statement);
	bool expectDeposit(int _deposit, int _oldHeight, SourceLocation const& _location);

	/// Verifies that a variable to be assigned to exists and has the same size
	/// as the value, @a _valueSize, unless that is equal to -1.
	bool checkAssignment(assembly::Identifier const& _assignment, size_t _valueSize = size_t(-1));

	Scope& scope(assembly::Block const* _block);
	void expectValidType(std::string const& type, SourceLocation const& _location);

	int m_stackHeight = 0;
	julia::ExternalIdentifierAccess::Resolver m_resolver;
	Scope* m_currentScope = nullptr;
	AsmAnalysisInfo& m_info;
	ErrorReporter& m_errorReporter;
	bool m_julia = false;
};

}
}
}
