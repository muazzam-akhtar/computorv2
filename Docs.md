# Computor v2 — Project Plan

Below is a complete sprint plan, task breakdown, README template, and a Notion workspace structure for the **Computor v2** project (42 school — "Your homemade basic calculator" interpreter).

---

## 1. Project Overview & Constraints (from the subject)

| Item | Detail |
|---|---|
| **Goal** | Build a CLI interpreter (`computorv2`) supporting rational numbers, complex numbers (rational coefficients), matrices, functions, and polynomial equations of degree ≤ 2 |
| **Mandatory features** | Type inference & assignment, reassignment across types, variable-to-variable assignment, `?` evaluation, `=` equation solving (deg ≤ 2 on ℝ/ℂ), matrix ops (`**`, `*`, `+`, `-`), powers `^`, modulus `%`, parentheses & priorities, function definition/application, square root solving of deg ≤ 2 polynomials |
| **Constraints** | No native complex type, no complex/matrix libraries (except self-written), Makefile required if compiled language, Norm not applied but code must be clear & verbose |
| **Syntax rules** | Names = letters only, case-insensitive, `i` reserved; `?` resolves; `=` solves when expression contains unknown |
| **Bonus** | Plotting, usual functions (exp, sqrt, abs, cos/sin/tan), radians, composition, norm, `vars` listing, history, matrix inversion, vector extension — **only graded if mandatory is PERFECT** |

### Suggested architecture (independent of language)
```
src/
├── lexer/          # tokenizer: numbers, identifiers, operators, brackets
├── parser/         # recursive-descent / Pratt parser → AST
├── ast/            # node types: Literal, Variable, FuncCall, BinaryOp, Assign, Solve
├── types/          # Rational, Complex, Matrix, Function, Polynomial
├── evaluator/      # type inference, dispatch tables, simplification
├── solver/         # linear & quadratic solver (ℝ and ℂ)
├── context/        # symbol table (case-insensitive), variable storage
└── repl/           # prompt loop, '?', '=', error formatting
```

---

## 2. Sprint Plan (5 sprints × 1 week, adaptable)

### Sprint 0 — Foundations & Setup (2–3 days)
**Goal:** Repo skeleton, toolchain, design decisions, test harness.

| # | Task | Estimate |
|---|---|---|
| S0-1 | Choose language & set up repo structure + Makefile (if compiled) | 2h |
| S0-2 | Write `README.md` (see §4) with build/run instructions | 1h |
| S0-3 | Define core type interfaces (Rational, Complex, Matrix, Function) | 3h |
| S0-4 | Set up test runner (shell scripts comparing expected vs actual output) | 2h |
| S0-5 | Draft grammar / token spec in Notion (see §5) | 2h |
| S0-6 | Collect sample test cases from subject pages 8–11 | 1h |

**Definition of Done:** Repo compiles/links, `make` works, empty REPL prints prompt, test harness runs.

---

### Sprint 1 — Rational Numbers & Expression Evaluator
**Goal:** `2 + 3 * 4`, parentheses, priorities, `%`, `^`, `?` evaluation.

| # | Task | Estimate |
|---|---|---|
| S1-1 | Lexer: integers, identifiers, operators `+ - * / % ^ ( ) ? =` | 3h |
| S1-2 | Pratt/recursive-descent parser with precedence | 4h |
| S1-3 | Rational type (numerator/denominator, normalization, gcd) | 3h |
| S1-4 | Arithmetic on rationals + `%` semantics | 2h |
| S1-5 | Evaluator for pure rational expressions + simplification | 3h |
| S1-6 | Symbol table (case-insensitive, `i` reserved) | 2h |
| S1-7 | REPL loop: display stored value after assignment, `?` resolves | 2h |
| S1-8 | Unit tests: `varA = 2 * 4 + 4`, `a + 2 = ?` etc. | 2h |

**DoD:** All rational examples from subject (pp. 9–11) pass; error messages for invalid input.

---

### Sprint 2 — Complex Numbers & Type Inference / Reassignment
**Goal:** `-4 + 2i`, cross-type reassignment, `y = x` snapshots.

| # | Task | Estimate |
|---|---|---|
| S2-1 | Complex type with rational `a`, `b` (no native complex) | 3h |
| S2-2 | Complex arithmetic `+ - * / ^`, display format `a + bi` | 3h |
| S2-3 | Type promotion rules (Int → Rational → Complex) | 3h |
| S2-4 | Type inference on assignment; reassignment changes type | 3h |
| S2-5 | Variable-to-variable assignment (value copy, not reference) | 2h |
| S2-6 | Functions: `funA(x) = …` parsing, application `funA(2)` | 4h |
| S2-7 | Polynomial simplification (e.g. `(x²+3x+2)/(x+1)` → `x+2`) | 4h |
| S2-8 | Tests: subject p. 8–10 examples | 2h |

