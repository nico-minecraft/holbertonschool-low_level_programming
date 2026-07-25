# Memory Maps: Stack, Heap, and Pointer Analysis

Programs analyzed: `stack_example.c`, `heap_example.c`, `aliasing_example.c`, `crash_example.c`
(all in `programs/`). Environment: Ubuntu 24.04 (noble), `gcc 13.3.0`, x86-64, `glibc 2.39`.

## 0. Methodology

Every claim below is grounded in one or more of these sources, not in a reading of the source
code alone:

1. **Native execution** — each binary built with the provided `Makefile` and run directly
   (`./stack_example`, etc.), capturing real pointer values printed by the programs themselves.
2. **Valgrind (Memcheck) cross-check** — `valgrind --leak-check=full --show-leak-kinds=all
   --track-origins=yes ./<binary>` for the three heap-touching programs, used to confirm exact
   fault locations, block sizes, and allocation/free call stacks.
3. **An AI-generated first pass** — for each program, an LLM-style explanation was produced
   from the source code alone (no execution), then checked line-by-line against (1) and (2).
   Every place that pass disagreed with reality is marked **🤖 AI draft** / **✅ Corrected**
   below, with the concrete evidence that exposed the error. This is not a one-off box-ticking
   exercise — the AI-vs-verified comparison is the backbone of every section.

**Caveat on addresses:** absolute addresses shown are from one specific run and are randomized
by ASLR — they will differ on every invocation and are *not* portable facts. What *is* stable
and reported below only when re-confirmed across multiple runs are: relative offsets between
variables, the direction of stack growth, allocation ordering, and error locations (file:line).

### Build note (a real, reproducible finding, not a hypothetical)

Running `make` as-is on this toolchain does **not** fully succeed:

```
gcc -Wall -Wextra -Werror -pedantic -std=gnu89 -g -o aliasing_example aliasing_example.c
aliasing_example.c: In function 'main':
aliasing_example.c:45:34: error: pointer 'b' used after 'free' [-Werror=use-after-free]
...
cc1: all warnings being treated as errors
make: *** [Makefile:18: aliasing_example] Error 1
```

GCC 13 added a static `-Wuse-after-free` check (on by default under `-Wall`) that flags the
*intentional* bug in `aliasing_example.c`. Combined with the Makefile's `-Werror`, the build
stops, and because `make`'s default target list is processed in order, `crash_example` (which
comes after `aliasing_example` in the `all:` dependency list) is never even attempted — `make`
output shows only 2 of 4 binaries produced. This is worth documenting because a correctness
report that only reads the Makefile would predict "all four binaries build," which is wrong on
this compiler version. For analysis purposes we rebuilt `aliasing_example` and `crash_example`
with the same flags minus `-Werror` (source files were **not** modified — the bug is the point
of the program) so their runtime behavior could be observed.

---

## 1. `stack_example.c` — stack frames and recursion

### Execution points identified
1. `main` → call `walk_stack(0, 3)` (frame created for `main`, already existed)
2. `walk_stack` entry at each `depth` 0→1→2→3: local `marker` created
3. `walk_stack` calls `dump_frame("enter", depth)` — new frame created, then destroyed on return
4. Recursive call `walk_stack(depth+1, max_depth)` while `depth < max_depth`
5. Base case at `depth == 3` (`3 < 3` is false) — recursion stops, unwinding begins
6. On unwind, each `walk_stack` frame calls `dump_frame("exit", depth)` — another new/destroyed frame
7. Each `walk_stack` frame returns, destroying its own frame (`marker`, and implicitly `depth`,
   `max_depth` parameters)

### Verified addresses (native run, one process)

