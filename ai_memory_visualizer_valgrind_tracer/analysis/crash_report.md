# Crash Report: `crash_example`

## 1. Description of the Crash

**Program:** `programs/crash_example.c`
**Build command:** `gcc -Wall -Wextra -Werror -pedantic -std=gnu89 -g -o crash_example crash_example.c`
**Observed behavior:** The program prints two lines and then terminates abnormally.

```
crash_example: deterministic NULL dereference (segmentation fault)
  requesting n=0
Segmentation fault (core dumped)
```

Process exit status: `139` (128 + `SIGSEGV`, signal 11).

Kernel confirmation from `dmesg`:

```
crash_example[565]: segfault at 0 ip 000055da44cab28a sp 00007ffce8eb9b10 error 6
in crash_example[128a,55da44cab000+1000]
```

Two facts here matter and are not speculative — they come straight from the kernel's fault record:

* `segfault at 0` — the faulting **virtual address being accessed was `0x0`** (NULL).
* `error 6` — decoded per the x86 page-fault error code, bit values `4 | 2 = 6` mean the fault occurred in **user mode** on a **write** to a **non-present page**. This was a write attempt to unmapped memory, not a read, and not a permissions violation on mapped memory.

## 2. Root Cause Analysis (Causal Chain)

Tracing from source to instruction, not just asserting the conclusion:

1. `main()` declares `int n = 0;` (line 25) and never changes it before use.
2. `nums = allocate_numbers(n);` (line 30) calls `allocate_numbers(0)`.
3. Inside `allocate_numbers` (lines 4–20):
   ```c
   if (n <= 0)
       return NULL;
   ```
   Because `n == 0`, this guard is true, and the function returns `NULL` **before** ever calling `malloc`. This is correct, defensive code on the *allocation* side — it deliberately refuses to allocate a zero/negative-sized buffer and signals failure via a NULL return, which is the standard C convention.
4. Back in `main()`, the return value is stored: `nums = NULL;` — but **`nums` is never checked for NULL**.
5. Line 32 executes: `nums[0] = 42;`, i.e. `*(nums + 0) = 42`, which dereferences `nums` as a write. Since `nums == NULL`, this is a **write to address `0x0`**.

Disassembly (`objdump -d -l`) ties this directly to machine code, confirming the mapping from source line to fault address:

```
crash_example.c:30
    nums = allocate_numbers(n);
    127d: call   11a9 <allocate_numbers>
    1282: mov    %rax,-0x8(%rbp)      ; nums = return value (0x0)

crash_example.c:32
    nums[0] = 42;
    1286: mov    -0x8(%rbp),%rax      ; rax = nums (0x0)
    128a: movl   $0x2a,(%rax)         ; *rax = 42  <-- FAULTING INSTRUCTION
```

The instruction pointer at the moment of the fault (`ip 000055da44cab28a`) is exactly `0x128a`, the `movl $0x2a,(%rax)` instruction generated for `nums[0] = 42;`, with `%rax` holding `0`. Source, compiled code, and kernel fault record all agree on the same single point of failure. There is no ambiguity left to resolve by trial and error.

**Root cause:** `main()` fails to check the return value of `allocate_numbers()` for `NULL` before dereferencing it. The function correctly signaled failure (via its documented NULL-on-error contract); the caller ignored that signal.

## 3. Why the Memory Access Is Invalid

* **Category of undefined behavior:** NULL pointer dereference (a write dereference of an invalid pointer). In the C standard this falls under invalid pointer dereference / accessing an object through a pointer that does not point to a valid object (C11 §6.5.3.2, §J.2).
* Address `0x0` is never a valid mapped address in a normal user-space process — it is reserved specifically so that NULL dereferences fault immediately and loudly rather than corrupting arbitrary memory.
* Because the mapping at `0x0` does not exist, the MMU raises a page fault on the write; the kernel has no valid mapping to service it, so it delivers `SIGSEGV` to the process. The "segmentation fault" the user sees is this signal — the **effect** of the invalid access, not the cause.

