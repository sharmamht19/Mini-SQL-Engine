#include "global.hpp"

// Function generates a random file name that is used to store a block
string gen_random_file_name() {
    static int BLOCK_ID = 1;
    int FILE_NAME_LENGTH = 5;
    string file_name;
    file_name.reserve(TEMP_PATH.length() + FILE_NAME_LENGTH + 1);
    for (int i = 0; i < FILE_NAME_LENGTH; i++) {
        file_name += 'a' + rand() % 26;
    }
    file_name += to_string(BLOCK_ID);
    BLOCK_ID++;
    return file_name;
}

/**
 * @brief Generates a file containing numbers from 1 to 100 in a random order.
 *
 * This function creates a vector filled with numbers from 1 to 100, shuffles them
 * to achieve a random order, and then writes them to a file. It returns the shuffled
 * numbers for further processing or verification.
 *
 */
vector<int> generate_random_file(const string &filename) {
    vector<int> data(100);
    iota(data.begin(), data.end(), 1);  // Fill with numbers from 1 to 100

    // Use shuffle with a random engine
    random_device rd;
    default_random_engine engine(rd());
    shuffle(data.begin(), data.end(), engine);

    std::ofstream out(filename);
    for (int num : data) {
        out << num << " ";
    }
    out.close();

    return data;
}

vector<int> read_random_file(const string &filename) {
    std::vector<int> data;

    // Open the file
    ifstream in(filename);
    if (!in.is_open()) {
        throw std::runtime_error("Unable to open file: " + filename);
    }

    // Read numbers from the file
    int number;
    while (in >> number) {
        data.push_back(number);
    }

    in.close();
    return data;
}

void save_to_csv(const vector<int>& data_i_values, const vector<vector<int>>& block_accesses, int fanout) {
    string filename = DATA_PATH + "analysis_results_fanout_" + to_string(fanout) + ".csv";
    ofstream csv_file(filename);

    // Write header
    csv_file << "Data_i,Data_i+1,BlockAccesses_With,BlockAccesses_Without\n";
    
    for (size_t i = 0; i < data_i_values.size(); ++i) {
        csv_file << data_i_values[i] << "," << data_i_values[i+1] << ",";
        csv_file << block_accesses[i][0] << "," << block_accesses[i][1] << "\n";
    }
    
    csv_file.close();
}


// function clears the temp folder
// Function needs to be changed if on WINDOWS
void clear_temp_folder() {
    string a = "rm -r " + TEMP_PATH;
    system(a.c_str());
    a = "mkdir " + TEMP_PATH;
    system(a.c_str());
}

// function initializes setup - clears temp folder and initializes seed for random number generator
void initialize() {
    srand((unsigned)time(nullptr) * getpid());
    clear_temp_folder();
}

void delete_file(string file_name) {
    remove(file_name.c_str());
}

bool is_null(string pointer) {
    if (pointer == NULL_PTR)
        return true;
    return false;
}
