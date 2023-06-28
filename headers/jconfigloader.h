#ifndef J_CONFIG_LOADER_H
#define J_CONFIG_LOADER_H

#include <iostream>
#include <fstream>
#include <nlohmann/json.hpp>            //json parser

#include <filesystem>

using namespace std;
using json = nlohmann::json;


namespace jconfigloader {
    class parameterObj
    {
        
    private:
        json m_data;                //internal data

        string m_file;              //loaded file
        std::vector<string> m_keys; //loaded keys

    public:
        parameterObj(string filePath) : m_file(filePath), m_keys({}), m_data(nullptr) { load_data(); }
        parameterObj(string filePath, std::vector<string> keys) : m_file(filePath), m_keys(keys), m_data(nullptr) { load_data(); }
        parameterObj(string filePath, std::string* default_data, size_t data_length) : m_file(filePath), m_keys({}), m_data(nullptr) { load_data(default_data, data_length); }
        parameterObj(string filePath, std::vector<string> keys, std::string* default_data, size_t data_length) : m_file(filePath), m_keys(keys), m_data(nullptr) { load_data(default_data, data_length); }
        ~parameterObj() {}

        //loads data from file, can be used to reload data
        json* load_data(string filePath, std::vector<string> keys, std::string* default_data, size_t data_length);
        
        json* load_data() { return load_data("", m_keys, nullptr, 0); }
        json* load_data(string filePath) { return load_data(filePath, m_keys, nullptr, 0); }
        json* load_data(std::vector<string> keys) { return load_data("", keys, nullptr, 0); }
        json* load_data(std::string* default_data, size_t data_length) { return load_data("", m_keys, default_data, data_length); }
        json* load_data(string filePath, std::vector<string> keys) { return load_data(filePath, keys, nullptr, 0); }
        json* load_data(string filePath, std::string* default_data, size_t data_length) { return load_data(filePath, m_keys, default_data, data_length); }
        json* load_data(std::vector<string> keys, std::string* default_data, size_t data_length) { return load_data("", keys, default_data, data_length); }

        //filters data according to set of keys
        json* filter_data(std::vector<string> keys) { return filter_data(keys, &m_data); }

        json* data() { return &m_data; }    //accesses data by reference

        std::string file() { return m_file; }                //returns the file name
        std::vector<string> keys() { return m_keys; }   //returns the used keys

        //Given a vector of keys, it returns the value in the json object data in the desired type (if it exists and a conversion is applicable)
        template <typename T> T get_value(const std::vector<string>& keys) { return get_value<T>(keys, &m_data); }

        //returns true if the key is present
        bool has_key(const std::vector<string>& keys) { return has_key(keys, &m_data); }

        //prints all the available keys in the stored object
        void print_keys() {

            if (m_data.is_object()) {
                for (auto it = m_data.begin(); it != m_data.end(); ++it) {
                    std::cout << "Key: " << it.key() << std::endl;
                }
            }
        }

    private: //recursive functions searching into the data pointing to the keys according to a decreasing vector
        json* filter_data(std::vector<string> keys, json* file);
        template <typename T> T get_value(const std::vector<string>& keys, json* file) {

            if (file == nullptr) {
                throw std::runtime_error("jconfigloader::get_value: Invalid file pointer");
            }

            if (!file->is_object()) {
                throw std::runtime_error("jconfigloader::get_value: Invalid JSON object");
            }

            if (keys.empty()) {
                throw std::runtime_error("jconfigloader::get_value: Empty keys vector");
            }

            T value;

            //recursive call on the keys vector, removing the 
            if (file->contains(keys[0])) {

                if (keys.size() == 1) { //base case - returns the value of the appropriate type if a conversion is applicable

                    //try the conversion
                    try { value = (*file)[keys[0]].get<T>(); }
                    catch(const std::exception& e) {
                        
                        throw std::runtime_error("jconfigloader:: Failed to convert: " + string( e.what() ));
                    }
                    
                    return value;
                }
                else {  //recursion case - calls the get_value<T> function without the key in position 0

                    std::vector<string> inner_keys(std::next(keys.begin()), keys.end());
                    return get_value<T>(inner_keys, &((*file)[keys[0]]));
                }
            }
            
            throw std::runtime_error("jconfigloader:: key not found: " + keys[0]);

            return value;   //returns default value

        }
        bool has_key(const std::vector<string>& keys, json* file);

    };
    
} // jconfigloader

#endif //  J_CONFIG_LOADER_H