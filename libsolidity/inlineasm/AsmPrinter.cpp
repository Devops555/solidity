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
 * @author Christian <c@ethdev.com>
 * @date 2017
 * Converts a parsed assembly into its textual form.
 */

#include <libsolidity/inlineasm/AsmPrinter.h>
#include <libsolidity/inlineasm/AsmData.h>
#include <libsolidity/interface/Exceptions.h>

#include <libdevcore/CommonData.h>

#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/replace.hpp>
#include <boost/range/adaptor/transformed.hpp>

#include <memory>
#include <functional>

using namespace std;
using namespace dev;
using namespace dev::solidity;
using namespace dev::solidity::assembly;

//@TODO source locations

string AsmPrinter::operator()(assembly::Instruction const& _instruction)
{
	solAssert(!m_yul, "");
	solAssert(isValidInstruction(_instruction.instruction), "Invalid instruction");
	return boost::to_lower_copy(instructionInfo(_instruction.instruction).name);
}

string AsmPrinter::operator()(assembly::Literal const& _literal)
{
	switch (_literal.kind)
	{
	case LiteralKind::Number:
		solAssert(isValidDecimal(_literal.value) || isValidHex(_literal.value), "Invalid number literal");
		return _literal.value + appendTypeName(_literal.type);
	case LiteralKind::Boolean:
		solAssert(_literal.value == "true" || _literal.value == "false", "Invalid bool literal.");
		return ((_literal.value == "true") ? "true" : "false") + appendTypeName(_literal.type);
	case LiteralKind::String:
		break;
	}

	string out;
	for (char c: _literal.value)
		if (c == '\\')
			out += "\\\\";
		else if (c == '"')
			out += "\\\"";
		else if (c == '\b')
			out += "\\b";
		else if (c == '\f')
			out += "\\f";
		else if (c == '\n')
			out += "\\n";
		else if (c == '\r')
			out += "\\r";
		else if (c == '\t')
			out += "\\t";
		else if (c == '\v')
			out += "\\v";
		else if (!isprint(c, locale::classic()))
		{
			ostringstream o;
			o << std::hex << setfill('0') << setw(2) << (unsigned)(unsigned char)(c);
			out += "\\x" + o.str();
		}
		else
			out += c;
	return "\"" + out + "\"" + appendTypeName(_literal.type);
}

string AsmPrinter::operator()(assembly::Identifier const& _identifier)
{
	solAssert(!_identifier.name.empty(), "Invalid identifier.");
	return _identifier.name;
}

string AsmPrinter::operator()(assembly::FunctionalInstruction const& _functionalInstruction)
{
	solAssert(!m_yul, "");
	solAssert(isValidInstruction(_functionalInstruction.instruction), "Invalid instruction");
	return
		boost::to_lower_copy(instructionInfo(_functionalInstruction.instruction).name) +
		"(" +
		boost::algorithm::join(
			_functionalInstruction.arguments | boost::adaptors::transformed(boost::apply_visitor(*this)),
			", ") +
		")";
}

string AsmPrinter::operator()(ExpressionStatement const& _statement)
{
	return boost::apply_visitor(*this, _statement.expression);
}

string AsmPrinter::operator()(assembly::Label const& _label)
{
	solAssert(!m_yul, "");
	solAssert(!_label.name.empty(), "Invalid label.");
	return _label.name + ":";
}

string AsmPrinter::operator()(assembly::StackAssignment const& _assignment)
{
	solAssert(!m_yul, "");
	solAssert(!_assignment.variableName.name.empty(), "Invalid variable name.");
	return "=: " + (*this)(_assignment.variableName);
}

string AsmPrinter::operator()(assembly::Assignment const& _assignment)
{
	solAssert(_assignment.variableNames.size() >= 1, "");
	string variables = (*this)(_assignment.variableNames.front());
	for (size_t i = 1; i < _assignment.variableNames.size(); ++i)
		variables += ", " + (*this)(_assignment.variableNames[i]);
	return variables + " := " + boost::apply_visitor(*this, *_assignment.value);
}

string AsmPrinter::operator()(assembly::VariableDeclaration const& _variableDeclaration)
{
	string out = "let ";
	out += boost::algorithm::join(
		_variableDeclaration.variables | boost::adaptors::transformed(
			[this](TypedName argument) { return formatTypedName(argument); }
		),
		", "
	);
	if (_variableDeclaration.value)
	{
		out += " := ";
		out += boost::apply_visitor(*this, *_variableDeclaration.value);
	}
	return out;
}

string AsmPrinter::operator()(assembly::FunctionDefinition const& _functionDefinition)
{
	solAssert(!_functionDefinition.name.empty(), "Invalid function name.");
	string out = "function " + _functionDefinition.name + "(";
	out += boost::algorithm::join(
		_functionDefinition.parameters | boost::adaptors::transformed(
			[this](TypedName argument) { return formatTypedName(argument); }
		),
		", "
	);
	out += ")";
	if (!_functionDefinition.returnVariables.empty())
	{
		out += " -> ";
		out += boost::algorithm::join(
			_functionDefinition.returnVariables | boost::adaptors::transformed(
				[this](TypedName argument) { return formatTypedName(argument); }
			),
			", "
		);
	}

	return out + "\n" + (*this)(_functionDefinition.body);
}

string AsmPrinter::operator()(assembly::FunctionCall const& _functionCall)
{
	return
		(*this)(_functionCall.functionName) + "(" +
		boost::algorithm::join(
			_functionCall.arguments | boost::adaptors::transformed(boost::apply_visitor(*this)),
			", " ) +
		")";
}

string AsmPrinter::operator()(If const& _if)
{
	solAssert(_if.condition, "Invalid if condition.");
	return "if " + boost::apply_visitor(*this, *_if.condition) + "\n" + (*this)(_if.body);
}

string AsmPrinter::operator()(Switch const& _switch)
{
	solAssert(_switch.expression, "Invalid expression pointer.");
	string out = "switch " + boost::apply_visitor(*this, *_switch.expression);
	for (auto const& _case: _switch.cases)
	{
		if (!_case.value)
			out += "\ndefault ";
		else
			out += "\ncase " + (*this)(*_case.value) + " ";
		out += (*this)(_case.body);
	}
	return out;
}

string AsmPrinter::operator()(assembly::ForLoop const& _forLoop)
{
	solAssert(_forLoop.condition, "Invalid for loop condition.");
	string out = "for ";
	out += (*this)(_forLoop.pre);
	out += "\n";
	out += boost::apply_visitor(*this, *_forLoop.condition);
	out += "\n";
	out += (*this)(_forLoop.post);
	out += "\n";
	out += (*this)(_forLoop.body);
	return out;
}

string AsmPrinter::operator()(Block const& _block)
{
	if (_block.statements.empty())
		return "{\n}";
	string body = boost::algorithm::join(
		_block.statements | boost::adaptors::transformed(boost::apply_visitor(*this)),
		"\n"
	);
	boost::replace_all(body, "\n", "\n    ");
	return "{\n    " + body + "\n}";
}

string AsmPrinter::formatTypedName(TypedName _variable) const
{
	solAssert(!_variable.name.empty(), "Invalid variable name.");
	return _variable.name + appendTypeName(_variable.type);
}

string AsmPrinter::appendTypeName(std::string const& _type) const
{
	if (m_yul)
		return ":" + _type;
	return "";
}
