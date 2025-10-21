# Contributing to Library Management System

First off, thank you for considering contributing to this project! It's people like you that make this library management system better for everyone.

## Code of Conduct

This project and everyone participating in it is governed by our commitment to providing a welcoming and inspiring community for all. Please be respectful and constructive in your communications.

## How Can I Contribute?

### Reporting Bugs

Before creating bug reports, please check the existing issues to avoid duplicates. When you create a bug report, include as many details as possible:

* **Use a clear and descriptive title**
* **Describe the exact steps to reproduce the problem**
* **Provide specific examples** including sample inputs
* **Describe the behavior you observed** and what you expected
* **Include screenshots** if relevant
* **Include your environment details** (OS, compiler version, etc.)

### Suggesting Enhancements

Enhancement suggestions are tracked as GitHub issues. When creating an enhancement suggestion, include:

* **Use a clear and descriptive title**
* **Provide a detailed description** of the suggested enhancement
* **Explain why this enhancement would be useful**
* **List any similar features** in other applications if applicable

### Pull Requests

1. **Fork the repository** and create your branch from `main`
2. **Follow the coding style** used throughout the project
3. **Test your changes** thoroughly
4. **Update documentation** if you're adding/changing functionality
5. **Write clear commit messages**
6. **Submit your pull request**

## Development Guidelines

### Coding Standards

* Use **K&R style** indentation with 4 spaces
* Keep functions **focused and single-purpose**
* Add **comments** for complex logic
* Use **meaningful variable names**
* **Validate all user inputs**
* Always use **safe string operations** (safe_strcpy, etc.)
* Check **return values** from functions like malloc, fopen, etc.

### Example Code Style

```c
// Good: Clear, safe, documented
void addBook() {
    if (current_user == NULL || !current_user->is_admin) {
        printf("Error: Only administrators can add books!\n");
        return;
    }

    char title[MAX_STR];
    printf("Enter book title: ");
    fgets(title, MAX_STR, stdin);
    title[strcspn(title, "\n")] = 0;

    if (strlen(title) == 0) {
        printf("Title cannot be empty!\n");
        return;
    }
    
    // ... rest of implementation
}
```

### Security Considerations

When contributing, please ensure:

* **Never store passwords in plain text**
* **Validate all inputs** before processing
* **Use buffer-safe functions** (strncpy, snprintf, etc.)
* **Check array bounds** before accessing
* **Free allocated memory** to prevent leaks
* **Handle file operations** with proper error checking

### Testing Your Changes

Before submitting a pull request:

1. **Compile without warnings**
   ```bash
   gcc -Wall -Wextra -o library library_management.c
   ```

2. **Test all functionality**:
   - User registration and login
   - Adding, removing, searching books
   - Issuing and returning books
   - File operations (save, load, export)
   - Edge cases (empty inputs, invalid data, etc.)

3. **Test on different platforms** if possible:
   - Linux
   - macOS
   - Windows

### Commit Message Guidelines

* Use the present tense ("Add feature" not "Added feature")
* Use the imperative mood ("Move cursor to..." not "Moves cursor to...")
* Limit the first line to 72 characters or less
* Reference issues and pull requests liberally after the first line

Example:
```
Add ISBN-13 validation for book entries

- Implement checksum calculation
- Add digit validation
- Update error messages for clarity

Fixes #123
```

## Project Structure

```
library_management.c
├── Includes & Defines
├── Type Definitions (Book, User, LibraryStats)
├── Global Variables
├── Function Prototypes
├── Main Function
├── Menu Functions
├── Authentication Functions
├── Book Management Functions
├── Utility Functions
└── Sorting & Search Functions
```

## Feature Requests Wishlist

Areas where contributions are especially welcome:

### High Priority
- [ ] Unit tests for core functions
- [ ] Memory leak detection and fixes
- [ ] Cross-platform compatibility improvements
- [ ] Enhanced error handling

### Medium Priority
- [ ] CSV import/export functionality
- [ ] Book categories/genres
- [ ] Multiple copies of same book
- [ ] User borrowing history

### Low Priority
- [ ] GUI version
- [ ] Network/multi-user support
- [ ] Database integration
- [ ] Email notifications

## Questions?

Feel free to open an issue with the "question" label if you have any questions about contributing!

## Recognition

Contributors will be acknowledged in the README.md file. Thank you for helping make this project better!

---

Happy coding! 🚀
