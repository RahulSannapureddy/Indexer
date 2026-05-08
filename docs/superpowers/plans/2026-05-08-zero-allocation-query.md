# Zero-Allocation Query Normalization Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Eliminate heap allocations during query normalization and tokenization in the C++ Ranker.

**Architecture:** Replace `std::istringstream` and temporary `std::string` objects in `Ranker::rank` with manual `std::string_view` split logic. This avoids heap allocations on the hot path.

**Tech Stack:** C++17

**Commit Policy:** Meticulous git commits for every small correct milestone. Commit messages must be human-looking, short, and very brief (e.g., "refactor: use string_view in ranker", "fix: remove stringstream allocation").

---

### Task 1: Refactor Ranker for Zero Allocation

**Files:**
- Modify: `src/engine_cpp/ranking.h`
- Modify: `src/engine_cpp/ranking.cpp`

- [ ] **Step 1: Update Ranker.h signatures**
Change `normalize_query` to return a `std::vector<std::string_view>` or process tokens directly to avoid `std::string` creation.

- [ ] **Step 2: Implement zero-allocation tokenization in ranking.cpp**
Replace `std::istringstream` in `Ranker::rank` with a custom loop using `std::string_view::find_first_not_of` and `std::string_view::find_first_of`.

- [ ] **Step 3: Remove stringstream from normalize_query**
Refactor `normalize_query` to either work in-place or return a temporary container of views.

- [ ] **Step 4: Verify correctness with search test**
Build the engine and verify it still returns correct results for a sample query.

- [ ] **Step 5: Verify "Zero Allocation" (Conceptual)**
Ensure no `new`, `std::string` (on hot path), or `std::istringstream` remains in the ranking logic.
