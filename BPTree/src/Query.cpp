#include "Query.hpp"

#include "RecordPtr.hpp"
#include "UnorderedHeap.hpp"

istream& operator>>(istream& is, QueryType& query_type) {
    string query_type_str;
    is >> query_type_str;
    if (query_type_str == "INSERT")
        query_type = INSERT;
    else if (query_type_str == "DELETE")
        query_type = DELETE;
    else if (query_type_str == "RANGE")
        query_type = RANGE;
    else if (query_type_str == "ANALYSE")
        query_type = ANALYSE;
    else if (query_type_str == "EXPORT")
        query_type = EXPORT;
    else if (query_type_str == "SOURCE")
        query_type = SOURCE;
    else
        query_type = QUIT;
    return is;
}

Query::Query() {
    this->query_type = QUIT;
    this->insert_key = DELETE_MARKER;
    this->delete_key = DELETE_MARKER;
    this->min_range_key = DELETE_MARKER;
    this->max_range_key = DELETE_MARKER;
    this->source_file_name = "";
}

Query::Query(const QueryType& query_type, const Key& key1, const Key& key2, const string file_name) {
    this->query_type = query_type;
    this->insert_key = DELETE_MARKER;
    this->delete_key = DELETE_MARKER;
    this->min_range_key = DELETE_MARKER;
    this->max_range_key = DELETE_MARKER;
    switch (query_type) {
        case INSERT:
            this->insert_key = key1;
            break;
        case DELETE:
            this->delete_key = key1;
            break;
        case RANGE:
            this->min_range_key = key1;
            this->max_range_key = key2;
            break;
        case SOURCE:
            this->source_file_name = file_name;
            break;
        default:
            break;
    }
    this->execute();
}

void Query::execute() const {
    switch (this->query_type) {
        case INSERT:
            return this->execute_insert();
        case DELETE:
            return this->execute_delete();
        case RANGE:
            return this->execute_range();
        case ANALYSE:
            return this->execute_analyse();
        case EXPORT:
            return execute_export();
        case SOURCE:
            return execute_source();
        case QUIT:
            return;
    }
}

void Query::execute_insert() const {
    RecordPtr record_ptr = heap.insert_key(this->insert_key);
    bptree.insert_key(this->insert_key, record_ptr);
    chart();
}

void Query::execute_delete() const {
    heap.delete_key(this->delete_key);
    bptree.delete_key(this->delete_key);
    chart();
}

/**
 * executes range query using both B+Tree + heap and just the
 * unordered heap. Prints the number of block accesses in either
 * case. To be used in Part(c)
 */

void Query::execute_range() const {
    ofstream fout(DATA_PATH + "range_[" + to_string(this->min_range_key) + "_" + to_string(this->max_range_key) + "].txt");
    BLOCK_ACCESSES = 0;
    bptree.range(fout, this->min_range_key, this->max_range_key);
    cout << BLOCK_ACCESSES << " ";
    BLOCK_ACCESSES = 0;
    heap.range(fout, this->min_range_key, this->max_range_key);
    cout << BLOCK_ACCESSES << endl;
    BLOCK_ACCESSES = 0;
    fout.close();
}

/**
 * Executes the analysis for evaluating block accesses.
 * This function initiates an analysis by generating randomized data, then it retrieves the number
 * of block accesses required for each pair of consecutive data values using a B+Tree structure.
 * The results are then saved into a CSV file for the given fanout.
 */

void Query::execute_analyse() const {
    vector<int> fanout_values = {4, 6, 8};
    string data_filename = DATA_PATH + "source.txt";
    // vector<int> randomized_data = generate_random_file(data_filename);
    vector<int> randomized_data = read_random_file(data_filename);

    int fanout = FANOUT;
    vector<int> data_i_values;
    vector<vector<int>> block_accesses;

    for (size_t i = 0; i < randomized_data.size() - 1; ++i) {
        data_i_values.push_back(randomized_data[i]);
        vector<int> accesses = this->execute_range(randomized_data[i], randomized_data[i + 1]);
        block_accesses.push_back(accesses);
    }

    save_to_csv(data_i_values, block_accesses, fanout);
}

/**
 * Executes the range query for given minimum and maximum keys.
 * This function computes the number of block accesses required when using a B+Tree and a heap
 * structure for the provided key range. It writes the range data to a temporary file and
 * then deletes it post processing. The results are returned as a vector containing the
 * block accesses for both the B+Tree and heap.
 *
 */

vector<int> Query::execute_range(int minKey, int maxKey) const {
    string filename = DATA_PATH + "range_[" + to_string(minKey) + "_" + to_string(maxKey) + "].txt";
    ofstream fout(filename);

    vector<int> blockAccesses;

    BLOCK_ACCESSES = 0;
    bptree.range(fout, minKey, maxKey);
    blockAccesses.push_back(BLOCK_ACCESSES);

    BLOCK_ACCESSES = 0;
    heap.range(fout, minKey, maxKey);
    blockAccesses.push_back(BLOCK_ACCESSES);

    BLOCK_ACCESSES = 0;  // Resetting the global variable

    fout.close();

    // Remove the file after closing
    delete_file(filename.c_str());

    return blockAccesses;
}

void Query::execute_export() {
    heap.export_heap();
    bptree.export_bptree();
}
void Query::chart() {
    ofstream fout(DATA_PATH + "chart.md");
    bptree.chart(fout);
    heap.chart(fout);
    fout.close();
}

void Query::execute_source() const {
    execute_script(this->source_file_name);
}

void Query::execute_script(string script_name) {
    ifstream fin(DATA_PATH + script_name);
    while (!fin.eof()) {
        auto query = new Query();
        fin >> query;
        query->execute();
        delete query;
    }
    fin.close();
}

void Query::command_line_interpreter() {
    auto query = new Query();
    while (true) {
        cout << "> ";
        cin >> query;
        if (query->query_type == QUIT)
            break;
        query->execute();
    }
    delete query;
}

istream& operator>>(istream& is, Query* query) {
    is >> query->query_type;
    switch (query->query_type) {
        case INSERT:
            is >> query->insert_key;
            break;
        case DELETE:
            is >> query->delete_key;
            break;
        case RANGE:
            is >> query->min_range_key;
            is >> query->max_range_key;
            break;
        case SOURCE:
            is >> query->source_file_name;
            break;
        default:
            break;
    }
    return is;
}
