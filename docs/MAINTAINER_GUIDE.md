# Maintainer Guide: Managing Roadmap Tasks

This guide is for project maintainers managing the task-to-PR workflow.

## Overview

The task system consists of:
- **TASKS.md**: 79 discrete tasks extracted from ROADMAP.md
- **GitHub Templates**: Issue and PR templates for structured submissions
- **Documentation**: Guides for contributors

## Daily Maintenance

### Reviewing New Issues

When a contributor creates a "Roadmap Task" issue:

1. **Verify the task exists** in TASKS.md
2. **Check for duplicates** - search for other issues with same task ID
3. **Assess contributor fit** - do they seem capable?
4. **Provide feedback** if requirements are unclear
5. **Assign labels**: `documentation`, `enhancement`, `good first issue`, etc.
6. **Assign milestone** if applicable

**Example labels to add:**
- `task-doc-*` for documentation tasks
- `task-build-*` for build system tasks
- `good first issue` for beginner-friendly tasks
- `help wanted` if task needs a volunteer
- `priority-high` for urgent tasks

### Reviewing Pull Requests

When a PR is submitted for a task:

1. **Check task reference** - does PR link to issue and task ID?
2. **Verify requirements** - are all requirements from TASKS.md met?
3. **Check scope** - is it focused on the task or does it drift?
4. **Review quality** - does it follow CODE_STYLE.md?
5. **Test if needed** - build, run tests, verify functionality
6. **Request changes** or **approve**

