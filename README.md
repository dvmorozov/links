# Links - Personal Bookmark Management System

A cross-platform CGI web application for managing bookmarks and organizing links in a hierarchical folder structure. Originally designed for IIS and Apache servers, now fully containerized with Docker support.

## 🌟 Features & Capabilities

Links is a comprehensive personal bookmark management system that combines traditional file-based storage with modern AI-powered organization capabilities.

### Core Bookmark Management

✅ **File System Based Storage** - Bookmarks stored as standard `.url` files in hierarchical folders  
✅ **Web Interface** - Complete CRUD operations (Create, Read, Update, Delete) for bookmarks and folders  
✅ **Cross-Platform** - Runs on Windows (IIS), Linux/macOS (Apache)  
✅ **Containerized Deployment** - Full Docker support with debug/release builds  
✅ **HTTPS Support** - SSL encryption with self-signed certificates  
✅ **User Authentication** - Session-based authentication system  

### User Interface Options

🎨 **Dual UI Modes**:
- **Legacy Interface** - Classic file manager-style interface with table layout
- **Bootstrap Interface** - Modern responsive design with improved styling

🖱️ **Intuitive Navigation**:
- Folder browsing with breadcrumb navigation
- Drag-and-drop-style folder organization
- Quick action buttons for common operations
- Consistent "Continue" button behavior across all dialogs

### Advanced AI-Powered Features

🤖 **Automatic Bookmark Organization**:
- **AI Analysis** - Uses local Ollama/Llama AI to analyze bookmark content
- **Smart Categorization** - Automatically creates logical folder structures based on bookmark topics
- **Content Understanding** - Analyzes titles, URLs, and domains for intelligent grouping
- **Preserve Existing Structure** - Respects current organization while improving it
- **Local AI Processing** - No external services required, complete privacy

🧠 **AI Enhancement Process**:
1. Analyzes bookmark metadata (title, URL, domain)
2. Uses semantic understanding to group related bookmarks
3. Creates topic-based folder hierarchy
4. Moves bookmarks to appropriate categories
5. Provides detailed status feedback

### Import & Export Capabilities

📥 **Firefox Import Support**:
- Import bookmarks from Firefox HTML exports
- Automatic conversion to `.url` file format
- Preserves folder structure during import
- XML processing with TinyXML integration

### Technical Features

🔧 **Robust Architecture**:
- **Storage Abstraction** - Clean separation between UI and file operations
- **Error Handling** - Graceful degradation when services unavailable
- **Cross-Platform HTTP** - Custom HTTP client for AI service communication
- **Comprehensive Logging** - Debug builds include detailed operation logging
- **Environment Detection** - Automatic Docker vs native execution detection

🔒 **Security & Reliability**:
- Session-based authentication
- Secure file handling with permission management
- Error recovery and rollback capabilities
- Input validation and sanitization

### AI Service Integration

🌐 **Local AI Processing**:
- **Ollama Integration** - Uses local Ollama installation for privacy
- **Configurable Models** - Support for various LLM models (default: llama3.2:latest)
- **Automatic Fallback** - Graceful operation when AI unavailable
- **Real-time Feedback** - Progress updates during analysis process

**Prerequisites for AI Features**:
- Local Ollama installation
- Llama model downloaded locally
- Sufficient system resources (4GB+ RAM recommended)

### Debug & Development Features

🔍 **Comprehensive Debug Logging** (Debug builds only):
- Function-level tracing for all auto-foldering operations
- Environment detection and configuration logging
- AI service communication logs (requests/responses)
- File operation tracking and error context
- Progress monitoring and performance metrics

**Debug Log Location**: `/tmp/links_debug.log`

### User Experience Enhancements

✨ **Modern Workflow**:
- One-click AI organization with progress feedback
- Consistent button behavior across success/error scenarios
- Context-preserving navigation (always return to working directory)
- Professional status pages with clear action guidance
- Error messages in English for better accessibility  

## 🚀 Quick Start with Docker (Recommended)

### Prerequisites
- Docker and Docker Compose installed
- 4GB+ available RAM
- **For AI Features**: Local Ollama installation

### Basic Setup (Bookmark Management)

**Windows:**
```cmd
cd Docker\Apache
.\build-and-deploy.bat debug
```

**Linux/macOS:**
```bash
cd Docker/Apache
./build-and-deploy.sh debug
```

### AI-Enhanced Setup (Auto-Organization)

1. **Install Ollama locally:**
   ```bash
   # Install from https://ollama.ai
   ollama pull llama3.2:latest
   ```

2. **Deploy with AI capabilities:**
   ```bash
   # Same Docker deployment as above
   cd Docker/Apache
   ./build-and-deploy.sh debug
   ```

3. **Verify AI connection:**
   - The application automatically detects local Ollama
   - Use "Auto-organize with AI" button to test functionality

### Access the Application

**Debug Version** (includes debugging tools):
```
https://localhost:4433/cgi-bin/links-legacy.exe?;key=;ch_folder=Ok
```

