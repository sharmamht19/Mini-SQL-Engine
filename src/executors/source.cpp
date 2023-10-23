#include "global.h"
/**
 * @brief 
 * SYNTAX: SOURCE filename
 */
#include <iostream>
#include <fstream>

void doCommand();


bool syntacticParseSOURCE()
{
    logger.log("syntacticParseSOURCE");
    if (tokenizedQuery.size() != 2)
    {
        cout << "SYNTAX ERROR" << endl;
        return false;
    }
    parsedQuery.queryType = SOURCE;
    parsedQuery.sourceFileName = tokenizedQuery[1];
    return true;
}

bool semanticParseSOURCE()
{
    logger.log("semanticParseSOURCE");
    if (!isQueryFile(parsedQuery.sourceFileName))
    {
        cout << "SEMANTIC ERROR: File doesn't exist" << endl;
        return false;
    }
    return true;
}

void executeSOURCE()
{
    logger.log("executeSOURCE");
    // cout << parsedQuery.sourceFileName << "Hello " << endl;

    string fileDest = "../data/" + parsedQuery.sourceFileName + ".ra";
    // cout << fileDest << "*****" << endl;

    ifstream file(fileDest);
    if (!file.is_open()) {
        cerr << "Could not open file: " << fileDest << endl;
        return;
    }

    string line;
    regex delim("[^\\s,]+");
    string command;

    while (getline(file, line)) {
        // cout << line << endl;
        cout << "Executing Command:" << line<< "\n> ";
        tokenizedQuery.clear();
        parsedQuery.clear();
        logger.log("\nReading New Command: ");
        command = line;
        logger.log(command);


        auto words_begin = std::sregex_iterator(command.begin(), command.end(), delim);
        auto words_end = std::sregex_iterator();
        for (std::sregex_iterator i = words_begin; i != words_end; ++i)
            tokenizedQuery.emplace_back((*i).str());
        // cout<<"tq"<<tokenizedQuery[0]<<endl;
        if (tokenizedQuery.size() == 1 && tokenizedQuery.front() == "QUIT")
        {
            break;
        }

        if (tokenizedQuery.empty())
        {
            continue;
        }

        if (tokenizedQuery.size() == 1)
        {
            cout << "SYNTAX ERROR" << endl;
            continue;
        }
        doCommand();
    }
    file.close();

    return;
}
