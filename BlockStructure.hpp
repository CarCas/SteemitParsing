//
// Created by Carmine Caserio on 19/12/2019.
//

#include <list>
#include <utility>
#include <vector>
#include <string>

using namespace std;

using ui = unsigned int;

class Operation{
public:
    std::string kind = "";

    virtual std::string print();
};

class Vote : public Operation {
private:
    std::string voter, author, permlink, timestamp;
    int weight;
public:
    Vote(std::string voter, std::string author, std::string permlink, int weigth, string timestamp);
    virtual std::string print();
    std::string get_voter();
    std::string get_author();
    std::string get_permlink();
    std::string get_timestamp();
    int get_weight();
};

class Comment : Operation{
private:
    int votes_obtained = 0;
    int weights_obtained = 0;
    std::string author, permlink, title, body, json_metadata, parent_author, parent_permlink, timestamp;
public:
    Comment(std::string parent_author, std::string parent_permlink, std::string author, std::string permlink, std::string title, std::string body, std::string json_metadata, std::string timestamp);
    int get_votes_obtained();
    void add_vote();
    int get_weights();
    void add_weights(int w);
    std::string get_author();
    std::string get_timestamp();
    virtual std::string print();
};


class CustomJsonFollow : Operation{
private:
    std::string follower, following, timestamp;
    vector<std::string> what{};
    vector<std::string> required_posting_auths{};
public:
    CustomJsonFollow(std::string follower, std::string following, vector<std::string> what, vector<std::string> required_posting_auths, std::string timestamp);
    std::string get_follower();
    std::string get_following();
    vector<std::string> get_what();
    vector<std::string> get_required_auths();
    std::string get_timestamp();
    virtual std::string print();
};

class CustomJsonReblog : Operation{
private:
    std::string account, author, permlink, timestamp;
    vector<std::string> req_posting_auths{};
public:
    CustomJsonReblog(std::string account, std::string author, std::string permlink, vector<std::string> req_posting_auths, std::string timestamp);
    std::string get_account();
    std::string get_author();
    std::string get_permlink();
    vector<std::string> get_req_posting_auths();
    std::string get_timestamp();
    virtual std::string print();
};


class Card : Operation{
private:
    vector<std::string> cards{};
    int price = -1, fee_pct = -1;
public:
    Card(vector<std::string> cards, int price, int fee_pct);
    vector<std::string> get_cards();
    int get_price();
    int get_fee_pct();
    virtual std::string print();
};


class CustomJsonSellCards : Operation{
private:
    vector<Card> cards{};
    std::string req_posting_auths;
public:
    CustomJsonSellCards(const std::string& req_posting_auths);
    void append(const Card& card);
    std::string get_req_posting_auths();
    virtual std::string print();
};


class Transfer : Operation{
private:
    std::string _from_, _to_, _memo_, _amount_;
public:
    Transfer(std::string _from, std::string _to, std::string _amount, std::string _memo);
    std::string from_();
    std::string to_();
    std::string memo();
    std::string amount();
    virtual std::string print();

};

class TransferToVesting : Operation {
private:
    std::string _from, _to, _amount;
public:
    TransferToVesting(std::string _from, std::string _to, std::string _amount);
    std::string from_();
    std::string to_();
    std::string amount();
    virtual std::string print();
};

class WithdrawVesting : Operation{
private:
    std::string account,  vesting_shares;
public:
    WithdrawVesting(std::string account, std::string vesting_shares);
    virtual std::string print();
};

class LimitOrderCreate : Operation{
private:
    std::string owner, expiration, amount_to_sell, min_to_receive;
    int orderid;
    bool fillorkill;
public:
    LimitOrderCreate(std::string owner, int orderid, std::string amount_to_sell,
            std::string min_to_receive, bool fillorkill, std::string expiration);
    virtual std::string print();
};

class LimitOrderCancel : Operation{
private:
    std::string owner;
    int orderid;
public:
    LimitOrderCancel(std::string owner, int orderid);
    virtual std::string print();
};

class Price : Operation{
private:
    std::string base, quote;
public:
    Price(std::string base, std::string quote);
    virtual std::string print();
};

class FeedPublish : Operation{
private:
    std::string publisher, exchange_rate;
public:
    FeedPublish(std::string publisher, std::string exchange_rate);
    virtual std::string print();
};


class TransactionBlock{
private:
    vector<Operation*> operations{};
    std::string transaction_id;
    int transaction_num = -1;
    std::string timestamp;

public:
    TransactionBlock(vector<Operation*> ops);
    TransactionBlock();
    vector<Operation*> get_operations();
    void set_operations(vector<Operation*> ops);
    std::string get_transaction_id();
    void set_transaction_id(std::string tid);
    int get_transaction_num();
    void set_transaction_num(int trn);
    virtual std::string print();
};


class Transaction{
private:
    std::string transaction_id;
    TransactionBlock transactionBlock;
public:
    Transaction(std::string tr_id, TransactionBlock  transactionBlock);
    std::string get_transaction_id();
    void set_transaction_id(std::string tid);
    TransactionBlock get_transaction_block();
    void set_transaction_block(TransactionBlock tbl);
    virtual void print();
};

class BlockStructure{
private:
    list<Transaction> transactions;
    std::string timestamp;
    list<int> transaction_ids;
    TransactionBlock transactionBlock;
    std::string witness_name;
public:
    BlockStructure();
    void set_transactions(list<Transaction> transact);
    void set_transaction_block(TransactionBlock transactBlock);
    void set_timestamp(std::string ts);
    string get_timestamp();
    void set_transaction_ids(list<int> tr_ids);
    void set_transaction_block(const TransactionBlock& tb);
    TransactionBlock get_transaction_block();
    void set_witness_name(std::string wn);
    std::string get_witness_name();
    std::list<Transaction> get_transactions();
    void print_block();

};