**Release Version** (optimized for production):
```
https://localhost:4444/cgi-bin/links-legacy.exe?;key=;ch_folder=Ok
```

*Note: Accept the self-signed certificate warning in your browser.*

### Using AI Auto-Organization

1. **Navigate** to any bookmark folder
2. **Click** the "Auto-organize with AI" button (🤖 icon)  
3. **Wait** for AI analysis (10-30 seconds)
4. **Review** results and click "Continue" to see organized folders

**Debug Logging** (Debug builds only):
```bash
# View real-time AI operation logs
tail -f /tmp/links_debug.log

# Check for auto-foldering activity
grep "do_auto_folder" /tmp/links_debug.log | tail -5
```
```
https://localhost:4444/cgi-bin/links-legacy.exe?;key=;ch_folder=Ok
```

*Note: Accept the self-signed certificate warning in your browser.*

## 📁 Project Structure

```
links/
├── Sources/                    # Application source code
│   ├── App/                   # Main application entry point
│   ├── Controllers/           # Command processing and URL routing
│   │   └── Commands.cpp       # Includes CMD_AUTO_FOLDER for AI features
│   ├── Models/                # Data access and business logic
│   │   ├── OllamaService.*    # AI service interface for auto-organization
│   │   ├── BookmarkReorganizer.*  # Main AI reorganization logic
│   │   └── FileList.cpp       # File system abstraction layer
│   ├── Views/                 # HTML generation and UI rendering
│   │   ├── AutoFolderStatus.* # AI operation status page
│   │   └── FileListLegacy.cpp # Includes auto-organize button
│   ├── Utils/                 # Utility functions and helpers
│   └── Content/               # CSS, JavaScript, and images
├── Docker/                     # Containerization configurations
│   ├── Apache/                # Apache web server container setup
│   └── LegacyBuilder/         # Build environment container
├── Projects/                   # Build configurations
│   ├── LegacyCGICMake/        # CMake build (Linux/macOS)
│   ├── LegacyCGIWindows/      # Visual Studio project (Windows)
│   └── UnitTestsCatch2/       # Unit tests
└── Documents/                  # Project documentation (now merged into README)
```

### Key Implementation Files

**AI Auto-Organization Components:**
- `OllamaService.h/.cpp` - HTTP client for local Ollama API communication
- `BookmarkReorganizer.h/.cpp` - Orchestrates AI analysis and file operations
- `AutoFolderStatus.h/.cpp` - User feedback page for AI operations
- `Commands.cpp` - Added `CMD_AUTO_FOLDER` command handler

**Core Architecture:**
- Storage abstraction through existing `Data`/`FileList` classes
- Cross-platform HTTP client for AI service communication
- Robust error handling with graceful AI service degradation
- Debug logging system for development and troubleshooting

## 🛠️ Manual Installation

### Apache Setup (Linux/macOS)

1. **Build the application:**
   ```bash
   cd Projects/LegacyCGICMake
   mkdir build && cd build
   cmake .. && make
   ```

2. **Install and configure:**
   ```bash
   # Install CGI binary
   sudo cp links-legacy /usr/lib/cgi-bin/
   sudo chmod +x /usr/lib/cgi-bin/links-legacy
   
   # Create required directories
   sudo mkdir -p /var/www/html/users/links/test
   sudo mkdir -p /var/www/html/temp/links
   sudo mkdir -p /var/www/html/links
   
   # Copy static assets
   sudo cp Sources/Content/Styles.css /var/www/html/links/
   sudo cp Sources/Content/Common.js /var/www/html/links/
   sudo cp -r Sources/Content/legacy-images /var/www/html/links/
   
   # Set permissions
   sudo chown -R www-data:www-data /var/www/html/users/links
   sudo chown -R www-data:www-data /var/www/html/temp/links
   sudo chmod -R 755 /var/www/html/users/links
   sudo chmod -R 755 /var/www/html/temp/links
   ```

3. **Configure HTTPS (self-signed certificate):**
   ```bash
   # Generate certificate
   sudo openssl req -x509 -nodes -days 365 -newkey rsa:2048 \
     -keyout /etc/ssl/private/apache-selfsigned.key \
     -out /etc/ssl/certs/apache-selfsigned.crt
   
   # Enable SSL modules
   sudo a2enmod ssl
   sudo a2ensite default-ssl
   sudo systemctl reload apache2
   ```

4. **Configure Apache for symbolic links:**
   ```bash
   # Edit /etc/apache2/sites-available/default-ssl.conf
   sudo nano /etc/apache2/sites-available/default-ssl.conf
   ```
   
   Add this configuration:
   ```xml
   <Directory /var/www/html/>
       Options +FollowSymLinks
       AllowOverride None
       Require all granted
   </Directory>
   ```

### Windows/IIS Setup

1. **Build with Visual Studio:**
   ```cmd
   cd Projects\LegacyCGIWindows
   # Open LegacyCGIWindows.vcxproj and build in Release mode
   ```

