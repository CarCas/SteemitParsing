#include <iostream>
#include "ParsingBlocks.hpp"

int main(int argc, char *argv[]) {
    if(argc == 1){
        cout << "Pass me: the accounts information, the reputations file, the list of bots and the " << 
            "file paths of the various files to parse or re-execute me with the --help option!" << endl;
        return 1;
    }
    if(string(argv[1]).compare("--help") == 0){
        cout << "For parsing I need the path of the various files: it's not enough just the filename. " << 
        "After waiting a while, I'll return and you'll see a new folder in the same folder of this .out file!" << endl;
        return 0;
    }
    auto total_time = timer::start();
    auto timer_start = timer::start();
    ParsingBlocks *pb = new ParsingBlocks(argv[1], argv[2], argv[3]);
    auto timer1 = timer::start();
    pb->parsing_users();
    auto stop1 = timer::milli_step(timer1);
    std::cout << "Parsing users took: " << stop1 << " millisec." << std::endl;
    for(int i = 4; i < argc; i++){
        auto timer2 = timer::start();
        std::string entry = argv[i];
        cout << entry << endl;
        pb->parsing_blocks(entry);
        auto stop2 = timer::milli_step(timer2);
        std::cout << "Parsing blocks took: " << stop2 << " millisec." << std::endl;
    }
    /*
    for(const auto & entry : std::filesystem::directory_iterator(argv[1])){
        auto timer2 = timer::start();
        cout << entry.path().string() << endl;
        pb->parsing_blocks(entry.path().string());
        auto stop2 = timer::milli_step(timer2);
        std::cout << "Parsing blocks took: " << stop2 << " millisec." << std::endl;

    }*/
    //vector<BlockStructure> blocks = pb->get_blocks();
    auto timer_stop = timer::milli_step(timer_start);
    std::cout << "Elapsed parsing time: " << timer_stop << std::endl;
    std::cout << "Size of users list: " << pb->get_users().size() << std::endl;
    //std::cout << "Size of blocks: " << blocks.size() << std::endl;
    auto start = timer::start();
    pb->save_by_repclass_file();
    auto stop = timer::milli_step(start);
    std::cout << "Saving to file all CommentHubs tooks " << stop << " millisec." << std::endl;
    auto start2 = timer::start();
    pb->save_frequencies_to_file();
    auto stop2 = timer::milli_step(start2);
    std::cout << "Saving frequencies to file tooks " << stop2 << " millisec." << std::endl;
    auto total_stop = timer::milli_step(total_time);
    std::cout << "The whole computation has been computed in " << total_stop << " millisec." << std::endl;

    
    return 0;
}




// PROCESSO PID SU MACCHINA REMOTA: 151711