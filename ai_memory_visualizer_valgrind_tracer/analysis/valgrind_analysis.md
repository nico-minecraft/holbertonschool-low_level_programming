# Valgrind Analysis: Reading Diagnostics as Memory Evidence

This document interprets Valgrind's output for the four programs in `programs/` as direct
consequences of specific memory operations — not as opaque diagnostic codes. It builds directly
on the object lifetimes and pointer maps established in `analysis/memory_maps.md` (Task 1); where
useful, this document cites that file instead of re-deriving facts already proven there.

Raw, unedited logs for every run are saved under `analysis/valgrind_logs/*.log` and referenced by
name below, so every quoted fragment can be checked against the full output.

## 0. Method

Command used for every program:
```
valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./<binary>
```
- `--leak-check=full` — report each individual leaked block (with an allocation stack trace),
  not just a leak count.
- `--show-leak-kinds=all` — report **all four** leak categories (`definitely lost`,
  `indirectly lost`, `possibly lost`, `still reachable`), not just the default subset. This
  matters below: one of `crash_example`'s records only shows up because of this flag, and
  correctly reading its category (not just its existence) is the crux of one of the corrections
  in §3.
- `--track-origins=yes` — for uninitialized-value errors, trace the *origin* of the undefined
  bytes (stack allocation vs. heap allocation, and where) rather than just the point where they
  were used. Costlier, but this is exactly the evidence needed to explain *why* an error fires,
  which is the whole point of this exercise.

`stack_example` is run first deliberately, as a **control**: it is the one program with no
memory-safety bugs, so its clean report establishes what "zero issues" looks like on this
toolchain (including the *expected*, harmless heap activity from glibc itself) before the other
three reports are read as deviations from it.

---

## 1. `stack_example` — control run (0 errors)

```
==567== HEAP SUMMARY:
==567==     in use at exit: 0 bytes in 0 blocks
==567==   total heap usage: 1 allocs, 1 frees, 4,096 bytes allocated
==567==
==567== All heap blocks were freed -- no leaks are possible
==567==
==567== ERROR SUMMARY: 0 errors from 0 contexts (suppressed: 0 from 0)
```
*(full log: `analysis/valgrind_logs/stack_example.log`)*

- **Type of error:** none.
- **Memory object involved:** one 4,096-byte heap block, allocated and freed by glibc itself
  (this program contains no `malloc`/`free` calls — every variable in `stack_example.c` is a
  stack local, matching `memory_maps.md` §1). That single alloc/free pair is glibc lazily
  creating, then cleanly releasing, the internal buffer for the `stdout` stream.
- **Why zero errors:** the program never allocates heap memory, so there is nothing for Memcheck
  to lose track of; its only stack accesses are to variables it declared and initialized itself
  in the same or an enclosing scope (`local_int`, `local_buf[0..1]`, `marker`), so there is no
  read of unwritten memory either. This 1-alloc/1-free pattern is the baseline to compare against
  in §4, where the *same* stdio allocation appears again in `crash_example` — but that time
  **not** freed, for a specific, diagnosable reason.

---

## 2. `heap_example` — memory leak (lost ownership)

```
==568== HEAP SUMMARY:
==568==     in use at exit: 6 bytes in 1 blocks
==568==   total heap usage: 5 allocs, 4 frees, 4,138 bytes allocated
==568==
==568== 6 bytes in 1 blocks are definitely lost in loss record 1 of 1
==568==    at 0x4846828: malloc (in .../vgpreload_memcheck-amd64-linux.so)
==568==    by 0x109211: person_new (heap_example.c:21)
==568==    by 0x1092FA: main (heap_example.c:51)
==568==
==568== LEAK SUMMARY:
==568==    definitely lost: 6 bytes in 1 blocks
==568==    indirectly lost: 0 bytes in 0 blocks
==568==      possibly lost: 0 bytes in 0 blocks
==568==    still reachable: 0 bytes in 0 blocks
```
*(full log: `analysis/valgrind_logs/heap_example.log`)*

