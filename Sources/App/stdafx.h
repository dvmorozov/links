#pragma once

#include <algorithm>
#include <assert.h>
#include <cctype>
#include <cerrno>
#include <clocale>
#include <codecvt>
#include <cstdio>
#include <cstdint>
#include <cstring>
#include <cwchar>
#include <errno.h>
#include <fcntl.h>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <locale>
#include <map>
#include <memory>
#include <regex>
#include <sstream>
#include <stdbool.h>
#include <stddef.h>
#include <stdexcept>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <time.h>
#include <vector>

#ifndef _WINDOWS
#include <openssl/evp.h>
#include <openssl/sha.h>
#endif