**DoD:** Reassignment examples (p. 9) and `funA/funB/funC` examples (p. 10) pass; simplified polynomial display works.

---

### Sprint 3 — Matrices
**Goal:** Matrix literal parsing, all matrix operations.

| # | Task | Estimate |
|---|---|---|
| S3-1 | Matrix parser: `[[1,2];[3,2];[3,4]]`, single row/column rules | 3h |
| S3-2 | Matrix type (n×p rational/complex entries) | 2h |
| S3-3 | Element-wise `+ - *` (scalar ↔ matrix) | 3h |
| S3-4 | Matrix multiplication `**` with dimension checking | 3h |
| S3-5 | Matrix `^` (non-negative integer powers) | 2h |
| S3-6 | Display formatting (aligned columns) | 2h |
| S3-7 | Mixed-type errors (dimension mismatch, division) | 2h |
| S3-8 | Tests from subject p. 11 | 2h |

**DoD:** `matA`, `matB` examples pass; `matA ** matB` correct; dimension errors reported cleanly.

---

### Sprint 4 — Equation Solving & Polish
**Goal:** `=` solving for degree ≤ 2, ℝ/ℂ roots, square-root handling, final polish.

| # | Task | Estimate |
|---|---|---|
| S4-1 | Polynomial collection from AST (single variable, deg ≤ 2) | 4h |
| S4-2 | Linear solver (one root on ℝ) | 2h |
| S4-3 | Quadratic solver (discriminant; roots on ℝ or ℂ) | 4h |
| S4-4 | Output format "Une solution sur R : -1" / "Deux solutions…" | 2h |
| S4-5 | Detect non-polynomial / deg > 2 → clear error | 2h |
| S4-6 | Edge cases: no solution, infinite solutions, `0 = 0` | 2h |
| S4-7 | Full regression suite across all sprints | 3h |
| S4-8 | Code readability pass (comments, design doc) | 2h |

**DoD:** `funA(x) = x^2 + 2x + 1`, `y = 0`, `funA(x) = y ?` → `-1` on ℝ; full subject examples pass end-to-end.

---

### Sprint 5 (Optional / Bonus) — Only if mandatory is PERFECT
| # | Task | Priority |
|---|---|---|
| B-1 | `vars` listing with values | High |
| B-2 | Command history with results | High |
| B-3 | Matrix inversion & norm | Medium |
| B-4 | Extra functions: exp, sqrt, abs, cos, sin, tan (radians) | Medium |
| B-5 | Function composition | Medium |
| B-6 | Function curve display (plotting) | Low |
| B-7 | Vector extension | Low |

---

## 3. Task Board Columns (for Notion)

`Backlog` → `Sprint Ready` → `In Progress` → `In Review` → `Done` → `Blocked`

Each card should include: **ID (Sx-y)**, **Estimate**, **Acceptance criteria**, **Related subject page**, **Test cases**, **Owner**.

---

## 4. README.md Template

```markdown
# Computor v2 — Your Homemade Basic Calculator

An instruction interpreter for advanced computations, inspired by `bc`,
supporting rational numbers, complex numbers (with rational coefficients),
matrices, functions, and polynomial equations of degree ≤ 2.

## Build

```bash
make        # compile & link
make clean  # remove objects
make fclean # remove objects + binary
make re     # rebuild
```

> Only applies if you use a compilable language (C/C++/Rust/…).
> For interpreted languages (Python/JS/…), replace with the run command.

## Run

```bash
./computorv2
```

## Usage

```
> varA = 2 * 4 + 4
12
> varA + 2 = ?
14
> funA(x) = x^2 + 2x + 1
x^2 + 2x + 1
> funA(x) = 0 ?
Une solution sur R :
-1
> matA = [[1,2];[3,2];[3,4]]
[ 1 , 2 ]
[ 3 , 2 ]
[ 3 , 4 ]
```

### Operators
| Op | Meaning |
|---|---|
| `+ - * /` | Basic arithmetic (also element-wise for matrices/scalars) |
| `%` | Modulus |
| `**` | Matrix multiplication |
| `^` | Integer power ≥ 0 |
| `?` | Evaluate/resolve expression |
| `=` | Assignment, or solve when expression contains an unknown |

### Types
- **Rational** — any `x ∈ ℚ`
- **Complex** — `a + ib` with `(a, b) ∈ ℚ²`
- **Matrix** — `A ∈ M_{n,p}(ℚ)`
- **Function** — single-variable, e.g. `funA(x) = 2*x^5 + 4x^2 - 5*x + 4`

### Syntax rules
- Names contain **letters only**, case-insensitive (`varA` ≡ `vara`)
- `i` is reserved
- Expressions are simplified when possible: `(x^2+3x+2)/(x+1)` → `x + 2`
- `2*xx` means `2 * variable_xx` — **not** `2*x^2`

## Design

```
src/
├── lexer/      # tokenizer
├── parser/     # precedence-climbing parser → AST
├── types/      # Rational, Complex, Matrix, Function, Polynomial
├── evaluator/  # type inference & dispatch
├── solver/     # linear/quadratic solver (ℝ and ℂ)
├── context/    # case-insensitive symbol table
└── repl/       # prompt loop & error formatting
```

Key design choices:
- No native complex type used (implemented from scratch, per subject).
- No third-party math libraries; all matrix/complex logic self-written.
- Type promotion: Int → Rational → Complex; matrices handled by dedicated dispatch.
- Simplification performed on the AST before display.

## Testing

```bash
./tests/run.sh   # runs all regression cases
```

Test cases are derived directly from subject pages 8–11 (assignment,
reassignment, matrices, functions, equation solving).

## Bonus (if implemented)

- [ ] `vars` listing
- [ ] Command history
- [ ] Matrix inversion / norm
- [ ] exp, sqrt, abs, cos, sin, tan (radians)
- [ ] Function composition
- [ ] Curve plotting

> ⚠️ Bonus is only evaluated if the mandatory part is perfect.

## Authors

- <login1> — <area>
- <login2> — <area>
```