- **Type of error:** memory leak, and specifically the **"definitely lost"** category — Valgrind's
  strongest classification, meaning: at process exit, no pointer *anywhere* in the program's
  reachable memory (registers, stack, globals, or other live heap blocks) refers to this block,
  and the block was never freed. This is distinct from "still reachable" (see §4, where a
  reachable-but-unfreed block from `crash_example` is deliberately *not* called a leak) and from
  "possibly lost" (used when Valgrind finds only interior, not base, pointers to a block — not
  the case here).
- **Memory object involved:** the 6-byte block returned by `malloc(len + 1)` at
  `heap_example.c:21`, reached via `person_new` called from `main` at `heap_example.c:51` — i.e.
  `alice->name`, holding `"Alice\0"`. Confirmed against `memory_maps.md` §2's heap diagram, which
  independently identified this exact block (address `0x55d3f817f2d0` in that run) as the leaked
  one from the program's own printed pointer values and the "deliberate leak" hint in the source.
- **Lifetime violation:** `alice->name`'s only owning pointer is the `name` field inside the
  `Person` struct at `alice`. `person_free_partial(alice)` frees that struct:
  ```c
  static void person_free_partial(Person *p)
  {
      if (!p) return;
      free(p);          /* frees the 16-byte struct only */
  }
  ```
  The instant `free(p)` runs, the only reference to the 6-byte name block is destroyed along with
  the memory that held it — nothing in the program ever read `p->name` out into a separate
  variable first. The block still exists on the heap (its lifetime doesn't end until `free`d),
  but it is now **unreachable**: this is a leak caused by lost ownership, not a use-after-free
  (nothing accesses the block after this point) and not a double-free (it's simply never freed
  again). Contrast with `bob`, freed correctly two lines earlier in `main` via
  `free(bob->name); free(bob);` — the member is freed *before* the struct that points to it, so
  no reference is ever severed while still needed.
- **Total-heap-usage sanity check:** "5 allocs, 4 frees" — 2×`Person` + 2×name = 4 program
  allocations, plus 1 for the stdio buffer (as in §1) = 5; of the 5, `bob` (struct + name),
  `alice`'s struct, and the stdio buffer are freed = 4; `alice->name` is the one unfreed
  allocation, consistent with "6 bytes in 1 blocks" left in use at exit.

---

## 3. `aliasing_example` — three use-after-free errors (not an overflow)

```
==569== Invalid read of size 4
==569==    at 0x1092F5: main (aliasing_example.c:42)
==569==  Address 0x4a7d088 is 8 bytes inside a block of size 20 free'd
==569==    at 0x484988F: free (...)
==569==    by 0x1092D1: main (aliasing_example.c:38)
==569==  Block was alloc'd at
==569==    at 0x4846828: malloc (...)
==569==    by 0x1091E4: make_numbers (aliasing_example.c:12)
==569==    by 0x109272: main (aliasing_example.c:30)

==569== Invalid write of size 4
==569==    at 0x109315: main (aliasing_example.c:44)
==569==  Address 0x4a7d08c is 12 bytes inside a block of size 20 free'd
              [same free/alloc stacks]

==569== Invalid read of size 4
==569==    at 0x109323: main (aliasing_example.c:45)
==569==  Address 0x4a7d08c is 12 bytes inside a block of size 20 free'd
              [same free/alloc stacks]
...
==569== HEAP SUMMARY:
==569==     in use at exit: 0 bytes in 0 blocks
==569== All heap blocks were freed -- no leaks are possible
==569== ERROR SUMMARY: 3 errors from 3 contexts (suppressed: 0 from 0)
```
*(full log: `analysis/valgrind_logs/aliasing_example.log`)*

- **Type of error (all three):** use-after-free, reported by Memcheck as `Invalid read of size 4`
  (lines 42, 45) and `Invalid write of size 4` (line 44). The specific phrase **"is N bytes
  inside a block of size 20 free'd"** — as opposed to "N bytes after a block of size 20 alloc'd",
  which is Valgrind's phrasing for a heap-buffer-*overflow* — is what identifies these as
  use-after-free rather than out-of-bounds: the accessed addresses (offsets 8 and 12) are well
  *within* the original 0–19 byte extent of the block; what makes them invalid is that the block's
  **lifetime has already ended**, not that the offset is out of range.
- **Memory object involved:** the single 20-byte `int[5]` array allocated in `make_numbers` at
  `aliasing_example.c:12` (via `main`, line 30) and freed via `free(a)` at `aliasing_example.c:38`.
  Both `a` and `b` hold this block's address — `b = a` on line 34 makes `b` an alias, per
  `memory_maps.md` §3's diagram, *before* the free.
- **Lifetime violation:** `free(a)` on line 38 ends the block's lifetime. C does not retroactively
  invalidate other pointers that held the same value — `b` still holds the identical numeric
  address it did before the free (confirmed in `memory_maps.md`: `b=0x563b74cba2b0` unchanged
  after `free(a)`), but the memory it refers to no longer belongs to this object. Every subsequent
  access through `b` — the read at line 42, the write at line 44, the read-back at line 45 — is a
  read or write through a pointer whose *referent's lifetime has already ended*, which is the
  precise definition of use-after-free. This is a lifetime bug expressed through *aliasing*: the
  bug isn't in `free(a)` itself (freeing `a` is correct and necessary), it's that `b` was never
  informed the object it aliases is gone.
- **Why "HEAP SUMMARY: 0 bytes in use... no leaks are possible" doesn't mean this program is
  fine:** leak-freedom and memory-safety are different properties. Every byte this program
  allocated was, in fact, freed exactly once (`total heap usage: 2 allocs, 2 frees` — the array
  and the stdio buffer) — there is no leak here at all. But the program still has three serious,
  distinct Memcheck errors. A review that only checked the leak summary would wrongly conclude
  this file is clean.

---

## 4. `crash_example` — NULL dereference, plus a non-leak "still reachable" record

```
==570== Invalid write of size 4
==570==    at 0x10928A: main (crash_example.c:32)
==570==  Address 0x0 is not stack'd, malloc'd or (recently) free'd
==570==
==570== Process terminating with default action of signal 11 (SIGSEGV)
==570==  Access not within mapped region at address 0x0
==570==    at 0x10928A: main (crash_example.c:32)
...
==570== HEAP SUMMARY:
==570==     in use at exit: 4,096 bytes in 1 blocks
==570==   total heap usage: 1 allocs, 0 frees, 4,096 bytes allocated
==570==
==570== 4,096 bytes in 1 blocks are still reachable in loss record 1 of 1
==570==    at 0x4846828: malloc (...)
==570==    by 0x48EC1B4: _IO_file_doallocate (filedoalloc.c:101)
==570==    by 0x48FC523: _IO_doallocbuf (genops.c:347)
==570==    by 0x48F9F8F: _IO_file_overflow@@GLIBC_2.2.5 (fileops.c:745)
==570==    by 0x48FAAAE: _IO_new_file_xsputn (fileops.c:1244)
==570==    by 0x48EEC5C: puts (ioputs.c:40)
==570==    by 0x10925E: main (crash_example.c:27)
==570==
==570== LEAK SUMMARY:
==570==    definitely lost: 0 bytes in 0 blocks
==570==    still reachable: 4,096 bytes in 1 blocks
==570== ERROR SUMMARY: 1 errors from 1 contexts (suppressed: 0 from 0)
```
*(full log: `analysis/valgrind_logs/crash_example.log`)*

There are **two separate findings** here, of two different kinds, and conflating them is a
mistake (see Correction B in §5).

### 4a. The Memcheck error: invalid write, NULL dereference
- **Type of error:** invalid write, and specifically a NULL-pointer dereference. Valgrind's
  wording is the tell, and it differs meaningfully from §3's wording: **"Address 0x0 is not
  stack'd, malloc'd or (recently) free'd."** This is the phrasing used when an address was never
  valid for *any* known reason (not on the stack, not from `malloc`, not recently freed) —
  contrast with `aliasing_example`'s "...is N bytes inside a block ... free'd", which fires for
  an address that *was* valid until a specific, traceable `free()` call. `0x0` is the reserved
  null address; no allocator will ever legitimately hand it out.
- **Memory object involved:** there isn't one — that's the point. `nums` in `main` is assigned the
  return value of `allocate_numbers(0)`, which (per `crash_example.c`'s own guard,
  `if (n <= 0) return NULL;`) returns `NULL` **without ever calling `malloc`**. `nums[0] = 42;` on
  line 32 dereferences that NULL value.
- **Lifetime violation:** technically none — this isn't a lifetime bug (nothing was freed or went
  out of scope), it's a **validity** bug: `nums` was never bound to a real object in the first
  place on this code path, and `main` writes through it without checking. Precision matters here
  because the task explicitly flags "the pointer points somewhere invalid" as unacceptably vague:
  the correct, checkable statement is that `nums` holds the specific value `0x0`, confirmed both
  by Valgrind's fault address and by `memory_maps.md` §4's account of the guard clause.

