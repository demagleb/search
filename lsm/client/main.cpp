
#include "__generator.hpp"
#include "lsm/structures/sstable.hpp"
#include <argparse/argparse.hpp>

#include <fmt/format.h>

#include <fstream>
#include <ios>
#include <lsm/lsm.hpp>

#include <exception>

namespace fs = std::filesystem;

class LSMClient {
public:
    LSMClient(fs::path path)
        : tree_(path)
    { }

    std::string insertRow(const std::string& key, const std::string& value) {
        tree_.insert(lsm::structures::Row(key, value));
        return fmt::format("inserted key: \"{}\" value: \"{}\"", key, value);
    }

    std::string eraseRow(const std::string& key) {
        tree_.erase(key);
        return fmt::format("erased key: \"{}\"", key);
    }

    std::string dump() {
        tree_.dump();
        return "dumped";
    }

    std::string getRow(const std::string& key) {
        const auto& row = tree_.getByKey(key);
        if (row) {
            return fmt::format("key: \"{}\" value: \"{}\"", row->key(), row->value());
        } else {
            return "no such key";
        }
    }

    std::string getRows(const std::string& left, const std::string& right) {
        std::string out;
        size_t n = 0;
        for (const auto& row : tree_.getByKeyRange(left, right)) {
            ++n;
            out += fmt::format("key: \"{}\" value: \"{}\"\n", row.key(), row.value());
        }
        return fmt::format("{} rows\n{}", n, out);
    }

    std::string getAll() {
        std::string out;
        size_t n = 0;
        for (const auto& row : tree_.getAll()) {
            ++n;
            out += fmt::format("key: \"{}\" value: \"{}\"\n", row.key(), row.value());
        }
        return fmt::format("{} rows\n{}", n, out);
    }

private:
    lsm::LSMTree tree_;
};


int main(int argc, const char** argv)
{
    argparse::ArgumentParser parser("lsm_client", "2.2.8");
    parser.add_argument("-d", "--dir")
        .required()
        .help("lsm tree directory");

    try {
        parser.parse_args(argc, argv);
    }
    catch (const std::exception& err) {
        std::cerr << err.what() << std::endl;
        std::cerr << parser;
        std::exit(1);
    }

    auto dir = parser.get<std::string>("--dir");

    LSMClient client(dir);

    while (true) {
        std::cout << "> ";
        std::cout.flush();
        std::string command;
        std::cin >> command;
        if (command == "insert") {
            std::string key;
            std::string value;
            std::cin >> key >> value;
            std::cout << client.insertRow(key, value) << std::endl;
        } else if (command == "erase") {
            std::string key;
            std::cin >> key;
            std::cout << client.eraseRow(key) << std::endl;
        } else if (command == "get") {
            std::string key;
            std::cin >> key;
            std::cout << client.getRow(key) << std::endl;
        } else if (command == "range") {
            std::string left;
            std::string right;
            std::cin >> left >> right;
            std::cout << client.getRows(left, right) << std::endl;
        } else if (command == "all") {
            std::cout << client.getAll() << std::endl;
        } else if (command == "dump") {
            std::cout << client.dump() << std::endl;
        } else if (command == "exit") {
            std::cout << client.dump() << std::endl;
            std::exit(0);
        } else {
            std::cout << "Unknown command \"" << command << "\"" << std::endl;
        }
    }

    return 0;
}
