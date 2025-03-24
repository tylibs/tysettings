# Contributing to TinySettings

Thank you for your interest in contributing to TinySettings! We welcome contributions
of all kinds, including bug fixes, new features, documentation improvements,
and more.  This document outlines the guidelines we ask contributors to follow.

## Getting Started

1. **Fork the repository:** Start by forking the TinySettings repository on GitHub. This creates your own copy of the project that you can modify.

2. **Clone your fork:** Clone your forked repository to your local machine:

```bash
   git clone https://github.com/tinybus/tinysettings.git
```

3.  **Create a branch:** Create a new branch for your contribution.  Use a descriptive name that reflects the purpose of your changes:

```bash
    git checkout -b feature/new-feature
```

or

```bash
    git checkout -b bugfix/fix-issue-123  # Replace 123 with the issue number
```

## Making Changes

1.  **Follow coding style:**  While a formal style guide isn't yet fully defined, we aim for clean, consistent, and readable C++ code.  Pay attention to indentation, naming conventions, and comments.  We encourage the use of a code formatter like `clang-format`.

2.  **Build and test:**  Ensure your changes compile and pass any existing tests. 

3.  **Document your changes:**  Update the documentation (including README, code comments, and any other relevant files) to reflect your changes.  Clear and concise documentation is crucial for maintainability.

## Submitting Your Contribution

1.  **Commit your changes:** Commit your changes with a clear and concise message.  Use the present tense and keep the first line short (under 50 characters).  Add more detail in the commit body if necessary.

```bash
    git commit -m "feat: support for xyz"
```

2.  **Push your branch:** Push your branch to your forked repository:

```bash
    git push origin feature/new-feature
```

3.  **Create a pull request:**  Go to the TinySettings repository on GitHub and create a pull request from your branch to the `main` branch.  Provide a clear description of your changes and reference any relevant issues.

## Commit Message Format

Format that includes a **type** and a **subject**:

Samples:
```
docs: update changelog to beta.5
```
```
fix: need to depend on latest rxjs and zone.js

The version in our package.json gets copied to the one we publish, and users need the latest of these.
```

### Revert

If the commit reverts a previous commit, it should begin with `revert: `, followed by the header of the reverted commit. In the body it should say: `This reverts commit <hash>.`, where the hash is the SHA of the commit being reverted.

### Type

Must be one of the following:

* **build**: Changes that affect the build system or external dependencies (example scopes: gulp, broccoli, npm)
* **ci**: Changes to our CI configuration files and scripts (example scopes: Travis, Circle, BrowserStack, SauceLabs)
* **docs**: Documentation only changes
* **feat**: A new feature
* **fix**: A bug fix
* **perf**: A code change that improves performance
* **refactor**: A code change that neither fixes a bug nor adds a feature
* **style**: Changes that do not affect the meaning of the code (white-space, formatting, missing semi-colons, etc)
* **test**: Adding missing tests or correcting existing tests

## License

By contributing to TinySettings, you agree that your contributions will be licensed under the [LICENSE](https://github.com/tinybus/tinysettings/blob/master/LICENSE)

## Questions?

If you have any questions or need help, please don't hesitate to open an issue or reach out to us.

Thank you for your contributions!
