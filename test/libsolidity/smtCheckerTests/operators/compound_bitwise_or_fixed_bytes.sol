pragma experimental SMTChecker;
contract C {
    function f() public pure returns (byte) {
        byte a = 0xff;
        byte b = 0xf0;
        b |= a;
        assert(a == b);

        a |= ~b;
        assert(a == 0); // fails
    }
}
// ----
// Warning 6328: (203-217): Assertion violation happens here.
