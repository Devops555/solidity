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
 * List of experimental features.
 */

#pragma once

#include <map>

namespace dev
{
namespace solidity
{

enum class ExperimentalFeature
{
	Test,
	TestOnlyAnalysis
};

static const std::map<ExperimentalFeature, bool> ExperimentalFeatureOnlyAnalysis = {
	{ ExperimentalFeature::TestOnlyAnalysis, true },
};

static const std::map<std::string, ExperimentalFeature> ExperimentalFeatureNames = {
	{ "__test", ExperimentalFeature::Test },
	{ "__testOnlyAnalysis", ExperimentalFeature::TestOnlyAnalysis },
};

}
}
