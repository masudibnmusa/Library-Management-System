# 📚 Library Management System

A comprehensive command-line library management system written in C, featuring user authentication, book tracking, fine calculation, and data persistence.

## ✨ Features

### 🔐 User Management
- **Secure Authentication**: Password hashing for user security
- **Role-Based Access**: Admin and regular user roles with different permissions
- **User Registration**: Self-service account creation for library members
- **Login Protection**: Maximum 3 login attempts with lockout

### 📖 Book Management
- **Add/Remove Books**: Full CRUD operations for book inventory
- **Book Details**: Track title, author, ISBN, publication year
- **ISBN Validation**: Automatic validation for ISBN-10 and ISBN-13
- **Duplicate Prevention**: Checks for existing titles and ISBNs

### 📋 Library Operations
- **Issue Books**: Track who borrowed which books with due dates
- **Return Books**: Process returns with automatic fine calculation
- **Search Functionality**: Search by title, author, or ISBN
- **Sort Options**: Sort books by title or author
- **Detailed View**: View comprehensive information about any book

### 💰 Fine Management
- **Automatic Calculation**: $5.00 per day for overdue books
- **Fine Tracking**: Monitor total pending fines across library
- **Overdue Warnings**: Alerts for overdue books during returns

### 📊 Statistics & Reports
- **Library Statistics**: Total, issued, and available book counts
- **Availability Rate**: Calculate library utilization
- **Export Functionality**: Generate text reports of entire catalog
- **Backup System**: Create timestamped database backups

### 💾 Data Persistence
- **Binary Storage**: Efficient data file format
- **Auto-save**: Prompt to save on logout
- **Crash Recovery**: Signal handling for graceful shutdowns
- **Logging**: Activity logging for auditing and debugging

## 🚀 Getting Started

### Prerequisites
- GCC compiler (or any C compiler)
- Standard C libraries
- Terminal/Command prompt

### Compilation

**Linux/macOS:**
```bash
gcc -o library library_management.c
```

**Windows:**
```bash
gcc -o library.exe library_management.c
```

### Running the Program

**Linux/macOS:**
```bash
./library
```

**Windows:**
```bash
library.exe
```

## 👤 Default Admin Credentials

On first run, the system creates a default admin account:
- **Username**: `admin`
- **Password**: `admin123`

⚠️ **Important**: Change the admin password after first login for security!

## 📖 Usage Guide

### For Regular Users

1. **Register**: Create an account from the main menu
2. **Login**: Use your credentials to access the system
3. **Browse**: View all available books
4. **Search**: Find specific books by title, author, or ISBN
5. **Request**: Contact admin to borrow or return books

### For Administrators

1. **Login**: Use admin credentials
2. **Add Books**: Input book details to expand the collection
3. **Issue Books**: Assign books to borrowers with due dates
4. **Return Books**: Process returns and calculate fines
5. **Manage**: Remove books, view statistics, create backups

## 📁 File Structure

```
project/
│
├── library_management.c    # Main source code
├── library.dat             # Book database (auto-generated)
├── users.dat               # User database (auto-generated)
├── library.log             # Activity log (auto-generated)
└── library_backup_*.dat    # Backup files (created on demand)
```

## 🔧 Configuration

You can modify these constants in the source code:

```c
#define MAX_STR 100              // Maximum string length
#define MAX_BORROWER_NAME 50     // Maximum borrower name length
#define MAX_USERS 100            // Maximum number of users
#define FINE_PER_DAY 5.0         // Fine amount per day
#define MAX_ATTEMPTS 3           // Maximum login attempts
```

## 🛡️ Security Features

- **Password Hashing**: Passwords are never stored in plain text
- **Input Validation**: All user inputs are validated and sanitized
- **Buffer Overflow Protection**: Safe string operations throughout
- **Login Throttling**: Limited login attempts to prevent brute force
- **Activity Logging**: All major operations are logged with timestamps

## 🐛 Known Limitations

- Maximum of 100 users (configurable)
- Single-threaded operation
- No network capabilities (local use only)
- Command-line interface only

## 🤝 Contributing

Contributions are welcome! Please feel free to submit a Pull Request. For major changes:

1. Fork the repository
2. Create your feature branch (`git checkout -b feature/AmazingFeature`)
3. Commit your changes (`git commit -m 'Add some AmazingFeature'`)
4. Push to the branch (`git push origin feature/AmazingFeature`)
5. Open a Pull Request

## 📝 License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## 👨‍💻 Author

Masud Ibn Musa

## 🙏 Acknowledgments

- Inspired by real-world library management needs
- Built for educational purposes and practical use

## 📞 Support

If you encounter any issues or have questions:
- Open an issue on GitHub
- Check the `library.log` file for error details
- Ensure all data files have proper read/write permissions

## 🗺️ Roadmap

Future enhancements planned:
- [ ] GUI version using GTK or Qt
- [ ] Multi-user concurrent access
- [ ] Database integration (SQLite/MySQL)
- [ ] Email notifications for due dates
- [ ] Book reservation system
- [ ] Enhanced reporting with charts
- [ ] Import/Export to CSV/JSON
- [ ] Book categories and tags

---

⭐ If you find this project useful, please consider giving it a star on GitHub!