### 4b. The leak-check record: still reachable, not a leak
- **Type of report:** `still reachable`, 4,096 bytes, one block. This is one of the four leak
  categories `--show-leak-kinds=all` exposes, and it is explicitly *not* grouped under
  `definitely lost` in the LEAK SUMMARY (`definitely lost: 0 bytes` in this same run) — Valgrind's
  own classification distinguishes "a live pointer to this block still exists at exit" (still
  reachable) from "no pointer to this block exists anywhere" (lost/leaked).
- **Memory object involved:** the allocation stack traces it precisely: `malloc` ←
  `_IO_file_doallocate` ← `_IO_doallocbuf` ← `_IO_file_overflow` ← `_IO_new_file_xsputn` ← `puts`
  ← `main (crash_example.c:27)`. Line 27 is
  `printf("crash_example: deterministic NULL dereference (segmentation fault)\n");` — GCC
  optimizes a constant, newline-terminated `printf` with no format specifiers into a plain
  `puts()` call, which is why the trace names `puts` rather than `printf`. This is glibc lazily
  allocating `stdout`'s internal 4 KB output buffer the first time anything is written to it.
- **Why "still reachable," not "leaked":** the buffer is still pointed to by glibc's internal
  `FILE` structure for `stdout` at the moment of the crash — it was never orphaned. It's simply
  never freed, because freeing it is normally the job of `exit()`'s cleanup handlers
  (`fclose`/`_IO_cleanup`), which only run on **normal** termination. `SIGSEGV` is an abnormal
  termination — the process is killed before that cleanup gets a chance to run. This is a direct,
  independent confirmation (via an exact call stack) of the buffering finding already established
  in `memory_maps.md` §4 by comparing `./crash_example` against `stdbuf -oL ./crash_example`:
  the two `printf`/`puts` calls before line 32 really did execute and really did allocate glibc's
  output buffer — they just never got to flush or free it. Compare directly to §1's control run,
  where the *same* stdio allocation is cleanly freed (`1 allocs, 1 frees`) because that program
  exits normally.

