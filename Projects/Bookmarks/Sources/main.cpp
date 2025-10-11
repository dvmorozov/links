#include <algorithm>
#include <iostream>
#include <libxml/HTMLparser.h>
#include <libxml/tree.h>
#include <sstream>
#include <sys/stat.h>
#include <sys/types.h>
#include <string>
#include <vector>

#include "Utils/Miscellaneous.h"

const int cutPathPartNumber = 2;
const std::string pathPrefix = "/home/dmitry/VSProjects/Bookmarks/Links";
const std::string dataFilePath = "/home/dmitry/VSProjects/Bookmarks/bin/x64/Debug/bookmarks.html";

// Function to concatenate path prefix with relative path
std::string concatenatePaths(const std::string& pathPrefix, const std::string& relativePath) {
    if (pathPrefix.empty()) {
        return relativePath;
    }
    if (relativePath.empty()) {
        return pathPrefix;
    }

    std::string result = pathPrefix;
    // Ensure the path prefix ends with a path separator
    if (result.back() != '/') {
        result += '/';
    }
    // Remove leading path separator from the relative path if present
    if (relativePath.front() == '/') {
        result += relativePath.substr(1);
    }
    else {
        result += relativePath;
    }

    return result;
}

void getLinkAttributes(xmlNode* element, std::string& url, std::string& linkName,
                       std::string& addDate, std::string& lastModifiedDate) {
    xmlChar* addDate_ = xmlGetProp(element, BAD_CAST "add_date");
    xmlChar* lastModified = xmlGetProp(element, BAD_CAST "last_modified");
    xmlChar* text = xmlNodeGetContent(element);
    xmlChar* href = xmlGetProp(element, BAD_CAST "href");

    url = "";
    linkName = "";
    addDate = "";
    lastModifiedDate = "";

    if (text) {
        linkName = reinterpret_cast<const char*>(text);
        xmlFree(text);
    }
    if (href) {
        url = reinterpret_cast<const char*>(href);
        xmlFree(href);
    }
    if (addDate_) {
        addDate = reinterpret_cast<const char*>(addDate_);
        xmlFree(addDate_);
    }
    if (lastModified) {
        lastModifiedDate = reinterpret_cast<const char*>(lastModified);
        xmlFree(lastModified);
    }
}

// Print the attributes of a link element
void printLinkAttributes(const std::string& url, const std::string& linkName,
                         const std::string& addDate, const std::string& lastModifiedDate,
                         const std::string& tree) {
    std::cout << tree << "Url: " << url << std::endl;
    std::cout << tree << "Name: " << linkName << std::endl;
    std::cout << tree << "Add date: " << addDate << std::endl;
    std::cout << tree << "Last modified date: " << lastModifiedDate << std::endl;
}

// Split a string by a delimiter
std::vector<std::string> split(const std::string& str, char delimiter) {
    std::vector<std::string> tokens;
    std::stringstream ss(str);
    std::string token;
    while (std::getline(ss, token, delimiter)) {
        tokens.push_back(token);
    }
    return tokens;
}

// Join vector of strings with a delimiter
std::string join(const std::vector<std::string>& vec, const std::string& delimiter) {
    std::ostringstream oss;
    for (size_t i = 0; i < vec.size(); ++i) {
        if (i != 0) {
            oss << delimiter;
        }
        oss << vec[i];
    }
    return oss.str();
}

// Function to create all nested directories
bool createNestedDirectories(const std::string& fullPath) {
    std::vector<std::string> pathParts = split(fullPath, '/');
    std::string currentPath;

    for (const std::string& part : pathParts) {
        if (part.empty()) {
            continue; // Skip empty parts (e.g., leading '/')
        }

        currentPath += "/" + part;

        // Check if the directory exists
        struct stat info;
        if (stat(currentPath.c_str(), &info) != 0) {
            // Directory does not exist, create it
            if (mkdir(currentPath.c_str(), 0755) != 0) {
                if (errno != EEXIST) {
                    std::cerr << "Error creating directory: " << currentPath << std::endl;
                    return false;
                }
            }
        }
        else if (!(info.st_mode & S_IFDIR)) {
            // Path exists but is not a directory
            std::cerr << "Path exists but is not a directory: " << currentPath << std::endl;
            return false;
        }
    }

    return true;
}

// Function to cut a given number of path parts from the beginning
std::string cutPathParts(const std::string& path, int partsToCut) {
    if (partsToCut < 0) {
        return path; // Return original path if partsToCut is negative
    }

    std::vector<std::string> pathParts = split(path, '/');
    if (static_cast<size_t>(partsToCut) >= pathParts.size()) {
        return path; // Return original path if partsToCut is greater than or equal to the number of parts
    }

    std::vector<std::string> shortenedPathParts(pathParts.begin() + partsToCut, pathParts.end());
    return join(shortenedPathParts, "/");
}

// Generate tree prefix
std::string generateTreePrefix(int treeLevel) {
    if (treeLevel < 1) {
        return "";
    }
    std::string prefix;
    for (int i = 0; i < treeLevel - 1; ++i) {
        prefix += "│   ";
    }
    return prefix + "│───|";
}

// Sanitize path
std::string sanitizePath(const std::string& candidatePath) {
    std::string sanitizedPath = candidatePath;
    std::replace_if(sanitizedPath.begin(), sanitizedPath.end(), [](char c) {
        // List of characters generally avoided in Linux paths
        return c == '\0' || c == ':' || c == '*' || c == '?' || c == '"' || c == '<' || c == '>' || c == '|' || c == '\\';
        }, '_'); // Replace invalid characters with an underscore

    return sanitizedPath;
}

