# Example: Contributing a Task from Start to Finish

This document shows a complete example of how a contributor would pick up and complete a task from TASKS.md.

## Scenario

Sarah is a new contributor who wants to help with documentation. She has experience with game engines and wants to contribute to the THUG project.

## Step 1: Browse Available Tasks

Sarah visits [TASKS.md](../TASKS.md) and looks for documentation tasks suitable for beginners.

She finds:

```markdown
#### TASK-DOC-002: Document Game Loop Architecture
**Priority**: High | **Complexity**: Medium | **Effort**: 3-6 hours

Document the frame processing and update cycle.

**Requirements**:
- Explain the main game loop structure
- Document frame timing and synchronization
- Explain update order of subsystems
- Include code examples

**Files to create**:
- `docs/GAME_LOOP.md`
```

This looks perfect for her - it's high priority, medium complexity, and she has the experience to complete it.

## Step 2: Check for Existing Work

Sarah searches GitHub issues for "TASK-DOC-002" and "game loop" to make sure no one else is working on it.

She finds no existing issues or PRs, so the task is available!

## Step 3: Create a GitHub Issue

Sarah goes to GitHub Issues and clicks "New Issue". She selects the "Roadmap Task" template.

She fills out the form:

```yaml
Task ID: TASK-DOC-002
Category: Documentation
Priority: High
Complexity: Medium

Description:
Document the frame processing and update cycle for the THUG engine.

Requirements from TASKS.md:
- Explain the main game loop structure
- Document frame timing and synchronization
- Explain update order of subsystems
- Include code examples

Files to create:
- docs/GAME_LOOP.md

Your Approach:
1. Study the existing game loop code in Code/Gel/
2. Trace the initialization and main loop
3. Document subsystem update order
4. Create diagrams showing the flow
5. Write clear documentation with examples
6. Review with maintainers

Questions:
- Should I include performance optimization notes?
- Preferred diagram format (ASCII art, Mermaid, SVG)?

Estimated Timeline: 1 week
```

She submits the issue. It's assigned issue #42.

## Step 4: Set Up Branch

Sarah opens her terminal:

```bash
# Make sure she's on latest main
cd thug
git checkout main
git pull upstream main

# Create a new branch
git checkout -b docs/task-doc-002-game-loop
```

## Step 5: Do the Work

Sarah spends several hours:

1. **Research**: She reads through the code in `Code/Gel/`, `Code/Core/`, and `Code/Sys/` to understand the game loop
2. **Take notes**: She documents the initialization sequence, main loop, and shutdown
3. **Create diagrams**: She creates a Mermaid diagram showing the game loop flow
4. **Write documentation**: She creates `docs/GAME_LOOP.md` with clear explanations and code references
5. **Add examples**: She includes code snippets showing key functions
6. **Proofread**: She reviews for clarity and accuracy

Her `docs/GAME_LOOP.md` includes:

```markdown
# THUG Game Loop Architecture

## Overview

The THUG engine uses a traditional game loop pattern with fixed timestep for logic
and variable timestep for rendering...

## Initialization Sequence

1. System initialization (Sys::Init())
2. Memory manager setup
3. File system initialization
4. Graphics system initialization
5. Game layer initialization (Gel::Init())
...

## Main Game Loop

[Mermaid diagram here]

## Update Order

Each frame, subsystems are updated in this order:
1. Input processing
2. Script execution
3. Physics simulation
4. Animation updates
5. Rendering
...
```

## Step 6: Test Changes

Sarah checks that her documentation is correct:

```bash
# Verify markdown renders correctly
cat docs/GAME_LOOP.md

# Check for broken links
grep -r "GAME_LOOP.md" docs/

# Verify code references are accurate
# (She manually checks the code paths she referenced)
```

## Step 7: Commit Changes

Sarah commits her work:

```bash
git add docs/GAME_LOOP.md
git commit -m "[TASK-DOC-002] Document game loop architecture

- Explain main game loop structure
- Document frame timing and synchronization
- Explain subsystem update order
- Include code examples and diagrams
- Add performance notes

Fixes #42"
```

## Step 8: Push and Create PR

Sarah pushes her branch:

```bash
git push origin docs/task-doc-002-game-loop
```

Then on GitHub, she creates a Pull Request using the PR template:

```markdown
## Description
Document the game loop architecture for THUG, including initialization, main loop,
update order, and shutdown sequences.

## Task Reference
- Task: TASK-DOC-002 from TASKS.md
- Related Issue: Fixes #42

## Type of Change
- [x] Documentation
- [ ] New feature
- [ ] Bug fix
- [ ] Code cleanup/refactoring
- [ ] Build system
- [ ] Testing
- [ ] Other

## Changes Made
- Created docs/GAME_LOOP.md with comprehensive game loop documentation
- Added Mermaid diagram showing frame processing flow
- Included code examples from actual engine source
- Documented subsystem update order
- Added performance optimization notes

## Requirements Completed
- [x] Explain the main game loop structure
- [x] Document frame timing and synchronization
- [x] Explain update order of subsystems
- [x] Include code examples

## Files Created/Modified
- `docs/GAME_LOOP.md` (created)

## Testing
**Documentation Testing:**
- [x] Documentation renders correctly in GitHub
- [x] Links work properly
- [x] Code references are accurate
- [x] Mermaid diagram displays correctly

## Screenshots
[Screenshot of rendered documentation]

## Checklist
- [x] Documentation follows style guidelines
- [x] Self-reviewed my own changes
- [x] Code examples are valid
- [x] Documentation is clear and comprehensive
- [x] No typos or grammatical errors
- [x] Changes are focused on the task
- [x] Linked related issue #42
- [x] Followed CREATING_PRS_FROM_ROADMAP.md guide

## Additional Notes
I added performance notes based on my experience with similar engines.
Happy to adjust the level of detail based on feedback.
```

## Step 9: Respond to Review

A maintainer reviews the PR and comments:

> "Great work! The documentation is very clear. Could you add a section about
> how the debug mode affects the game loop timing? Also, please link this from
> the main docs/README.md."

Sarah responds:

```markdown
Good catch! I'll add:
1. Debug mode timing section
2. Link from docs/README.md

Will push updates shortly.
```

She makes the requested changes:

```bash
# Edit the documentation
vim docs/GAME_LOOP.md  # Add debug mode section
vim docs/README.md     # Add link to GAME_LOOP.md

# Commit the updates
git add docs/GAME_LOOP.md docs/README.md
git commit -m "Address review feedback

- Add debug mode timing section
- Link from docs/README.md
- Clarify frame interpolation"

# Push updates
git push origin docs/task-doc-002-game-loop
```

She comments on the PR:

> "Updated! Added the debug mode section and linked from docs/README.md. 
> Please let me know if you'd like any other changes."

## Step 10: PR is Merged

The maintainer approves and merges the PR:

> "Perfect! Thanks for the contribution. Merging now."

The PR is merged to main.

## Step 11: Clean Up

Sarah updates her local repository:

```bash
# Switch back to main
git checkout main

# Pull the merged changes
git pull upstream main

# Delete her feature branch (optional)
git branch -d docs/task-doc-002-game-loop

# Delete remote branch (optional)
git push origin --delete docs/task-doc-002-game-loop
```

## Step 12: Celebrate! 🎉

Sarah has successfully:
- Contributed valuable documentation to the project
- Learned about the THUG engine architecture
- Gained experience with the contribution workflow
- Helped future developers understand the codebase

She can now:
- Pick another task from TASKS.md
- Help review other contributors' PRs
- Answer questions from other contributors
- Suggest new tasks for TASKS.md

## What Sarah Did Right

✅ **Checked for existing work** before starting
✅ **Created an issue** to claim the task
✅ **Followed the guidelines** in CREATING_PRS_FROM_ROADMAP.md
✅ **Used proper branch naming** (docs/task-doc-002-game-loop)
✅ **Wrote clear commit messages** with task ID
✅ **Used the PR template** properly
✅ **Responded promptly** to review feedback
✅ **Made requested changes** without pushback
✅ **Stayed focused** on the task requirements

## Tips for Other Contributors

1. **Start small**: Pick a task matching your skill level
2. **Communicate**: Comment on the issue if you have questions
3. **Stay focused**: Complete one task before starting another
4. **Test thoroughly**: Make sure your changes work as expected
5. **Be responsive**: Respond to review feedback promptly
6. **Learn and improve**: Each contribution makes you better
7. **Help others**: Share your experience with new contributors

## More Examples

Want to see more examples?
- Check closed PRs with task IDs
- Look at merged documentation changes
- Ask in community channels
- Review CREATING_PRS_FROM_ROADMAP.md

---

This example shows the complete workflow from start to finish. Your experience
may vary slightly, but the general process is the same for all tasks.

Happy contributing! 🚀