2. **Configure IIS:**
   - Copy `links-legacy.exe` to your CGI directory
   - Create directories under `C:\inetpub\wwwroot\`:
     - `users\links\test\`
     - `temp\links\`
     - `links\`
   - Grant IUSR full access to data and temp directories
   - Copy static assets to `links\` directory

## 🎯 Usage

### Directory Structure

The application uses specific directory locations:

**Data Directory**: `users/links/<user_name>` - Contains bookmark `.url` files
**Temp Directory**: `temp/links` - Session keys and temporary files  
**Content Directory**: `links/` - CSS, JS, and image assets

### Firefox Bookmark Import

1. **Export** bookmarks from Firefox as HTML
2. **Prepare** the file:
   - Remove HTML header
   - Add XML header: `<?xml version="1.0" encoding="UTF-8"?>`
   - Wrap content in `<root>` tags
3. **Install TinyXML** for parsing: `sudo apt install libtinyxml2-dev`
4. **Process** the file using the import functionality

## 🔧 Configuration

### Environment Variables

| Variable | Description | Example |
|----------|-------------|---------|
| `DOCUMENT_ROOT` | Web server document root | `/var/www/html` or `C:\inetpub` |
| `QUERY_STRING` | CGI query parameters | `;key=;ch_folder=Ok` |
| `SCRIPT_NAME` | CGI script path | `/cgi-bin/links-legacy.exe` |

### User Permissions

**Linux/Apache:**
```bash
# Set directory ownership
sudo chown -R <user>:<group> <links_directory>
sudo chmod -R 700 <links_directory>

# Create symbolic link for user data
sudo ln -s <links_directory> /var/www/html/users/links/<user_name>

# Set ACL permissions
sudo setfacl -m u:<user_name>:rwx <links_directory>
```

**Windows/IIS:**
- Grant IUSR read/write access to data and temp directories
- Ensure CGI execution permissions are set

## 🧪 Testing

### Unit Tests
```bash
cd Projects/UnitTestsCatch2
mkdir build && cd build
cmake .. && make && ./unit-tests
```

### Manual Testing
- Copy contents of `Projects/TestData/` to build directory
- Tests use current directory for test files

## 🐛 Troubleshooting

### Common Issues

**Container fails to start:**
```bash
docker ps -a
docker logs links-legacy-apache-debug
```

**Permission errors:**
- Verify directory permissions for web server user
- Check IUSR/www-data access rights
- Ensure temp directory is writable

**SSL certificate warnings:**
- Expected with self-signed certificates
- Click "Advanced" → "Proceed" in browser

### AI Auto-Organization Issues

**"AI service (Ollama) is not available" error:**
```bash
# Check if Ollama is running locally
curl http://localhost:11434/api/version

# Start Ollama if not running
ollama serve

# Verify model is available
ollama list | grep llama3.2
```

**AI analysis fails or returns no suggestions:**
```bash
# Debug mode: Check logs for detailed error context
grep -i "error\|exception" /tmp/links_debug.log | tail -10

# Check AI service connectivity
grep "Testing connection" /tmp/links_debug.log | tail -3

# Verify bookmark analysis attempts
grep "do_auto_folder" /tmp/links_debug.log | tail -5
```

**Performance issues during AI analysis:**
- Ensure system has 4GB+ available RAM
- Local Ollama model may take time to load on first use
- Analysis time scales with number of bookmarks (typically 10-30 seconds)

### Debug Commands
```bash
# Container shell access
docker exec -it links-legacy-apache-debug /bin/bash

# Check Apache logs
docker exec links-legacy-apache-debug tail -f /var/log/apache2/error.log

# Monitor AI operations in real-time (debug builds only)
tail -f /tmp/links_debug.log
```

### Debug vs Release Builds

**Debug Build Features:**
- Comprehensive logging to `/tmp/links_debug.log`
- Function-level tracing for AI operations
- Detailed error context and stack traces
- Performance metrics and progress tracking

**Release Build Features:**
- All logging compiled out (zero performance impact)
- Optimized for production use
- Minimal resource overhead

To enable debug logging, compile in debug mode:
```bash
cmake -DCMAKE_BUILD_TYPE=Debug && make
```

## 📖 Documentation

- **[Docker Setup Guide](Docker/README.md)** - Complete containerization guide
- **Source Code** - See `Sources/` directory for implementation details  
- **Build Systems** - CMake (Linux/macOS) and Visual Studio (Windows)
- **AI Features** - Auto-organization capabilities described in Features section above

### Architecture Overview

**AI Integration Design:**
- Local Ollama service for privacy and offline operation
- Storage abstraction through existing data layer
- Graceful degradation when AI services unavailable
- Cross-platform HTTP client implementation
- Comprehensive error handling and user feedback

---

This project combines traditional bookmark management with modern AI capabilities while maintaining a clean public repository and preserving full development history locally. Use Docker for the easiest setup experience, and add local Ollama for AI-powered auto-organization features.