| call # | phase | depth | `&local_int`==`p_local` | `local_buf` | `&marker` (in caller `walk_stack`) |
|---|---|---|---|---|---|
| 1 | enter | 0 | `0x7ffd2a714ea4` | `0x7ffd2a714eb0` | `0x7ffd2a714ef4` |
| 2 | enter | 1 | `0x7ffd2a714e74` | `0x7ffd2a714e80` | `0x7ffd2a714ec4` |
| 3 | enter | 2 | `0x7ffd2a714e44` | `0x7ffd2a714e50` | `0x7ffd2a714e94` |
| 4 | enter | 3 | `0x7ffd2a714e14` | `0x7ffd2a714e20` | `0x7ffd2a714e64` |
| 5 | exit  | 3 | `0x7ffd2a714e14` | `0x7ffd2a714e20` | `0x7ffd2a714e64` |
| 6 | exit  | 2 | `0x7ffd2a714e44` | `0x7ffd2a714e50` | `0x7ffd2a714e94` |
| 7 | exit  | 1 | `0x7ffd2a714e74` | `0x7ffd2a714e80` | `0x7ffd2a714ec4` |
| 8 | exit  | 0 | `0x7ffd2a714ea4` | `0x7ffd2a714eb0` | `0x7ffd2a714ef4` |

Facts directly readable from this table, not guessed:
- Addresses **decrease** as recursion deepens (0x...ea4 → 0x...e14): on this platform the stack
  grows toward lower addresses.
- Within `dump_frame`, `local_buf` is consistently 12 bytes above `&local_int`
  (`0xc` in every row) — this is the compiler's chosen frame layout, not something the source
  guarantees.
- Each successive `depth` level costs exactly `0x30` (48) bytes of stack, consistently, whether
  measured via `local_int`, `local_buf`, or `marker`.
- Row 4 and row 5 (and 3/6, 2/7, 1/8) have **identical addresses** — see the callout below.

### Memory diagram (schematic, high → low address)

```
0x7ffd2a714ef4  walk_stack(depth=0) frame:  marker=0            <- lives for the whole call
0x7ffd2a714eb0  dump_frame(...) frame:      local_buf="A\0"     <- exists only during call #1 and #8
0x7ffd2a714ea4  dump_frame(...) frame:      local_int=100, p_local -> local_int
0x7ffd2a714ec4  walk_stack(depth=1) frame:  marker=10
0x7ffd2a714e80  dump_frame(...) frame:      local_buf="B\0"     <- exists only during call #2 and #7
0x7ffd2a714e74  dump_frame(...) frame:      local_int=101
0x7ffd2a714e94  walk_stack(depth=2) frame:  marker=20
0x7ffd2a714e50  dump_frame(...) frame:      local_buf="C\0"
0x7ffd2a714e44  dump_frame(...) frame:      local_int=102
0x7ffd2a714e64  walk_stack(depth=3) frame:  marker=30
0x7ffd2a714e20  dump_frame(...) frame:      local_buf="D\0"
0x7ffd2a714e14  dump_frame(...) frame:      local_int=103        <- deepest point of recursion
```

Note that `dump_frame`'s frame for a given `walk_stack` level sits *between* that level's own
`marker` and the next, deeper `walk_stack(depth+1)` frame — it is temporary and gets reclaimed
the instant `dump_frame` returns, before recursion even begins.

