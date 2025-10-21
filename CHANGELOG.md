# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [4.0.0] - 2025-01-XX

### Added
- **User Authentication System**
  - Password hashing for secure storage
  - Role-based access control (Admin/User)
  - User registration functionality
  - Login attempt limiting (3 attempts max)
  
- **Enhanced Security Features**
  - Signal handlers for graceful shutdown (SIGINT, SIGTERM)
  - Activity logging with timestamps
  - Input validation and sanitization
  - Buffer overflow protection
  
- **Fine Management System**
  - Automatic fine calculation ($5 per day overdue)
  - Fine tracking in statistics
  - Overdue warnings on book return
  
- **Advanced Book Management**
  - ISBN validation (ISBN-10 and ISBN-13)
  - Duplicate prevention (title and ISBN)
  - Book sorting (by title or author)
  - Enhanced search with case-insensitive matching
  
- **Data Management**
  - Database backup functionality
  - Export to text file
  - Versioned file format
  - User data persistence
  
- **UI Improvements**
  - Cross-platform screen clearing
  - Pause/continue prompts
  - Detailed book information display
  - Improved menu organization
  - Save confirmation on logout

### Changed
- Restructured menu system for better UX
- Improved error messages and user feedback
- Enhanced file format with metadata header
- Better memory management and cleanup
- Optimized merge sort implementation

### Fixed
- Memory leaks in book management
- Input buffer handling issues
- File permission handling
- Date/time formatting in exports
- ISBN validation edge cases

## [3.0.0] - Previous Version

### Added
- Basic book issue and return system
- File-based data persistence
- Search functionality
- Library statistics

### Changed
- Improved data structure efficiency
- Enhanced user interface

## [2.0.0] - Previous Version

### Added
- Linked list implementation for book storage
- CRUD operations for books
- Basic file operations

## [1.0.0] - Initial Release

### Added
- Basic book management
- Simple console interface
- In-memory storage

---

## Version History Summary

- **v4.0** - Full-featured system with security & user management
- **v3.0** - Issue/return system with persistence
- **v2.0** - Data structures and file operations
- **v1.0** - Initial prototype

## Upgrading

### From v3.0 to v4.0
- User database will be created automatically
- Default admin account (admin/admin123) will be available
- Existing book data will be migrated automatically
- **Action Required**: Change default admin password after first login

### From v2.0 to v3.0
- Book data format changed - manual migration may be needed
- Backup your `library.dat` file before upgrading

## Future Releases

### Planned for v5.0
- [ ] GUI implementation
- [ ] Database backend support
- [ ] Multi-user concurrent access
- [ ] Email notification system
- [ ] Book reservation feature
- [ ] Advanced reporting with charts

### Under Consideration
- Mobile app version
- Web interface
- API for third-party integrations
- Barcode scanning support
- Integration with online book databases
