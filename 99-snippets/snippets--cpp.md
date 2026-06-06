---
creation-date: 2026-06-06
copyright-years: 2026
copyright-authors: David SPORN
SPDX-License-Identifier: AGPL-3.0-or-later
---

# C++

A collection of C++ blocks.

_Naming scheme : `'cpp'+suffix`, e.g. `cppclsrul5`, `cppclsrul5_unmvuncp`._

## cppdoxcmplx

Doxygen block when the content will be complex (text wall + snippets).

```cpp
/************************************************
Brief.

Your doc.
************************************************/
```

## cppclsrul5

Template of a class that needs to define the copy and move operations. The template is
taylored for a base polymorphic class (with virtual destructor), if more complex, one
will have to change the `= default` by a custom implementation.

```cpp
class @@@{v:cname} {
  public:
    // TODO constructors
    // TODO destructor

    /**
     * Copy operation (rule of 5).
     */
    @@@{v:cname}(const @@@{v:cname}&) = default;

    /**
     * Copy operator (rule of 5).
     */
    @@@{v:cname}& operator=(const @@@{v:cname}&) = default;

    /**
     * Move operation (rule of 5).
     */
    @@@{v:cname}(@@@{v:cname}&&) = default;

    /**
     * Move operator (rule of 5).
     */
    @@@{v:cname}& operator=(@@@{v:cname}&&) = default;

  private:
};
```

### cppclsrul5_unmvuncp

Template of a class that is not movabe nor copyable.

```cpp
class @@@{v:cname} {
  public:
    // TODO constructors
    // TODO destructor

    /**
     * Deleted copy operation, this class is not copyable.
     */
    @@@{v:cname}(const @@@{v:cname}&) = delete;

    /**
     * Deleted copy operator, this class is not copyable.
     */
    @@@{v:cname}& operator=(const @@@{v:cname}&) = delete;

    /**
     * Deleted move operation, this class is not movable.
     */
    @@@{v:cname}(@@@{v:cname}&&) = delete;

    /**
     * Deleted move operator, this class is not movable.
     */
    @@@{v:cname}& operator=(@@@{v:cname}&&) = delete;

  private:
};
```

