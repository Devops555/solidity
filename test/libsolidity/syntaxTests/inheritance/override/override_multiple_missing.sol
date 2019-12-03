abstract contract A {
	int public testvar;
	function foo() internal virtual returns (uint256);
	function test(uint8 _a) virtual internal returns (uint256);
}
abstract contract B {
	function foo() internal virtual returns (uint256);
	function test() internal virtual returns (uint256);
}
abstract contract C {
	function foo() internal virtual returns (uint256);
}
abstract contract D {
	function foo() internal virtual returns (uint256);
	function test() internal virtual returns (uint256);
}
abstract contract X is A, B, C, D {
	int public override testvar;
	function test() internal override(B, D, C) virtual returns (uint256);
	function foo() internal override(A, C) virtual returns (uint256);
}
// ----
// TypeError: (584-601): Invalid contract specified in override list: C.
// TypeError: (654-668): Function needs to specify overridden contracts B and D.
