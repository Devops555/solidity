{
    function f() -> x { pop(address()) for { pop(callvalue()) } 0 {} { } }
}
// ====
// stackOptimization: true
// ----
// PUSH1 0x19
// JUMP
// JUMPDEST
// PUSH1 0x0
// ADDRESS
// POP
// CALLVALUE
// POP
// JUMPDEST
// PUSH1 0x0
// ISZERO
// PUSH1 0x15
// JUMPI
// JUMPDEST
// PUSH1 0xA
// JUMP
// JUMPDEST
// JUMPDEST
// SWAP1
// JUMP
// JUMPDEST