// Function to check if a path contains more parts than a given number
bool hasEnoughParts(const std::string& path, int requiredParts) {
    if (requiredParts < 0) {
        return false; // Return false if requiredParts is negative
    }

    std::vector<std::string> pathParts = split(path, '/');
    return static_cast<int>(pathParts.size()) > requiredParts;
}

std::string getAbsolutePath(const std::vector<std::string>& path) {
    std::string relativePath = join(path, "/");
    return concatenatePaths(pathPrefix, cutPathParts(sanitizePath(relativePath), cutPathPartNumber));
}

void printAbsolutePath(const std::vector<std::string>& path, int treeLevel) {
    (void)treeLevel; // Unused parameter

    //std::cout << "Tree level: " << treeLevel << std::endl;
    std::cout << "Directory: " << getAbsolutePath(path) << std::endl;

    /*
    std::cout << treePrefix
              << "Directory: "
              << getFullPath(path) << std::endl;
              */
}

void createLinkFile(const std::string& url, const std::string& linkName,
                    const std::string& addDate, const std::string& lastModifiedDate,
                    const std::string& linkDirectory) {
    create_url_file(
        widen(linkName).c_str(),
        widen(url).c_str(),
        widen(linkDirectory).c_str());
}

// Recursively search for reference tags
void searchReferenceTags(xmlNode* node, std::vector<std::string>& path, int treeLevel)
{
    int elementIndex = 0;
    int directoryElementIndex = 0;
    std::string treePrefix = generateTreePrefix(treeLevel);

    for (xmlNode* element = node; element; element = element->next) {
        if (element->type == XML_ELEMENT_NODE) {
            /*
            std::cout << treePrefix
                      << "Tree level: " << treeLevel 
                      << ", element name: " << element->name 
                      << ", index: " << elementIndex << std::endl;
                      */
            elementIndex++;

            //  The paragraph after the "dl" element closes the directory
            if (xmlStrcasecmp(element->name, BAD_CAST "p") == 0 && directoryElementIndex > 0) {
                directoryElementIndex--;
                if (directoryElementIndex == 0) {
                    std::cout << "Directory closed..." << std::endl;

                    if ((int)path.size() > treeLevel - 4) {
                        path.pop_back();
                    }
                }
            }

            // Names must be in lower case
            if (xmlStrcasecmp(element->name, BAD_CAST "h3") == 0) {
                xmlChar* text = xmlNodeGetContent(element);
                if (text) {
                    //  Correcting path depth according to the tree level
                    std::string currentDirectory = reinterpret_cast<const char*>(text);
                    path.push_back(currentDirectory);
                    xmlFree(text);
                }
            }

            if (xmlStrcasecmp(element->name, BAD_CAST "dl") == 0) {
                directoryElementIndex = 1;

                std::cout << std::endl;
                printAbsolutePath(path, treeLevel);
            }

            if (xmlStrcasecmp(element->name, BAD_CAST "a") == 0) {
                xmlChar* href = xmlGetProp(element, BAD_CAST "href");
                // Check if the href attribute is a valid URL
                if (href && (xmlStrncmp(href, BAD_CAST "http", 4) == 0 || 
                             xmlStrncmp(href, BAD_CAST "https", 5) == 0)) {
                    std::string relativePath = join(path, "/");
                    // Skip the link if the directory has not enough parts (it is Firefox directory)
                    if (hasEnoughParts(relativePath, cutPathPartNumber) && 
                        createNestedDirectories(getAbsolutePath(path)))
                    {
                        std::string url; 
                        std::string linkName;
                        std::string addDate;
                        std::string lastModifiedDate;
                        getLinkAttributes(element, url, linkName, addDate, lastModifiedDate);

                        createLinkFile(url, linkName, addDate, lastModifiedDate, getAbsolutePath(path));

                        std::string columnPrefix = "    ";
                        printLinkAttributes(url, linkName, addDate, lastModifiedDate, columnPrefix);
                        //printLinkAttributes(url, linkName, addDate, lastModifiedDate, treePrefix);
                    }
                }
                else {
                    xmlFree(href);
                }
            }
        }
        // Recursively search in child nodes
        searchReferenceTags(element->children, path, treeLevel + 1);
    }
}

int main()
{
    // Initialize the library and check potential ABI mismatches
    LIBXML_TEST_VERSION

    //  Controls output of the application.
    //  It seems than text is converted internally to the selected code page.
#ifdef _WINDOWS
    //  IIS works only with UTF-8.
    setlocale(LC_ALL, "ru_RU.UTF-8");
#else
    //  Use the "locale" command to see actual console locale at the system.
    //  Apache displays cyrillic properly only with this settings.
    setlocale(LC_ALL, "C.UTF-8");
#endif

    // Use htmlReadFile instead of xmlReadFile
    htmlDocPtr doc = htmlReadFile(dataFilePath.c_str(), nullptr, HTML_PARSE_RECOVER | HTML_PARSE_NOERROR | HTML_PARSE_NOWARNING);
    if (doc == nullptr) {
        std::cerr << "Error loading file." << std::endl;
        return -1;
    }

    // Access the root element
    xmlNode* root = xmlDocGetRootElement(doc);
    if (root == nullptr) {
        std::cerr << "Error: No root element." << std::endl;
        xmlFreeDoc(doc);
        return -1;
    }

    // Recursively search for reference tags
    std::vector<std::string> path;
    searchReferenceTags(root, path, 1);

    // Free the document
    xmlFreeDoc(doc);

    // Cleanup function for the XML library
    xmlCleanupParser();

    return 0;
}