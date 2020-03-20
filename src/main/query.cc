#include <chrono>
#include <fstream>
#include <iostream>

#include "base/binding/binding.h"
#include "base/binding/binding_iter.h"
#include "base/parser/logical_plan/op/op.h"
#include "base/parser/logical_plan/op/op_select.h"
#include "relational_model/graph/relational_graph.h"
#include "relational_model/query_optimizer/physical_plan_generator.h"
#include "relational_model/relational_model.h"

using namespace std;

int main(int argc, char **argv) {
    auto start = chrono::system_clock::now();

    if (argc < 2) {
        cerr << "Error: No input file provided." << endl;
        return 1;
    }
    char const* filename = argv[1];
    ifstream in(filename, ios_base::in);
    if (!in) {
        cerr << "Error: Could not open input file: " << filename << endl;
        return 1;
    }

    stringstream str_stream;
    str_stream << in.rdbuf();
    string query = str_stream.str();

    try {
        auto select_plan = Op::get_select_plan(query);

        RelationalModel::init();

        PhysicalPlanGenerator plan_generator { };
        auto root = plan_generator.exec(*select_plan);

        root->begin();
        auto binding = root->next();
        int count = 0;
        while (binding != nullptr) {
            binding->print();
            binding = root->next();
            count++;
        }

        auto end = chrono::system_clock::now();
        chrono::duration<float, std::milli> duration = end - start;
        cout << "Found " << count << " results in " << duration.count() << " milliseconds.\n";
    }
    catch (std::exception& e) {
        cout << "Exception in query excecution: " << e.what() << endl;
    }
}
