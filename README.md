# jconfigloader
Simple class offering a loader and generator for parameters in JSON format


## How to Install
Clone the repo on your computer and run the **INSTALL** shell script. You might need to up the script priviledges running this line before the install script.

// chmod +x ./INSTALL.sh

The **INSTALL** script copies the library file (.a) from the build directory in /usr/local/lib and all needed headers (.h) from the headers directory to a new folder /usr/include/jconfigloader

This can be done by hand, with no need for the INSTALL script

### Dependencies
The library depends on the **nlohmann** library for easy JSON manipulation. The other files included in the library are all part of the standard c++ fare, with the small addition that **filesystem** needs the program to be compiled in c++ 17 or later. More on that in the Use->Compiling section


## How to Uninstall
To uninstall the library it is sufficient to delete the files the INSTALL script copied. To expedite the process, you can run the **UNINSTALL** shell script


## Use
To use the library you have to include the jconfigloader header as with any other library

### Functions & Objects
This is the description of the main methods and classes offered by the library

**parameterObj**: class containing the parameters in JSON form

**nlohmann::json* load_data(filePath, keys, default_data, data_length)**:
Loads data from the specified file, can be used to reload the file if it is changed
Always returns a pointer to the JSON object formed

- no parameters -> reloads the file in memory
- std::string filePath -> complete path to the JSON file to load - _if omitted_ the function acts on the memorised file path
- std::vector<std::string> keys -> limit the loaded parameters to a subset of the JSON file - _if omitted_ the function loads all the JSON file without limits
- std::string* default_data -> points to a standard JSON file to be created if the filePath is valid but does not contain the expected JSON file - _if omitted_ it does not point to a default file and therefore throws an exception if the JSON file is not present at the specified path
- size_t data_length -> the length of the default parameter file in number of characters - _NEEDED_ if the default_data parameter is present, to be skipped otherwise

**nlohmann::json* filter_data(keys)**: returns a pointer to a newly made JSON object holding the section of the loaded JSON file according to the std::vector<string> keys provided
**nlohmann::json* data()**: returns a pointer accessing the loaded data
**std::string file()**: returns the loaded file name if present
**std::vector<string> keys()**: returns a vector of the keys used in loading the file
**template <typename T> T get_value(keys)**: given a vector of keys (including the value name in last position), it returns the value in the JSON object data in the desired type (if it exists and a conversion is applicable)
**bool has_key(keys)**: returns true if the specified value is present, false otherwise
**void print_keys()**: prints all the available keys in the stored object


### Compiling
To compile a program using this library, you will have to specify the c++ version to use to be 17 or higher and the library file to take from, as seen in the example below

- g++ -std=c++17 -o <your_output> <your_program> -ljconfigloader

  

## TODO:

- add method examples to README.md
- add better comments to code
- prettify the README.md