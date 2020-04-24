# -lstdc++fs needed for using the std::filesystem package included in the C++17 version.
all:
	g++-8 -std=c++17 -Wall -pedantic -O2 -I. ./UserStructure.hpp ./BlockStructure.cpp ./BlockStructure.hpp ./ParsingBlocks.hpp ./clock.hpp ./main.cpp -lpthread -lstdc++fs -o caserio_parsing8.out 
	./caserio_parsing8.out ./accountsInfoNew.json ./reputations.dat ./bots_list.txt ./steem_files/steem.blockchain_0.json

exec:
	./caserio_parsing.out ./InfoUsers/accountsInfo.json ./caserio_accountsInfo_rep.dat ./caserio_bots_list.txt ./steem.blockchain_0.json ./steem.blockchain_1.json ./steem.blockchain_2.json ./steem.blockchain_03 ./steem.blockchain_04 ./steem.blockchain_05 ./steem.blockchain_06 ./steem.blockchain_07 ./steem.blockchain_08 ./steem.blockchain_09 ./steem.blockchain_10 ./steem.blockchain_11 ./steem.blockchain_12 ./steem.blockchain_13 ./steem.blockchain_14 ./steem.blockchain_14.5.json ./steem.blockchain_15 ./steem.blockchain_16.json ./steem.blockchain_17.json

compile:
	g++ -std=c++17 -Wall -pedantic -O2 -I. ./UserStructure.hpp ./BlockStructure.cpp ./BlockStructure.hpp ./ParsingBlocks.hpp ./clock.hpp ./main.cpp -lpthread -lstdc++fs -o caserio_parsing.out 

compile8:
	g++-8 -std=c++17 -Wall -pedantic -O2 -I. ./UserStructure.hpp ./BlockStructure.cpp ./BlockStructure.hpp ./ParsingBlocks.hpp ./clock.hpp ./main.cpp -lpthread -lstdc++fs -o caserio_parsing8.out 