**PR Review Checklist:**
- [ ] References correct task ID (e.g., TASK-DOC-001)
- [ ] Links related issue (e.g., Fixes #42)
- [ ] All requirements completed
- [ ] Documentation updated (if applicable)
- [ ] Tests pass (if applicable)
- [ ] Code follows style guidelines
- [ ] Changes are minimal and focused
- [ ] Commit messages are clear

### Merging and Closing

After approving a PR:

1. **Merge the PR** (squash or regular merge based on project policy)
2. **Close linked issue** (GitHub should do this automatically with "Fixes #42")
3. **Update TASKS.md** - mark task as completed (optional)
4. **Update ROADMAP.md** - mark corresponding item complete (if applicable)
5. **Thank the contributor** - acknowledge their work

## Weekly Maintenance

### Task Status Updates

Once a week:

1. **Review open task issues**
   - Close stale issues (no activity for 30+ days)
   - Ask for status updates on long-running tasks
   - Offer help to stuck contributors

2. **Update TASKS.md** (optional)
   - Mark completed tasks
   - Add new tasks from ROADMAP.md updates
   - Adjust priorities based on project needs

3. **Check for blocked tasks**
   - Identify tasks waiting on dependencies
   - Update TASKS.md with dependency information
   - Communicate blockers to interested contributors

### Documentation Review

1. **Review new documentation PRs**
   - Ensure accuracy
   - Check for consistency with existing docs
   - Verify links work

2. **Update guides if needed**
   - CREATING_PRS_FROM_ROADMAP.md
   - EXAMPLE_WORKFLOW.md
   - CONTRIBUTING.md

## Monthly Maintenance

### Strategic Review

Once a month:

1. **Review task distribution**
   - Which categories have most activity?
   - Which tasks are consistently skipped?
   - Are priorities still accurate?

2. **Update priorities**
   - Adjust based on project direction
   - Move completed tasks to archive
   - Add newly identified tasks

3. **Contributor recognition**
   - Thank active contributors
   - Highlight contributions in release notes
   - Consider adding CONTRIBUTORS.md

### System Improvements

1. **Template updates**
   - Are templates working well?
   - Do contributors fill them out completely?
   - What questions are commonly asked?

2. **Documentation improvements**
   - Based on contributor questions
   - Common issues or confusion
   - Missing information

## Managing Task Completion

### Marking Tasks Complete in TASKS.md

When a task is completed, you can update TASKS.md:

```markdown
#### TASK-DOC-001: Create ARCHITECTURE.md
**Status**: ✓ Complete (PR #123)
**Priority**: High | **Complexity**: Medium | **Effort**: 4-8 hours
```

Or move to a "Completed Tasks" section at the end of TASKS.md.

### Updating ROADMAP.md

Update the corresponding item in ROADMAP.md:

```markdown
- [x] Document the game engine pipeline (TASK-DOC-001)
```

## Handling Common Scenarios

### Scenario 1: Duplicate Task Claims

Two contributors claim the same task:

1. Check timestamps - who claimed first?
2. If close, ask if they want to collaborate
3. Suggest first claimant continues
4. Suggest second contributor pick related task
5. Update issue to reflect decision

### Scenario 2: Stalled Task

Contributor claimed task but no progress:

1. Comment asking for status update
2. If no response after 7 days, comment again
3. If no response after 14 days, unassign and close issue
4. Mark task as available again
5. Thank contributor for initial interest

### Scenario 3: Task Needs Splitting

Task is too large or complex:

1. Comment on issue with suggestion to split
2. Create TASK-XXX-NNN-A, TASK-XXX-NNN-B subtasks
3. Update TASKS.md with subtasks
4. Ask contributor which part they want to tackle
5. Document dependencies between subtasks

### Scenario 4: Task No Longer Relevant

1. Comment explaining why task is no longer needed
2. Close related issues
3. Update TASKS.md to mark task as deprecated
4. Remove from future task lists
5. Update ROADMAP.md if needed

### Scenario 5: Poor Quality PR

PR doesn't meet standards:

1. Leave constructive feedback
2. Request specific changes
3. If contributor doesn't respond, give reasonable time
4. If changes not made, consider:
   - Closing PR and reopening task
   - Making changes yourself if minor
   - Finding another contributor

## Task Lifecycle

```
[Task in TASKS.md]
       ↓
[Issue created by contributor]
       ↓
[Maintainer reviews & approves]
       ↓
[Contributor works on task]
       ↓
[PR submitted]
       ↓
[Maintainer reviews PR]
       ↓
[Changes requested] → [Contributor updates] → [Back to review]
       ↓
[PR approved]
       ↓
[PR merged]
       ↓
[Issue closed]
       ↓
[Task marked complete]
```

## Labels to Use

Suggested labels for task issues and PRs:

**By Category:**
- `task-documentation`
- `task-build-system`
- `task-code-quality`
- `task-testing`
- `task-modernization`
- `task-tooling`
- `task-platform`
- `task-advanced`
- `task-community`

**By Status:**
- `good first issue` - Beginner-friendly
- `help wanted` - Needs volunteer
- `in progress` - Being worked on
- `blocked` - Waiting on dependencies
- `needs review` - PR ready for review

**By Priority:**
- `priority-critical`
- `priority-high`
- `priority-medium`
- `priority-low`

## Communication Guidelines

### With Contributors

**Do:**
- Be welcoming and encouraging
- Provide constructive feedback
- Acknowledge their effort
- Be patient with beginners
- Explain decisions clearly

**Don't:**
- Be dismissive or harsh
- Ignore questions
- Make assumptions about skill level
- Rush contributors
- Merge without proper review

### In PR Reviews

**Good review comment:**
> "Great work on the documentation! Could you add a section about X? 
> Also, please update the link in docs/README.md to point to this new file."

**Better than:**
> "Missing X. Fix links."

## Metrics to Track

Optional metrics for project health:

1. **Task completion rate** - Tasks completed per month
2. **Time to completion** - Average time from issue to PR merge
3. **Contributor count** - Unique contributors working on tasks
4. **Task categories** - Which categories are most/least active
5. **PR quality** - Revisions needed before merge

## Tools and Automation

Consider setting up:

1. **GitHub Actions** - Auto-label PRs based on task ID
2. **Project boards** - Visualize task progress
3. **Issue templates** - Already done! ✓
4. **Stale bot** - Auto-close inactive issues
5. **PR size labeler** - Flag large PRs for extra review

## Getting Help

If you're overwhelmed:

1. **Recruit co-maintainers** - Share the load
2. **Ask for help** - Post in community channels
3. **Prioritize ruthlessly** - Focus on critical tasks
4. **Delegate** - Let experienced contributors review PRs
5. **Take breaks** - Maintainer burnout is real

## Resources

- [TASKS.md](../TASKS.md) - Task catalog
- [CREATING_PRS_FROM_ROADMAP.md](CREATING_PRS_FROM_ROADMAP.md) - Contributor guide
- [EXAMPLE_WORKFLOW.md](EXAMPLE_WORKFLOW.md) - Example workflow
- [CONTRIBUTING.md](CONTRIBUTING.md) - Contribution guidelines
- [ROADMAP.md](../ROADMAP.md) - Original roadmap

## Questions?

If you have questions about managing the task system:
1. Review this guide
2. Check GitHub documentation on issues/PRs
3. Ask other maintainers
4. Update this guide with answers!

---

**Remember**: The goal is to make contributing easy and rewarding. Be welcoming,
be clear, and recognize good work. Every contribution helps! 🚀
