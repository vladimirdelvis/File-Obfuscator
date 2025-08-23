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

class obfuscator{

    const uint64_t seed1,seed2,s_address1,s_address2,t_address1,t_address2;

    public:
        explicit obfuscator(const uint64_t seed1, const uint64_t seed2, const uint64_t s_address1, const uint64_t s_address2, const uint64_t t_address1, const uint64_t t_address2)
            : seed1(seed1), seed2(seed2), s_address1(s_address1), s_address2(s_address2), t_address1(t_address1), t_address2(t_address2) {}
        int run(const string& in_file, string& out_file, const string& index);
};

int obfuscator::run(const string& in_file, string& out_file, const string& index) {

    // TANIMLAR
    ifstream infile(in_file, std::ios::binary);

    if (!infile) {
        println(stderr, "Error opening input file: {}", in_file);
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

    uint64_t file_size = std::filesystem::file_size(in_file);
    
    vector<uint64_t> source_addr;
    vector<uint64_t> target_addr;

    unordered_set<uint64_t> target_addr_set;

    mt19937_64 generator1(seed1);

    mt19937_64 generator2(seed2);
    uniform_int_distribution<uint64_t> dist1(t_address1, t_address2);

    mt19937_64 generator3(time(nullptr));
    uniform_int_distribution<uint8_t> dist2(0, 255); // RASTGELE BAYT URETICISI

    uint64_t temp = 0;

    println("Initialized obfuscator with parameters:");
    println("Seed1: {}", seed1);
    println("Seed2: {}", seed2);
    println("s_address1: {}", s_address1);
    println("s_address2: {}", s_address2);
    println("t_address1: {}", t_address1);
    println("t_address2: {}", t_address2);
    println("File size: {} bytes", file_size);
    println("Input file: {}", in_file);
    println("Output file: {}", out_file);

    if(t_address2 - t_address1 < s_address2 - s_address1){
        println(stderr, "Error: Target address range is smaller than source address range.");
        return 4;
    }

    println("Starting the source and target address generation...");

    // ALGORITMA

    // RASTGELE SECILMIS KAYNAK ADRESLERI OLUSTURMA

    for (uint64_t i = s_address1; i <= s_address2; ++i) {
        source_addr.emplace_back(i);
    }
    shuffle(source_addr.begin(), source_addr.end(), generator1);
    println("Source addresses generated successfully.");
    println("Source addresses: {}", source_addr.size());
    println("Generating target addresses...");

    // RASTGELE SECILMIS HEDEF ADRESLERI OLUSTURMA

    while(true)
    {
        temp = dist1(generator2);
        target_addr_set.emplace(temp);
        if(target_addr_set.size() == s_address2 - s_address1 + 1) {
            target_addr.assign(target_addr_set.begin(), target_addr_set.end());
            shuffle(target_addr.begin(), target_addr.end(), generator2);
            target_addr_set.clear();
            //target_addr_set.~unordered_set();
            break;
        }
    }
    
    println("Target addresses generated successfully.");
    println("Target addresses: {}", target_addr.size());
    println("Obfuscating file...");

    // ILGILI BAYTLARI KAYNAK ADRESLERDEN HEDEF ADRESLERE KOPYALAMA
    for (uint64_t i = 0; i < source_addr.size(); i++)
    {
        infile.seekg(source_addr.at(i));
        auto ch = infile.get();
        outfile.seekp(target_addr.at(i) - t_address1);
        outfile.put(ch);
    }
    
    infile.close();
    println("Input file closed successfully.");
    sort(target_addr.begin(), target_addr.end()); // HEDEF ADRESLERIN ARASINI DOLDURMAK ICIN SIRALANMASI
    

    // HEDEF ADRESLERININ ARASINI RASTGELE BAYTLARLA DOLDURMA
    if(t_address2 - t_address1 > s_address2 - s_address1){
        println("Filling gaps with random bytes...");
        for (uint64_t i = 0; i < target_addr.at(0) - t_address1; i++) // DOSYANIN BAŞI DOLDURULUYOR
        {
            outfile.seekp(i);
            outfile.put(dist2(generator3));
        }
        for (uint64_t i = 0; i < target_addr.size() - 1; i++) // TARGET ADRESLERIN ARASI DOLDURULUYOR
        {
            if (target_addr.at(i) + 1 - t_address1 < target_addr.at(i + 1) - t_address1) {
                for (uint64_t f = target_addr.at(i) + 1 - t_address1; f < target_addr.at(i + 1) - t_address1; f++) {
                    outfile.seekp(f);
                    outfile.put(dist2(generator3));
                }
            }
        }
        // DOSYANIN t_address2 - t_address1 + 1 BAYTLIK BOYUTTA OLACAGININ GARANTI EDILMESI

        for (uint64_t i = target_addr.back() + 1; i <= t_address2; i++) // DOSYANIN SONU DOLDURULUYOR
        {
            outfile.seekp(i - t_address1);
            outfile.put(dist2(generator3));
        }
    }

    outfile.close();
    println("Output file closed successfully.");
    println("Obfuscation completed successfully.");

    return 0;
}

// MAIN FUNCTION

int main(int argc, char* argv[]){
    if (argc <= 1){
        println(stderr, "Usage: obfuscator <seed1> <seed2> <s_address1> <s_address2> <t_address1> <t_address2> <input_file> <output_file> <index>");
        return 1;
    }
    else if (argc == 10){
        obfuscator obf(stoull(argv[1]), stoull(argv[2]), stoull(argv[3]), stoull(argv[4]), stoull(argv[5]), stoull(argv[6]));
        string out_file = argv[8];
        return obf.run(argv[7], out_file, argv[9]);
    }
    else{
        println(stderr, "Arguments mismatch. Expected 9 arguments, got {}.", argc-1);
        return 2;
    }
}