### Stack, heap, or both?

* The **invalid access itself** is neither a stack overrun nor a heap overrun — it's a dereference of an unmapped address (`0x0`), which is a distinct category from both.
* **Stack memory** is involved only in the mechanical sense that `nums` is a local variable living in `main`'s stack frame (`-0x8(%rbp)`) — the stack frame is where the bad pointer value is *stored*, but the stack frame itself is never overrun or corrupted.
* **Heap memory** was never actually touched: `malloc` inside `allocate_numbers` is never reached because of the early-return guard for `n <= 0`. There is no heap corruption, no allocation failure, and no allocated buffer involved in this crash at all.
* Conclusion: this is a **pointer-validity bug**, not a stack- or heap-bounds bug. The stack merely holds the (correctly NULL) pointer value that the code then fails to validate before using it as a memory address.

## 4. Evaluating the AI-Provided Explanations

I asked an AI assistant to speculate on possible causes/fixes without seeing the disassembly or kernel fault record, to have something concrete to critique. Its typical output (paraphrased) offered several candidate explanations:

| AI Suggestion | Verdict | Why |
|---|---|---|
| "Likely a NULL pointer dereference because `nums` may not be checked after allocation." | **Correct**, and matches the confirmed root cause. | This is verifiable: the fault address is exactly `0x0`, and the disassembly shows `nums` (in `%rax`) is `0` at the faulting instruction. |
| "Could be a heap buffer overflow if `n` is larger than expected, writing past the allocated block." | **Incorrect / speculative** for this specific run. | With `n = 0`, `malloc` is never called (`n <= 0` guard). There is no allocated block to overflow. This explanation is a generically plausible C bug pattern, but it does not match the evidence for *this* crash. |
| "Could be a dangling pointer from `free()` being called twice or used after `free`." | **Incorrect** for this program. | `free(nums)` (line 36) is never reached — the crash occurs on line 32, before `free` executes. There is only one `free` call in the whole function, and it is unreached in the crashing run. |
| "Could be a stack overflow from deep/unbounded recursion." | **Incorrect.** | There is no recursion in this program; `allocate_numbers` is called once. The stack pointer in the fault record (`sp 00007ffce8eb9b10`) is a normal, shallow value, inconsistent with stack exhaustion. |
| "Suggested fix: check `nums` for NULL before dereferencing it, and handle the `n == 0` case explicitly (e.g., print an error and exit, or skip the write)." | **Correct and directly actionable.** | This addresses the actual root cause: the missing NULL check after `allocate_numbers()` returns. |

**Assessment of the AI's reasoning process:** the tool correctly identified NULL dereference as *one* plausible explanation among several, but by default it hedges across multiple generic "common C bugs" (overflow, double-free, stack exhaustion) rather than confirming which one actually applies. Without independently verifying against the fault address, the disassembly, or the actual control flow (the `n <= 0` guard that prevents `malloc` from ever running), the AI's overflow/double-free/stack-exhaustion suggestions would have been actively misleading if taken as diagnosis rather than brainstorming. This is the core risk of using an AI tool for this kind of analysis: its suggestions are hypotheses to be checked against hard evidence (kernel fault record, disassembly, source control flow), not conclusions to be trusted directly.

## 5. Suggested Fix *(optional, clearly a proposal, not applied to the code)*

```c
nums = allocate_numbers(n);

if (nums == NULL) {
    fprintf(stderr, "allocate_numbers failed for n=%d\n", n);
    return 1;
}

nums[0] = 42;
```

This restores the caller's obligation to honor the NULL-on-error contract that `allocate_numbers` already implements correctly. No change to `allocate_numbers` itself is needed — its behavior (refusing to allocate for `n <= 0` and signaling that via NULL) is correct; the bug is entirely in the caller's failure to check the result.
