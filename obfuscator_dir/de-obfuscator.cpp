#include <iostream>
#include <fstream>
#include <print>
#include <random>
#include <string>
#include <vector>
#include <filesystem>
#include <ctime>
#include <unordered_set>
#include <cinttypes>
#include <algorithm>

using namespace std;

class deobfuscator {

    const uint64_t seed1, seed2, s_address1, s_address2, t_address1, t_address2;

    public:
        explicit deobfuscator(const uint64_t seed1, const uint64_t seed2, const uint64_t s_address1, const uint64_t s_address2, const uint64_t t_address1, const uint64_t t_address2)
            : seed1(seed1), seed2(seed2), s_address1(s_address1), s_address2(s_address2), t_address1(t_address1), t_address2(t_address2){}
        int run(const string& in_file, string& out_file, const string& index);
};

int deobfuscator::run(const string& in_file, string& out_file, const string& index) {

    // TANIMLAR

    ifstream infile(in_file, std::ios::binary);
    
    if (!infile) {
        println(stderr, "Error opening input file: ", in_file);
        return 3;
    }

    // PERFORMANS VE BELLEK ACISINDAN BUFFERINGIN DEVRE DISI BIRAKILMASI
    
    infile.rdbuf()->pubsetbuf(nullptr, 0);
    out_file.append(".part").append(index);

    ofstream outfile(out_file, std::ios::binary);
    outfile.rdbuf()->pubsetbuf(nullptr, 0);

    if (!outfile) {
        println(stderr, "Error opening output file: {}", out_file);
        return 5;
    }

    vector<uint64_t> source_addr;
    vector<uint64_t> target_addr;
    
    unordered_set<uint64_t> source_addr_set;

    mt19937_64 generator1(seed1);
    mt19937_64 generator2(seed2);

    uniform_int_distribution<uint64_t> dist1(s_address1, s_address2);

    uint64_t temp = 0;

    println("Initialized deobfuscator with parameters:");
    println("Seed1: {}", seed1);
    println("Seed2: {}", seed2);
    println("s_address1: {}", s_address1);
    println("s_address2: {}", s_address2);
    println("t_address1: {}", t_address1);
    println("t_address2: {}", t_address2);
    println("Input file: {}", in_file);
    println("Output file: {}", out_file);

    if(t_address2 - t_address1 > s_address2 - s_address1){
        println(stderr, "Error: Target address range is larger than source address range.");
        return 4;
    }


    println("Starting source and target address generation...");

    // RASTGELE OLARAK SECILMIS KAYNAK ADRESLERININ URETILMESI

    while(true){
        temp = dist1(generator2);
        source_addr_set.emplace(temp);
        if(source_addr_set.size() == t_address2 - t_address1 + 1) {
            source_addr.assign(source_addr_set.begin(), source_addr_set.end());
            shuffle(source_addr.begin(), source_addr.end(), generator2);
            source_addr_set.clear();
            break;
        }
    }

    println("Source addresses generated successfully.");
    println("Source addresses: {}", source_addr.size());
    println("Generating target addresses...");

    // RASTGELE OLARAK SECILMIS HEDEF ADRESLERININ URETILMESI

    for (uint64_t i = t_address1; i <= t_address2; i++)
    {
        target_addr.emplace_back(i);
    }
    shuffle(target_addr.begin(), target_addr.end(), generator1);

    println("Target addresses generated successfully.");
    println("Target addresses: {}", target_addr.size());

    println("De-Obfuscating file...");

    // KAYNAKTAN HEDEFE BAYT TRANSFERI
    for (uint64_t i = 0; i < source_addr.size(); i++)
    {
        infile.seekg(source_addr.at(i));
        auto ch = infile.get();
        outfile.seekp(target_addr.at(i) - t_address1);
        outfile.put(ch);
    }

    infile.close();
    outfile.close();
    
    println("De-obfuscation completed successfully.");
    return 0;
}


int main(int argc, char* argv[]) {
    if (argc <= 1) {
        println(stderr, "Usage: de-obfuscator <seed1> <seed2> <s_address1> <s_address2> <t_address1> <t_address2> <input_file> <output_file> <index>");
        return 1;
    } else if (argc == 10) {
        deobfuscator deobf(stoull(argv[1]), stoull(argv[2]), stoull(argv[3]), stoull(argv[4]), stoull(argv[5]), stoull(argv[6]));
        string out_file = argv[8];
        return deobf.run(argv[7], out_file, argv[9]);
    } else {
        println(stderr, "Arguments mismatch. Expected 9 arguments, got {}.", argc-1);
        return 2;
    }
}