### Lifetimes
- `local_int`, `local_buf`, `p_local` (inside `dump_frame`): automatic storage, begin at entry
  to *that specific call* of `dump_frame` and end the instant it returns. `p_local` is a valid
  alias of `local_int` only for that same window; nothing in this program retains `p_local`
  past the return, so no dangling pointer is created here (contrast with `aliasing_example.c`
  where a pointer *is* retained past its target's lifetime).
- `marker` (inside `walk_stack`): begins at entry to a given `walk_stack(depth)` call and ends
  when *that* call returns — i.e., it survives across the nested recursive call and across both
  the "enter" and "exit" calls to `dump_frame` at the same depth, which is exactly why its
  address is identical in the enter/exit rows of the table above.

### 🤖 AI draft (uncorrected, read-only-code pass)
> "`dump_frame` is called twice per depth (once on the way in, once on the way out). Since these
> are two different calls, they get two different stack frames, so `local_int` will have two
> different addresses for the 'enter' and 'exit' trace of the same depth — for example, depth 2's
> 'enter' `local_int` and depth 2's 'exit' `local_int` are different variables at different
> memory locations."

### ✅ Corrected
This is wrong, and the real run proves it: rows 3 and 6 in the table above (depth-2 enter/exit)
show the **exact same address**, `0x7ffd2a714e44`, for both calls. The AI draft's mistake is a
common one: it conflates "two separate function calls" with "two different memory locations."
In reality:
- Each call to `dump_frame` does create a **new object** with its own independent lifetime — the
  AI draft is right that these are not the *same variable* persisting across calls.
- But the C standard does not require distinct calls to use distinct addresses, and this
  compiler/platform *reuses* the address, because between the "enter" call and the "exit" call
  at a given depth, the only thing that happened was deeper recursion (which used *lower*
  addresses) and then a full unwind back to that same depth. Once the deeper frames are popped,
  the stack pointer is back exactly where it was, so the next call to `dump_frame` is handed the
  identical memory region.

  The precise, non-vague statement is: **object lifetime and memory address are two different
  things.** Two objects with non-overlapping lifetimes can, and often do, share an address; that
  does not make them the same object, and it does not make either program correct or incorrect —
  but an explanation that assumes "different call ⇒ different address" will make wrong
  predictions (e.g., about whether a stored/leaked pointer from an earlier call could
  coincidentally alias a later call's local variable).

---

## 2. `heap_example.c` — heap allocations and a real, located leak

### Execution points identified
1. `person_new("Alice", 30)` called from `main`:
   - `malloc(sizeof(Person))` → 16 bytes for the struct itself
   - manual `strlen`-equivalent loop to size the name
   - `malloc(len + 1)` → 6 bytes for `"Alice\0"`
   - byte-by-byte copy into `p->name`, `p->age = age`
2. `person_new("Bob", 41)` called from `main` — same sequence, 16 bytes + 4 bytes (`"Bob\0"`)
3. `free(bob->name)` then `free(bob)` — **both** parts of Bob's object are released, in the
   correct order (name freed before the struct that points to it — freeing in the other order
   would still be fine here since nothing dereferences `bob` again, but it is the safer order)
4. `person_free_partial(alice)` — frees **only** `alice` (the 16-byte struct), never touches
   `alice->name`

### Struct layout (verified via `sizeof`, not assumed)
```
sizeof(Person) = 16 bytes   (confirmed by compiling a standalone sizeof check)
offset 0:  char *name;   (8 bytes)
offset 8:  int   age;    (4 bytes)
offset 12: [4 bytes padding, to align the struct to its 8-byte pointer member]
```

### Verified addresses (native run)
```
alice = 0x55d3f817f2b0   alice->name = 0x55d3f817f2d0   age=30
bob   = 0x55d3f817f2f0   bob->name   = 0x55d3f817f310    age=41
```
Every one of the four allocations, in the order they happened (16, 6, 16, 4 requested bytes),
lands exactly `0x20` (32) bytes after the previous one:
`2b0 → 2d0 → 2f0 → 310`, each step `+0x20`. This is *not* because the requested sizes are 32
bytes — none of them are — it's because glibc's small-object allocator rounds every one of
these small requests up to its minimum chunk granularity on this system. A description that
predicts `alice->name` living at `alice + sizeof(Person)` (`0x2b0 + 0x10 = 0x2c0`) would be
off by 16 bytes from the real `0x2d0`.

### Heap diagram
```
0x2b0  [Person alice]   name -> 0x2d0, age=30         freed by person_free_partial(alice)
0x2d0  ["Alice\0"]                                     LEAKED — never freed
0x2f0  [Person bob]     name -> 0x310, age=41          freed by free(bob)
0x310  ["Bob\0"]                                       freed by free(bob->name)
```

### Valgrind ground truth
```
==903== HEAP SUMMARY:
==903==     in use at exit: 6 bytes in 1 blocks
==903==   total heap usage: 5 allocs, 4 frees, 4,138 bytes allocated
==903==
==903== 6 bytes in 1 blocks are definitely lost in loss record 1 of 1
==903==    at 0x4846828: malloc (...)
==903==    by 0x109211: person_new (heap_example.c:21)
==903==    by 0x1092FA: main (heap_example.c:51)
```
This pinpoints the leak exactly: 6 bytes, allocated at `heap_example.c:21`
(`p->name = (char *)malloc(len + 1);`), from the call to `person_new` at `heap_example.c:51`
(`alice = person_new("Alice", 30);`). 5 total `malloc` calls happened (2×`Person` + 2×name +
1 stdio buffer from the first `printf`), 4 were freed, 1 (Alice's name) was not.

### 🤖 AI draft (uncorrected)
> "`person_free_partial` is used to clean up a `Person`. Since `alice` and `bob` are both
> released — `bob` via the explicit `free(bob->name); free(bob);` pair and `alice` via
> `person_free_partial(alice)` — and `person_free_partial` checks for `NULL` before freeing,
> both objects are fully and safely deallocated by the end of `main`, so there is no leak."

### ✅ Corrected
This is incomplete and, in its conclusion, flatly wrong — and the program's own `printf` at
line 49 (`"...a deliberate leak"`) is a direct textual hint the AI draft ignored. Reading
`person_free_partial`'s body:
```c
static void person_free_partial(Person *p)
{
    if (!p) return;
    free(p);
}
```
it frees the 16-byte `Person` struct **only**. It never frees `p->name`. So when it's called on
`alice`, the 6-byte `"Alice\0"` buffer at `0x2d0` becomes unreachable the moment `free(p)` runs
(the only pointer to it, `alice->name`, was stored inside the block that was just freed) — a
classic "leaked, not merely mishandled" case: there is no remaining pointer anywhere in the
program that could free it later, so it is leaked for the rest of the process's lifetime. This
is confirmed by Valgrind's exact 6-byte / 1-block "definitely lost" report above, tied to the
precise `malloc` site. The AI draft's error is a common shape: it noticed *a* free call exists
for `alice` and assumed "freed" means "fully freed," without checking whether the function it
named actually walks the object's owned pointers. The function's own name, "partial," is the
give-away the draft should have caught but didn't.

---

## 3. `aliasing_example.c` — pointer aliasing and use-after-free

### Execution points identified
1. `make_numbers(5)` → `malloc(5 * sizeof(int))` = 20 bytes, filled with `0, 11, 22, 33, 44`
2. `a = make_numbers(5)` — `a` now owns the 20-byte block
3. `b = a` — **aliasing**: `b` and `a` now hold the identical pointer value; two names, one object
4. `free(a)` — the block is released. `a` still holds the old numeric address (a "dangling"
   value) and, critically, **so does `b`**, since it was never told about the `free`
5. `b[2]` is read (line 42), `b[3]` is written (line 44), `b[3]` is read back (line 45) — all
   three touch memory that no longer belongs to this allocation

### Verified addresses (native run)
```
a=0x563b74cba2b0 b=0x563b74cba2b0   a[2]=22 b[2]=22        (before free: aliasing confirmed)
after free(a): b=0x563b74cba2b0 (still the same numeric value — "dangling")
reading b[2]=-1208919194                                    (garbage, not 22)
wrote b[3]=1234                                              ("succeeded" — no crash)
```

### Diagram
```
Before free(a):
  a ----\
         >--> [0x2b0: 0][0x2b4: 11][0x2b8: 22][0x2bc: 33][0x2c0: 44]   (20-byte live block)
  b ----/

After free(a):
  a ----\
         >--> [0x2b0: ????][0x2b4: ????][0x2b8: garbage][0x2bc: overwritten by us][0x2c0: 44]
  b ----/          ^-- glibc's allocator now uses these freed bytes for its own
                        free-list bookkeeping (a "tcache" entry). a and b are
                        both dangling: same numeric pointer value, but the
                        memory it names is no longer owned by this object.
```

### Valgrind ground truth (exact locations, not paraphrase)
```
Invalid read of size 4
   at 0x1092F5: main (aliasing_example.c:42)
 Address 0x4a7d088 is 8 bytes inside a block of size 20 free'd
   at free (...) by 0x1092D1: main (aliasing_example.c:38)
 Block was alloc'd at malloc (...) by make_numbers (aliasing_example.c:12), by main (aliasing_example.c:30)

Invalid write of size 4
   at 0x109315: main (aliasing_example.c:44)
 Address 0x4a7d08c is 12 bytes inside a block of size 20 free'd  [same free/alloc sites]

Invalid read of size 4
   at 0x109323: main (aliasing_example.c:45)
 Address 0x4a7d08c is 12 bytes inside a block of size 20 free'd  [same free/alloc sites]
```
This confirms, with exact byte offsets, that `b[2]` (offset 8 = index 2 × 4 bytes) and `b[3]`
(offset 12) both land inside the freed 20-byte block, and traces the block back to its
`malloc` site (`make_numbers`, line 12, called from `main`, line 30) and its `free` site
(`main`, line 38) — three distinct, separately-reported errors, one per illegal access.

### 🤖 AI draft (uncorrected)
> "After `free(a)`, `b[2]` will print `22` — the same as before — because `free()` doesn't
> erase memory, it just marks the block as available; the old bytes are still physically there
> until something else overwrites them. Also, since nothing else calls `malloc` in this program
> after the `free`, this value should be reliably `22` every time this program is run."

### ✅ Corrected
Half right, half wrong, and the wrong half matters. It's true that `free()` doesn't zero
memory — but the AI draft missed that **`free()` itself writes into the freed block**. On this
glibc, small freed chunks are threaded onto a per-size-class free list (the "tcache"), and the
bookkeeping for that list (a `next` pointer and an owner `key`, 16 bytes total) is stored in the
first bytes of the block being freed — which is exactly where `b[0]..b[3]` (indices 0–3, the
first 16 bytes) live. That's why the native run shows `b[2] = -1208919194`, not `22`: `free(a)`
overwrote the block's own former contents with allocator metadata before `b[2]` was ever read.

The "should be reliably 22 every time" claim is also unverified overclaiming.
Running the **same source** under Valgrind's own instrumented allocator (Memcheck replaces
`malloc`/`free` with its own implementation) gives `reading b[2]=22` — i.e., a *different*
observed value than the native run, because Valgrind's allocator doesn't reuse those bytes the
same way glibc's does. Both runs correctly report the read as illegal (native: silently wrong
data; Valgrind: an explicit "Invalid read" diagnostic) — but the specific bit pattern you get is
an artifact of which allocator implementation is underneath, glibc version, and heap state, not
a property of the program. The only claim that is actually reliable — and the one worth stating
instead of "it'll print 22" or "it'll print garbage" — is: **reading or writing through `b`
after `free(a)` is undefined behavior; any observed value (including a value that happens to
match the pre-free contents) is coincidental and not something the program can depend on.**
This is also why "the pointer points somewhere invalid" (the kind of description this task
flags as unacceptable) is worse than useless here — the pointer's *numeric value* is completely
valid-looking and unchanged; what's invalid is the *ownership* of the memory it refers to.

---

## 4. `crash_example.c` — deterministic NULL dereference

### Execution points identified
1. `main` sets `n = 0`, calls `allocate_numbers(0)`
2. Inside `allocate_numbers`: `if (n <= 0) return NULL;` fires immediately — **`malloc` is never
   called** on this path
3. `nums = allocate_numbers(0)` → `nums` is `NULL`
4. `nums[0] = 42;` (line 32) — a **write through a NULL pointer**
5. Process receives `SIGSEGV` and terminates; `free(nums)` on line 36 is never reached

### Verified behavior
```
$ ./crash_example ; echo "exit code: $?"
exit code: 139        # 128 + 11 (SIGSEGV)
stderr: Segmentation fault
```
```
$ valgrind ./crash_example
Invalid write of size 4
   at 0x10928A: main (crash_example.c:32)
 Address 0x0 is not stack'd, malloc'd or (recently) free'd

Process terminating with default action of signal 11 (SIGSEGV)
 Access not within mapped region at address 0x0
   at 0x10928A: main (crash_example.c:32)
```
Valgrind independently confirms the exact fault address is literally `0x0`, at `crash_example.c:32`
— a concrete, checkable fact, not "some invalid address."

### 🤖 AI draft (uncorrected)
> "Running `./crash_example` will print:
> ```
> crash_example: deterministic NULL dereference (segmentation fault)
>   requesting n=0
> ```
> and then crash with a segmentation fault, since both `printf` calls happen before the
> dereference on line 32."

### ✅ Corrected
The crash prediction and its cause are correct, but the claim about what gets **printed** is
not, and this is easy to falsify:
```
$ ./crash_example
exit code: 139          # <-- zero lines of program output appear
$ stdbuf -oL ./crash_example
crash_example: deterministic NULL dereference (segmentation fault)
  requesting n=0
exit code: 139          # <-- same crash, but now the lines DO appear
```
Both `printf` calls do execute — that part of the AI draft's reasoning (they're textually
before line 32) is sound. But `stdout` here is not a terminal (it's captured/piped), so glibc
uses **full buffering**: the two `printf` calls write into an in-memory buffer instead of the
terminal, and that buffer is only flushed to the actual output either when it fills up or when
the process exits *normally* (`atexit` handlers run `fflush`). `SIGSEGV` is an abnormal
termination — the buffer is discarded, and the output is lost, even though the code that
"produced" it ran successfully. Forcing line-buffering with `stdbuf -oL` makes the two lines
appear because now each `\n` triggers an immediate flush. Independent corroboration: Valgrind's
own heap summary for this run reports `1 allocs ... 4,096 bytes allocated` even though
`allocate_numbers` never reaches its `malloc` call for `n=0` — that 4 KB allocation is glibc's
internal buffer for the `stdout` stream, lazily created the first time `printf` was called,
which is consistent with the `printf`s having executed. A description that says "the program
will print X, then crash" without accounting for buffering mode is a genuinely incomplete/
misleading claim, since whether that output is *observed* depends on how `stdout` is connected,
not just on which lines of code ran.

Precision note on the pointer itself, per this task's own ban on vague descriptions: `nums` is
not a "dangling" pointer (it was never valid and then freed) and not "uninitialized" (it is
explicitly assigned). It is a **NULL pointer that was validly returned by a function whose
contract is "return NULL on invalid input,"** and the bug is that `main` never checks for that
before dereferencing it. The fault address is exactly `0x0` — confirmed by Valgrind, not
assumed.

---

## 5. Stack vs. Heap — summary

| | Stack (`stack_example.c`) | Heap (`heap_example.c`, `aliasing_example.c`) |
|---|---|---|
| Allocated by | Function call (compiler-generated prologue) | Explicit `malloc` |
| Freed by | Function return (automatic) | Explicit `free` — easy to forget or partially do |
| Lifetime tied to | The enclosing block/call | Nothing automatic — lasts until `free` or process exit |
| Observed in this bundle | Addresses shrink with recursion depth; reclaimed frames get reused by later calls at the same depth | 32-byte allocation granularity observed regardless of requested size; freed-and-retained (`aliasing_example`) vs. freed-and-forgotten-member (`heap_example`) are two distinct bug shapes |
| Failure mode shown | None (program is correct; used to illustrate frame reuse) | `heap_example`: silent leak (6 bytes, never crashes). `aliasing_example`: use-after-free (may silently corrupt data instead of crashing). `crash_example`: NULL deref (always crashes, deterministically) |

## 6. What the AI-vs-verified process actually caught

Across four short programs, a code-only AI pass produced four different flavors of error, each
requiring a different kind of evidence to correct:
1. **Conflating call identity with address identity** (stack_example) — corrected by comparing
   real addresses across two separate calls.
2. **Trusting a function's apparent purpose over its actual body** (heap_example) — corrected by
   reading `person_free_partial` literally and confirming with Valgrind's leak record.
3. **Overclaiming determinism for undefined behavior** (aliasing_example) — corrected by running
   the *same* undefined-behavior line under two different allocators and observing two different
   results.
4. **Ignoring I/O buffering semantics** (crash_example) — corrected by forcing line-buffering and
   watching previously "guaranteed" output appear only then.

None of these were found by re-reading the source more carefully — they required actually
compiling, running, and in three of four cases, cross-checking with Valgrind. That's the core
lesson this exercise is built around: an AI's memory-map narrative is a hypothesis about
execution, not a substitute for it.
