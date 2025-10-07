# Creating Pull Requests from Roadmap Items

This guide explains how to convert roadmap features from [ROADMAP.md](ROADMAP.md) into actionable GitHub issues and pull requests using [TASKS.md](TASKS.md).

## Quick Start

1. **Browse [TASKS.md](TASKS.md)** to find a task that interests you
2. **Create a GitHub issue** (optional but recommended) using the issue template below
3. **Follow the workflow** in [docs/CONTRIBUTING.md](docs/CONTRIBUTING.md)
4. **Submit your PR** using the PR template in CONTRIBUTING.md

## Step-by-Step Process

### Step 1: Choose a Task

Browse [TASKS.md](TASKS.md) and select a task based on:

- **Your skill level** (check the Complexity rating)
- **Time available** (check the Effort estimate)
- **Your interests** (pick something you're excited about)
- **Current needs** (prioritize High priority tasks)

**Good First Tasks for Beginners:**
- Documentation tasks (most TASK-DOC-* items)
- Code quality tasks (TASK-QUALITY-001, 002, 003)
- Simple tool configuration tasks

**Intermediate Tasks:**
- Build system improvements
- Testing infrastructure
- Tool development
- Code refactoring

**Advanced Tasks:**
- Platform abstraction layer
- Graphics backend implementation
- Code modernization
- Complex tool development

### Step 2: Check for Existing Work

Before starting, check if someone is already working on it:

```bash
# Search GitHub issues
# Look for issues with the task ID or similar title

# Search recent PRs
# Check if there's an open PR for this work
```

On GitHub:
1. Go to Issues tab
2. Search for the task ID (e.g., "TASK-DOC-001")
3. Check if there's an existing issue or PR

### Step 3: Create a GitHub Issue (Recommended)

Creating an issue helps:
- Claim the task so others don't duplicate work
- Get feedback before investing time
- Track progress
- Link related discussions

**Issue Template:**

```markdown
# [TASK-XXX-NNN] Task Title

## Task Reference
From TASKS.md: TASK-XXX-NNN

## Description
[Copy the description from TASKS.md]

## Requirements
[Copy the requirements from TASKS.md]

## Files to Create/Modify
[List files from TASKS.md]

## Plan
[Outline your approach to completing this task]

## Questions
[Any questions or clarifications needed]

## Estimated Timeline
[When do you plan to have this completed?]

## Related Issues
[Link to any related issues]
```

**Example Issue:**

```markdown
# [TASK-DOC-001] Create ARCHITECTURE.md

## Task Reference
From TASKS.md: TASK-DOC-001

## Description
Create comprehensive architecture documentation explaining the overall system design.

Priority: High | Complexity: Medium | Effort: 4-8 hours

## Requirements
- Document the game engine pipeline (initialization → game loop → shutdown)
- Create architecture diagrams for major subsystems
- Map data flow between Core, Gel, Gfx, Sk, and Sys layers
- Explain the role of each major subsystem

## Files to Create
- docs/ARCHITECTURE.md

## Plan
1. Study the existing codebase structure
2. Identify major subsystems and their relationships
3. Create architecture diagrams using Mermaid or similar
4. Write comprehensive documentation
5. Get feedback from maintainers

## Questions
- Preferred diagram format? (Mermaid, SVG, PNG?)
- Desired level of detail?
- Should I include code examples?

## Estimated Timeline
Planning to complete within 1 week

## Related Issues
Related to ROADMAP.md Section 1.1
```

### Step 4: Set Up Your Branch

Follow the branching convention:

```bash
# Fork the repository (if you haven't already)
# Clone your fork
git clone https://github.com/YOUR_USERNAME/thug.git
cd thug

# Add upstream remote
git remote add upstream https://github.com/kamer1337/thug.git

# Create a branch
# Use format: category/task-id-brief-description
git checkout -b docs/task-doc-001-architecture-md
# or
git checkout -b feature/task-build-001-modular-cmake
# or
git checkout -b fix/task-quality-005-header-guards
```

**Branch Naming Convention:**

- `docs/task-xxx-yyy` - Documentation tasks
- `feature/task-xxx-yyy` - New features
- `fix/task-xxx-yyy` - Bug fixes or improvements
- `test/task-xxx-yyy` - Testing tasks
- `build/task-xxx-yyy` - Build system tasks
- `refactor/task-xxx-yyy` - Code refactoring

### Step 5: Do the Work

Follow the requirements from TASKS.md:

1. **Make minimal, focused changes**
   - Stick to the task requirements
   - Don't mix multiple tasks in one PR
   - Keep commits logical and atomic

2. **Follow project standards**
   - Read [docs/CODE_STYLE.md](docs/CODE_STYLE.md)
   - Follow existing conventions
   - Preserve historical accuracy when modifying code

3. **Document as you go**
   - Update relevant documentation
   - Add code comments where helpful
   - Explain complex decisions

4. **Test your changes**
   - Build the project
   - Run existing tests
   - Test your new functionality
   - Verify documentation renders correctly

### Step 6: Commit Your Changes

Write clear commit messages:

```bash
git add .
git commit -m "[TASK-DOC-001] Create ARCHITECTURE.md

- Document game engine pipeline
- Add architecture diagrams
- Explain subsystem relationships
- Map data flow between layers

Related to #123"
```

**Commit Message Format:**

```
[TASK-ID] Brief summary (50 chars or less)

- Detailed bullet points
- Explaining what changed
- And why it changed
- One change per line

Related to #issue-number
Fixes #issue-number (if applicable)
```

### Step 7: Push and Create PR

```bash
# Push to your fork
git push origin docs/task-doc-001-architecture-md
```

Then on GitHub:
1. Go to your fork
2. Click "Compare & pull request"
3. Fill out the PR template (see below)
4. Link the related issue
5. Submit the PR

**Pull Request Template:**

```markdown
## [TASK-XXX-NNN] Task Title

### Description
Brief description of what this PR does.

### Task Reference
- Task: TASK-XXX-NNN from TASKS.md
- Related Issue: #issue-number

### Type of Change
- [ ] Documentation
- [ ] New feature
- [ ] Bug fix
- [ ] Code cleanup/refactoring
- [ ] Build system
- [ ] Testing
- [ ] Other (specify)

### Changes Made
- Specific change 1
- Specific change 2
- Specific change 3

### Requirements Completed
- [x] Requirement 1 from TASKS.md
- [x] Requirement 2 from TASKS.md
- [ ] Optional requirement 3

### Files Created/Modified
- `path/to/new/file.md` (created)
- `path/to/modified/file.cpp` (modified)

### Testing
How were these changes tested?
- Built successfully on [platform]
- Tested [specific functionality]
- Verified documentation renders correctly

### Screenshots (if applicable)
[Add screenshots for UI/documentation changes]

### Checklist
- [ ] Code/docs follow style guidelines
- [ ] Self-reviewed changes
- [ ] Documentation updated (if needed)
- [ ] No new warnings
- [ ] Changes are minimal and focused
- [ ] Linked related issue
- [ ] Tests pass (if applicable)

### Additional Notes
Any other context or information reviewers should know.
```

## Multiple Tasks in Sequence

If you want to work on multiple related tasks:

1. **Complete one task fully** before starting another
2. **Wait for PR to be merged** before starting dependent task
3. **Or create separate branches** for independent tasks

Example workflow:
```bash
# Task 1
git checkout -b docs/task-doc-001-architecture
# ... do work ...
git push origin docs/task-doc-001-architecture
# ... create PR, wait for review ...

# Task 2 (independent)
git checkout main
git pull upstream main
git checkout -b docs/task-doc-002-game-loop
# ... do work ...
```

## Task Dependencies

Some tasks depend on others. Check TASKS.md for:

- "Related" mentions
- Prerequisite requirements
- Build system dependencies

**Example Dependencies:**
- Testing tasks require test framework (TASK-TEST-001)
- Platform tasks may need HAL design (TASK-MODERN-001)
- Tool tasks may need format documentation first

## Getting Help

If you're stuck or have questions:

1. **Check Documentation**
   - [ROADMAP.md](ROADMAP.md) - Overall vision
   - [TASKS.md](TASKS.md) - Task details
   - [docs/CONTRIBUTING.md](docs/CONTRIBUTING.md) - Workflow
   - [docs/CODE_STYLE.md](docs/CODE_STYLE.md) - Style guide

2. **Search Issues/PRs**
   - Someone might have asked the same question
   - Related PRs might have useful context

3. **Ask in Your Issue**
   - Comment on the issue you created
   - Ask specific questions
   - Maintainers will respond

4. **Community Channels**
   - [Coming soon - Discord/Discussion forum]
   - GitHub Discussions (if enabled)

## Review Process

After submitting your PR:

1. **Automated Checks** (if configured)
   - Build verification
   - Code formatting
   - Static analysis

2. **Maintainer Review**
   - Code/documentation quality
   - Adherence to task requirements
   - Style compliance

3. **Feedback Loop**
   - Respond to comments
   - Make requested changes
   - Ask for clarification if needed
   - Update your PR branch

4. **Approval and Merge**
   - Maintainer approves
   - PR is merged to main branch
   - Your branch can be deleted

## After Your PR is Merged

Congratulations! 🎉

```bash
# Update your local main branch
git checkout main
git pull upstream main

# Delete your feature branch (optional)
git branch -d docs/task-doc-001-architecture

# Delete remote branch (optional)
git push origin --delete docs/task-doc-001-architecture
```

## Tips for Success

### Documentation Tasks
- **Read existing docs** to match style and tone
- **Use clear headings** and structure
- **Add diagrams** where they help understanding
- **Include examples** when documenting APIs
- **Proofread carefully** before submitting

### Code Tasks
- **Understand the context** before changing code
- **Preserve history** - don't remove working code unnecessarily
- **Test thoroughly** on your platform
- **Follow existing patterns** in the codebase
- **Comment complex logic** to help future readers

### Build System Tasks
- **Test on multiple platforms** if possible
- **Document new build options** clearly
- **Maintain backwards compatibility** when possible
- **Provide clear error messages** for build failures

### Tool Development
- **Add usage documentation** for all tools
- **Include examples** of tool usage
- **Handle errors gracefully** with helpful messages
- **Make tools configurable** with command-line options
- **Write tool documentation** in tools/README.md

## Common Pitfalls to Avoid

1. **Scope Creep**
   - Stick to the task requirements
   - Don't fix unrelated issues
   - Create separate PRs for separate concerns

2. **Breaking Changes**
   - Avoid breaking existing functionality
   - Test thoroughly before submitting
   - Document any necessary breaking changes

3. **Unclear Communication**
   - Write clear commit messages
   - Explain your reasoning in PR description
   - Respond to review comments promptly

4. **Incomplete Work**
   - Complete all requirements from TASKS.md
   - Update relevant documentation
   - Test your changes

5. **Style Inconsistency**
   - Follow existing code style
   - Use consistent naming
   - Match documentation tone

## Example Workflow

Here's a complete example workflow:

```bash
# 1. Pick task TASK-DOC-001 from TASKS.md
# 2. Create GitHub issue #42

# 3. Set up branch
git checkout main
git pull upstream main
git checkout -b docs/task-doc-001-architecture

# 4. Do the work
# ... create docs/ARCHITECTURE.md ...
# ... add diagrams ...
# ... test locally ...

# 5. Commit
git add docs/ARCHITECTURE.md
git add docs/diagrams/architecture.svg
git commit -m "[TASK-DOC-001] Create ARCHITECTURE.md

- Document game engine pipeline
- Add architecture diagram
- Explain subsystem relationships
- Map data flow between layers

Fixes #42"

# 6. Push and create PR
git push origin docs/task-doc-001-architecture
# ... create PR on GitHub ...

# 7. Respond to review
# ... make requested changes ...
git add docs/ARCHITECTURE.md
git commit -m "Address review feedback

- Clarify initialization sequence
- Add more details to Gel layer
- Fix typos"
git push origin docs/task-doc-001-architecture

# 8. After merge
git checkout main
git pull upstream main
git branch -d docs/task-doc-001-architecture
```

## Task Status Tracking

Track your progress:

- [ ] Task selected from TASKS.md
- [ ] Checked for existing work
- [ ] Created GitHub issue (if needed)
- [ ] Set up branch
- [ ] Completed requirements
- [ ] Tested changes
- [ ] Committed with clear message
- [ ] Created pull request
- [ ] Responded to review
- [ ] PR merged

## Updating TASKS.md

If you find that:
- Task requirements are unclear
- Estimated effort is wrong
- Dependencies are missing
- New tasks are needed

Please:
1. Create an issue describing the problem
2. Suggest improvements
3. Maintainers will update TASKS.md

## Questions?

If this guide doesn't answer your question:
- Check [docs/CONTRIBUTING.md](docs/CONTRIBUTING.md)
- Search existing issues
- Ask in the community channel
- Create a discussion thread

---

**Happy Contributing!** 🚀

Remember: Every contribution helps, no matter how small. Documentation improvements, bug reports, and thoughtful questions are all valuable contributions to the project.