---

## 5. Notion Workspace Structure

### 5.1 Top-level page: `📐 Computor v2`

**Sections (as sub-pages / databases):**

1. **📋 Subject Summary**
   - Mandatory vs bonus checklist (linked to tasks)
   - Key subject pages mapped to features (p. 8 assignment, p. 9 reassignment, p. 10 solving, p. 11 matrices/syntax)

2. **🏃 Sprint Board** — *Database: "Tasks"*
   - Properties: `ID` (title), `Sprint` (select: S0–S5/Bonus), `Status` (select), `Estimate` (number), `Owner` (person), `Acceptance Criteria` (text), `Subject Ref` (text), `Test Cases` (relation → Test Suite DB)
   - Views: *Board (by Status)*, *Table (by Sprint)*, *Calendar (by due date)*

3. **🧪 Test Suite** — *Database*
   - Properties: `Input`, `Expected Output`, `Feature` (relation → Feature DB), `Passing` (checkbox)
   - Views: *All failing*, *By feature*

4. **🧠 Design Decisions (ADR log)**
   - One page per decision: language choice, parser strategy, type-promotion rules, simplification approach, solver strategy, error message format

5. **📚 Grammar & Semantics Spec**
   - EBNF grammar
   - Operator precedence table
   - Display/simplification rules
   - Error message conventions

6. **🐛 Bug Tracker** — *Database*
   - `Title`, `Severity`, `Repro input`, `Expected`, `Actual`, `Status`, `Linked task`

7. **✅ Definition of Done / Peer-Eval Checklist**
   - All mandatory examples from the subject reproduced exactly
   - `make re` from clean clone works
   - Error cases tested (dimension mismatch, deg > 2, unknown var, reserved `i`)
   - Code readable & commented (Norm not required, but clarity asked by subject)

### 5.2 Notion "Tasks" database — CSV import template

```csv
ID,Sprint,Title,Status,Estimate (h),Owner,Acceptance Criteria,Subject Ref
S0-1,S0,Repo skeleton + Makefile,Backlog,2,,make re builds from clean,General Instructions
S0-2,S0,README template,Backlog,1,,Build/run/usage documented,—
S1-1,S1,Lexer,Backlog,3,,Tokens for all mandatory operators,p.10
S1-3,S1,Rational type,Backlog,3,,Normalized fractions with gcd,p.8
S2-6,S2,Function parsing/application,Backlog,4,,funA(2) evaluates correctly,p.8-10
S3-4,S3,Matrix multiplication **,Backlog,3,,Dimension-checked product,p.10-11
S4-3,S4,Quadratic solver,Backlog,4,,Roots on R or C with correct format,p.10
```

---

## 6. Peer-Evaluation Prep Checklist (Notion page)

- [ ] Mandatory features **all** working — bonus is only graded if this is perfect
- [ ] Every subject example (pp. 8–11) reproduced exactly (including output formatting)
- [ ] Reassignment across types (int → complex) demonstrated live
- [ ] Matrix `**` vs `*` distinction explained at defense
- [ ] Reserved `i` and case-insensitivity demonstrated
- [ ] Simplification examples shown (`(x²+3x+2)/(x+1)` → `x+2`)
- [ ] Solver: deg ≤ 2 on ℝ and ℂ, plus edge cases (no solution, infinite solutions)
- [ ] Code walkthrough ready: clear design, comments, no hidden libraries
- [ ] Git repo clean; only repo contents evaluated

---

### Quick dependency graph (critical path)

```
S0 (setup) → S1 (rational+parser+REPL) → S2 (complex+inference+functions) → S3 (matrices) → S4 (solver+polish) → S5 (bonus)
```

S1-1/Lexer and S1-2/Parser block everything else; S2-6 (functions) blocks S4 (solving); S3 (matrices) is independent after S1 and can be parallelized with S2 if two people work.