---

## 5. AI-vs-verified corrections (interpreting Valgrind output specifically)

The two corrections below are about **misreading Valgrind's diagnostics themselves** — a
different failure mode from Task 1's corrections, which were about predicting program behavior
before running anything. Both were checked against (a) the exact source lines named in the
report and (b) the object lifetimes already established in `memory_maps.md`, per the task
instructions.

### Correction A — `aliasing_example`: use-after-free misread as a buffer overflow

**🤖 AI draft (uncorrected):**
> "Valgrind's three `Invalid read`/`Invalid write of size 4` errors in `aliasing_example` mean
> the program is reading past the end of the array — `a` was allocated for 5 ints (20 bytes),
> and accessing index 2 and 3 through `b` after the array has effectively 'shrunk' is an
> out-of-bounds/buffer-overflow bug. The fix would be to allocate more space up front."

**✅ Corrected:**
This misreads both the report and the code, and would point a fix at the wrong target. Two
concrete pieces of evidence rule it out:
1. **The offsets are in-bounds.** Index 2 (byte offset 8) and index 3 (byte offset 12) are both
   well inside a 20-byte (indices 0–4) array — there is no shrinking or resizing anywhere in this
   program, and Valgrind never says the address is *outside* the block's bounds.
2. **Valgrind's own wording says the opposite of "overflow."** Every one of the three errors is
   reported as "N bytes **inside** a block of size 20 **free'd**," with an accompanying stack
   trace for the `free()` call at line 38. Valgrind reserves this phrasing specifically for
   accesses to memory that was valid and in-bounds but whose block has since been released; a
   genuine overflow report instead says "N bytes **after** a block of size M **alloc'd**" and
   carries no `free` stack trace at all, because the block in that case is still live. The
   presence of a `free` stack trace here is the single most load-bearing detail in the report,
   and the AI draft's explanation never engages with it.

