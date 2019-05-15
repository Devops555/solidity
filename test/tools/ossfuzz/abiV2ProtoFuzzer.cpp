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

#include <test/tools/ossfuzz/abiV2FuzzerCommon.h>
#include <test/tools/ossfuzz/protoToAbiV2.h>
#include <src/libfuzzer/libfuzzer_macro.h>
#include <fstream>

using namespace dev::test::abiv2fuzzer;
using namespace dev;
using namespace std;

DEFINE_PROTO_FUZZER(Contract const& _input)
{
	string contract_source = ProtoConverter{}.contractToString(_input);

	if (const char* dump_path = getenv("PROTO_FUZZER_DUMP_PATH"))
	{
		// With libFuzzer binary run this to generate the solidity source file x.sol from a proto input:
		// PROTO_FUZZER_DUMP_PATH=x.sol ./a.out proto-input
		ofstream of(dump_path);
		of << contract_source;
	}

	// Raw runtime byte code generated by solidity
	dev::bytes byteCode;
	std::string hexEncodedInput;

	try
	{
		// Compile contract generated by the proto fuzzer
		SolidityCompilationFramework solCompilationFramework;
		std::string contractName = ":Factory";
		byteCode = solCompilationFramework.compileContract(contract_source, contractName);
		Json::Value methodIdentifiers = solCompilationFramework.getMethodIdentifiers();
		// We always call the function test() that is defined in proto converter template
		hexEncodedInput = methodIdentifiers["test()"].asString();
	}
	catch (...)
	{
		cout << contract_source << endl;
		throw;
	}
	// TODO: Call evmone wrapper here
	return;
}