#include "../headers/jconfigloader.h"


json* jconfigloader::parameterObj::load_data(string filePath, std::vector<string> keys, std::string* default_data, size_t data_length) {

    filePath = (filePath == "") ? m_file : filePath;
    if (filePath == "")
        throw std::runtime_error("jconfigloader:: no file path provided");


    //check directory path <program path>/config_<prgName>/scriber_init.json
    // create config directory if needed
    std::string dirPath = filePath.substr(0, filePath.find_last_of('/'));

    std::filesystem::path dirPathObj(dirPath);
    if (!std::filesystem::is_directory(dirPathObj)) {

        std::string subDirPath = dirPath.substr(0, dirPath.find_last_of('/'));
        std::filesystem::path subDirPathObj(subDirPath);

        if (!std::filesystem::is_directory(subDirPathObj))
            throw std::runtime_error("jconfigloader:: path does not exist: " + subDirPath);
        else if (mkdir(dirPath.c_str(), 0777) != 0)
            throw std::runtime_error("jconfigloader:: could not create folder: " + dirPath);
    }
   

    if (!std::filesystem::exists(filePath)) {

        if (default_data == nullptr)
            throw std::runtime_error("jconfigloader:: file not found: " + filePath + ". No data present to create default file");
        
        std::ofstream outFile(filePath, std::ios::app);
        if (outFile.is_open()) {

            for (size_t i = 0; i < data_length; i++) {
                outFile << default_data[i];
            }
            
            
            outFile.close();
        }
        else {
            throw std::runtime_error("jconfigloader:: file not found: " + filePath + ". Unable to open default file");
        }
    }

    ifstream file(filePath);
    if (!file) {
        throw std::runtime_error("jconfigloader:: couldn't open file " + filePath);
    }

    //load ALL data
    file >> m_data;
    file.close();

    if (!m_data.is_object()) {

        throw std::runtime_error("jconfigloader:: invalid json object");
    }

    //filter data according to keys
    if (keys.size() == 0)
        return &m_data;
    else {
        m_data = *filter_data(keys);
        return &m_data;
    }
}
json* jconfigloader::parameterObj::filter_data(std::vector<string> keys, json* file) {

    if (file->contains(keys[0])) {

        json tmp_data = m_data[keys[0]];
        *file = (*file)[keys[0]];

        if (keys.size() == 1) {

            if (!file->is_object())
                throw std::logic_error("jconfigloader::filter_data: key does not produce a json object but a single value");

            return &m_data;
        }
        else {  //recursion case - calls the filter_data function without the key in position 0

            std::vector<string> inner_keys(std::next(keys.begin()), keys.end());
            return filter_data(inner_keys, file);
        }
    }
    
    throw std::runtime_error("jconfigloader:: key not found: " + keys[0]);

    return &m_data;
}

bool jconfigloader::parameterObj::has_key(const std::vector<string>& keys, json* file) {

    if (file->contains(keys[0])) {

        if (keys.size() == 1)
            return true;
        else {
            std::vector<string> inner_keys(std::next(keys.begin()), keys.end());
            return has_key(inner_keys, &((*file)[keys[0]]));
        }
    }

    return false;
}