Getting this right isn't pedantic: an overflow bug is fixed by allocating more memory or bounding
the loop that writes past the array; a use-after-free bug (the actual bug here) is fixed by
managing pointer lifetime — e.g., setting `b = NULL` right after `free(a)`, or restructuring so
no alias outlives the object. Applying the overflow "fix" (allocate more space) would do nothing
here, since the array was always large enough for the indices being accessed.

### Correction B — `crash_example`: "still reachable" misread as an additional leak

**🤖 AI draft (uncorrected):**
> "Besides the NULL-pointer crash, Valgrind's HEAP SUMMARY for `crash_example` shows '4,096
> bytes in 1 blocks' still allocated at program exit, so the program also has a 4 KB memory leak
> that should be fixed with an extra `free()` call."

**✅ Corrected:**
The premise — "still allocated at exit" — is true, but "leak" is the wrong word for it, and
Valgrind's own report says so explicitly if you read past the HEAP SUMMARY into the LEAK SUMMARY
a few lines later: `definitely lost: 0 bytes in 0 blocks` / `still reachable: 4,096 bytes in 1
blocks`. Valgrind draws this distinction deliberately — "still reachable" means a pointer chain
to the block still exists (here, inside glibc's internal `FILE` structure for `stdout`), which is
the opposite of what makes something a leak (an *unreachable*, orphaned block). Following the
block's own allocation stack trace (§4b above) shows it is glibc's stdio buffer, created as a
side effect of the program's own `printf`/`puts` call at line 27, not something `crash_example.c`
allocated or is responsible for freeing directly. Every buffered-I/O C program that terminates via
an uncaught signal — completely independent of whether the rest of the program is correct — will
show an equivalent "still reachable" record for its stdio buffers, simply because normal-exit
cleanup never got to run. Calling this a leak "to fix" would send someone looking for a missing
`free()` in application code that doesn't need one, and would distract from the actual, single
real bug in this file: the unchecked NULL dereference on line 32.

---

## 6. Supplementary: use-of-uninitialized-memory (not present in the required four programs)

None of `stack_example`, `heap_example`, `aliasing_example`, or `crash_example` triggers a
"Conditional jump or move depends on uninitialised value(s)" / "Use of uninitialised value"
warning, and it's worth confirming *why*, rather than leaving the category silently unaddressed:
- `stack_example`: `local_buf[0]` and `local_buf[1]` are written before being read; bytes
  `local_buf[2..15]` are never read at all (writing without reading never triggers this check).
- `heap_example` / `aliasing_example`: every heap byte that's read was written first, either by
  the copy loop in `person_new` or by `make_numbers`'s fill loop — the bugs in these two programs
  are about lifetime (§2, §3), not initialization.
- `crash_example`: the fault happens before any heap object exists at all (§4a) — there's no
  allocated memory to be uninitialized.

To make sure this warning category is actually demonstrated with real, captured Valgrind output
rather than only described, a minimal supplementary program,
`analysis/supplementary/uninit_demo.c`, was written, built, and run under the same Valgrind flags
(full log: `analysis/valgrind_logs/uninit_demo.log`). It is **not** part of the required bundle in
`programs/` and is not built by the Makefile — it exists solely to give this category a concrete
example, since the four assigned programs don't produce one.

```c
int *p = (int *)malloc(sizeof(int)); /* malloc does NOT zero memory */
int local;                            /* automatic local, no initializer */

if (*p == 42)                         /* branches on an uninitialized heap value */
    ...
if (local == 0)                       /* branches on an uninitialized stack value */
    ...
```

Relevant excerpt:
```
==595== Conditional jump or move depends on uninitialised value(s)
==595==    at 0x1091CC: main (uninit_demo.c:9)
==595==  Uninitialised value was created by a heap allocation
==595==    at 0x4846828: malloc (...)
==595==    by 0x1091BE: main (uninit_demo.c:6)
...
==595== Use of uninitialised value of size 8
==595==    at 0x48C60BB: _itoa_word (_itoa.c:183)
==595==    ... by 0x1091FA: main (uninit_demo.c:12)
==595==  Uninitialised value was created by a heap allocation
==595==    at 0x4846828: malloc (...)
==595==    by 0x1091BE: main (uninit_demo.c:6)
...
==595== Conditional jump or move depends on uninitialised value(s)
==595==    at 0x1091FF: main (uninit_demo.c:14)
==595==  Uninitialised value was created by a stack allocation
==595==    at 0x1091A9: main (uninit_demo.c:5)
```
- **Type of error:** use of uninitialized memory, in two forms — a conditional branch on an
  undefined value (`if (*p == 42)`, `if (local == 0)`), and a value of undefined bytes flowing
  into library code (`_itoa_word`, glibc's internal integer-to-string routine used by `printf`'s
  `%d`), each reported separately because Memcheck flags every *use* of tainted bytes, not just
  the first one.
- **Memory object involved:** two distinct objects, both correctly distinguished by
  `--track-origins=yes`: the 4-byte heap block from `malloc(sizeof(int))` at `uninit_demo.c:6`,
  and the automatic stack variable `local` declared at `uninit_demo.c:5`.
- **Lifetime/misuse explanation:** this is neither a lifetime-end bug (nothing was freed or
  scoped out) nor a validity bug (both pointers/variables refer to real, currently-live storage)
  — it is a **definedness** bug. `malloc` reserves space but copies no data into it (unlike
  `calloc`, which zero-fills), so `*p`'s bits are whatever was last in that heap region; `local`
  is never assigned before its first read, so its bits are whatever the previous stack frame at
  that address left behind (per `memory_maps.md` §1, we already established that stack addresses
  get reused across calls — this is the concrete hazard that reuse creates when a *new* frame's
  variable is read before being written). Memcheck detects this via per-byte "shadow" validity
  bits it maintains alongside real memory, independent of what the leftover bit pattern happens to
  be — which is why this class of error is reported reliably even when, by chance, the leftover
  value would have "looked" reasonable.

---

## 7. Summary table

| Program | Report | Classification | Object | Root cause |
|---|---|---|---|---|
| `stack_example` | none | — (control) | — | pure stack usage; its one heap alloc (stdio buffer) is cleanly freed on normal exit |
| `heap_example` | `6 bytes ... definitely lost` | Leak — lost ownership | `alice->name` (6-byte block, `heap_example.c:21`) | `person_free_partial` frees the owning struct without freeing the member it points to first |
| `aliasing_example` | 3× `Invalid read/write of size 4 ... free'd` | Use-after-free | 20-byte `int[5]`, alloc'd `:12`, freed `:38` | alias `b` outlives the block's lifetime; accessed after `free(a)` without being invalidated |
| `crash_example` | `Invalid write ... not stack'd, malloc'd or (recently) free'd` + SIGSEGV | NULL-pointer dereference | `nums` (`NULL`, `allocate_numbers(0)` never calls `malloc`) | write through a pointer that was never bound to a real object, unchecked before use |
| `crash_example` | `4,096 bytes ... still reachable` | Not a leak — abnormal-termination artifact | glibc's internal `stdout` buffer, alloc'd via `puts` at `:27` | process killed by SIGSEGV before normal-exit stdio cleanup runs; a live pointer to the block still exists |
| `uninit_demo` (supplementary) | `Conditional jump ...` / `Use of uninitialised value` | Use of uninitialized memory | heap `int` (`uninit_demo.c:6`) and stack `local` (`uninit_demo.c:5`) | `malloc` doesn't zero memory; an unassigned automatic variable holds leftover bits; both are read/branched-on before being written |

## 8. Relationship to Task 1

Every object identified above — `alice->name`, the aliased `int[5]` array, `nums`, the stdio
buffer — was already named and placed in `analysis/memory_maps.md`'s stack/heap diagrams before
Valgrind was run against it here. Nothing in this document introduces a new mental model of the
programs; it takes the objects and lifetimes already mapped out in Task 1 and shows precisely
which Valgrind diagnostic corresponds to which lifetime event (a free that severs the last
reference, a free that leaves a stale alias behind, a pointer that was never bound to an object,
an object whose owning process died before its cleanup ran). That correspondence — diagnostic
text ⇄ specific memory event — is what makes the reports interpretable instead of just alarming.
