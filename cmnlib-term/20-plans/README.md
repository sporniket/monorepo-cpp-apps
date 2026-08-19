---
creation-date: 2026-08-19
copyright-years: 2026
copyright-authors: David SPORN
SPDX-License-Identifier: AGPL-3.0-or-later
since: 0.0.2
---

# Plans of implementation

Some complex Objective/Key Results need multiple simpler tasks to be done in the right order.

Such decompositions and orderings are gathered here.

## Organisation

An OKR is part of milestone, hence a milestone document **WILL** gather all the OKR and their description

E.g., milestone 0.0.2 will have its OKRs described" in a file named `000_000_002.md`

A milestone without complex OKR **MAY** have no plans.

**IMPORTANT NOTE** : the plan file is note a release notes. It's an help to enumerate the tasks to define and there order of implementation.

## Format

An OKR description starts with a level 2 heading title.

It contains : 
* a sorted array (Task ID, Type, Short description/title) listing the tasks (type is EPIC or none/simple).
  * The first item is the main EPIC, holding the OKR. 
  * Then comes sub-EPICs that covers a partial implementation of the main EPIC. _The order of the sub-EPIC is the recommended order of realization._
  * Then come the simple tasks.
* A dependency graph, using the task ids, using the `mermaid.js` syntax, with distinctive shapes for EPICs and simple tasks
 

