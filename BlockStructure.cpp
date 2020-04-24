//
// Created by Carmine Caserio on 19/12/2019.
//

#include <iostream>
#include "BlockStructure.hpp"
#include "constants.hpp"
#include <string>

using namespace std;

using ui = unsigned int;

string Operation::print(){
    return "";
}

Vote::Vote(){}

Vote::Vote(string voter, string author, string permlink, int weigth, string timestamp) : voter(move(voter)), author(move(author)),
    permlink(move(permlink)), timestamp(move(timestamp)), weight(weigth) { this->kind = VOTE; }

string Vote::print(){
    string toret = "V\n";
    toret += "voter: " + this->voter + "\n";
    toret += "auth: " + this->author + "\n";
    toret += "permlink: " + this->permlink + "\n";
    toret += "weight: " + to_string(this->weight) + "\n";
    return toret;
}

string Vote::get_voter(){
    return this->voter;
}

string Vote::get_author(){
    return this->author;
}

string Vote::get_permlink(){
    return this->permlink;
}

int Vote::get_weight(){
    return this->weight;
}

string Vote::get_timestamp(){
    return this->timestamp;
}

Comment::Comment(string parent_author, string parent_permlink, string author, string permlink, string title, string body, string json_metadata, string timestamp){
    this->parent_author = (parent_author != "") ? move(parent_author) : "";
    this->parent_permlink = (parent_permlink != "") ? move(parent_permlink) : "";
    this->author = move(author);
    this->permlink = move(permlink);
    this->title = move(title);
    this->body = move(body);
    this->json_metadata = move(json_metadata);
    this->timestamp = move(timestamp);
    this->kind = COMMENT;
}

int Comment::get_votes_obtained(){
    return this->votes_obtained;
}

void Comment::add_vote(){
    this->votes_obtained++;
}

int Comment::get_weights(){
    return this->weights_obtained;
}

void Comment::add_weights(int w){
    this->weights_obtained += w;
}

string Comment::get_author(){
    return this->author;
}

string Comment::get_timestamp(){
    return this->timestamp;
}

string Comment::print(){
    string toret = "C\n";
    toret += "par_auth: " + this->parent_author + "\n";
    toret += "par_perml: " + this->parent_permlink + "\n";
    toret += "auth: " + this->author + "\n";
    toret += "perml: " + this->permlink + "\n";
    toret += "title: " + this->title + "\n";
    toret += "body: " + this->body + "\n";
    toret += "json_mt: " + this->json_metadata + "\n";
    return toret;
}

//void CustomJsonFollow::strip(vector<string> *w) {
//    auto it = w->begin();
//    for (ui i = 0; i < w->size(); i++) {
//        stripLeadingAndtrailingSpaces(&(*it));
//        advance(it, 1);
//    }
//}

//void CustomJsonFollow::stripLeadingAndtrailingSpaces(string *stng) {  // stripLeadingAndTrailingSpaces()
//    char *cur = const_cast<char *>(stng->c_str());
//    /* First remove leading spaces */
//    const char *firstNonSpace = cur;
//    while (*firstNonSpace != '\0' && isspace(*firstNonSpace))
//        ++firstNonSpace;
//    size_t len = strlen(firstNonSpace) + 1;
//    memmove(cur, firstNonSpace, len);
//    /* Now remove trailing spaces */
//    char *endOfString = cur + len;
//    while (cur < endOfString && isspace(*endOfString))
//        --endOfString;
//    *endOfString = '\0';
//}

CustomJsonFollow::CustomJsonFollow(string follower, string following, vector<string> what, vector<string> required_posting_auths, std::string timestamp) {
    this->follower = move(follower);
    //stripLeadingAndtrailingSpaces(&this->follower);
    this->following = move(following);
    //stripLeadingAndtrailingSpaces(&this->following);
    //strip(&what);
    this->what = move(what);
    //strip(&required_posting_auths);
    this->required_posting_auths = move(required_posting_auths);
    this->timestamp = move(timestamp);
    this->kind = FOLLOW;
}

string CustomJsonFollow::get_follower() {
    return this->follower;
}

string CustomJsonFollow::get_following(){
    return this->following;
}

vector<string> CustomJsonFollow::get_what(){
    return this->what;
}

vector<string> CustomJsonFollow::get_required_auths(){
    return this->required_posting_auths;
}

string CustomJsonFollow::get_timestamp(){
    return this->timestamp;
}

string CustomJsonFollow::print(){
    string toret = "F\n";
    toret += "follower: " + this->follower + "\n";
    toret += "following: " + this->following + "\n";
    toret += "what: [ ";
    if (!this->what.empty()){
        for (const string& w : this->what)
            toret += w + " ";
    }
    toret += "]\n";
    if (!this->required_posting_auths.empty()){
        toret += "req_auths: [ ";
        for (const string& ra : this->required_posting_auths)
            toret += ra + " ";
    }
    toret += "]\n";
    return toret;
}


CustomJsonReblog::CustomJsonReblog(string account, string author, string permlink, vector<string> req_posting_auths, string timestamp){
    this->account = move(account);
    this->author = move(author);
    this->permlink = move(permlink);
    this->req_posting_auths = move(req_posting_auths);
    this->timestamp = move(timestamp);
    this->kind = REBLOG;
}

string CustomJsonReblog::get_account(){
    return this->account;
}

string CustomJsonReblog::get_author(){
    return this->author;
}

string CustomJsonReblog::get_permlink(){
    return this->permlink;
}

vector<string> CustomJsonReblog::get_req_posting_auths(){
    return this->req_posting_auths;
}

string CustomJsonReblog::get_timestamp(){
    return this->timestamp;
}

string CustomJsonReblog::print(){
    string toret = "RB\n";
    toret += "account: " + this->account + "\n";
    toret += "author: " + this->author + "\n";
    toret += "permlink: " + this->permlink + "\n";
    toret += "req_posting_auths: [ ";
    if (!req_posting_auths.empty()){
        for (const string& r : this->req_posting_auths)
            toret += r + " ";
    }
    toret += "]\n";
    return toret;
}


Card::Card(vector<string> cards, int price, int fee_pct){
    this->cards = move(cards);
    this->price = price;
    this->fee_pct = fee_pct;
    this->kind = CARD;
}

vector<string> Card::get_cards(){
    return this->cards;
}

int Card::get_price(){
    return this->price;
}

int Card::get_fee_pct(){
    return this->fee_pct;
}

string Card::print(){
    string toret = "CARD\n";
    if (!this->cards.empty())
        for (const string& c : this->cards)
            toret += c + "\n";
    else
        toret += "[]\n";

    toret += "price: " + to_string(this->price) + "\n";
    toret += "fee_pct: " + to_string(this->fee_pct) + "\n";
    return toret;
}


CustomJsonSellCards::CustomJsonSellCards(const string& req_posting_auths){
    if (!req_posting_auths.empty())
        this->req_posting_auths = req_posting_auths;
    this->kind = SM_SELL;
}

void CustomJsonSellCards::append(const Card& card){
    this->cards.push_back(card);
}

string CustomJsonSellCards::get_req_posting_auths(){
    return this->req_posting_auths;
}

string CustomJsonSellCards::print(){
    string toret = "SM SELL\n";
    for(Card c : this->cards)
        toret += c.print() + "\n";
    return toret;
}


Transfer::Transfer(string _from, string _to, string _amount, string _memo) :
        _from_(move(_from)), _to_(move(_to)), _memo_(move(_memo)), _amount_(move(_amount)) { this->kind = TRANSFER; }

string Transfer::from_(){
    return this->_from_;
}

string Transfer::to_(){
    return this->_to_;
}

string Transfer::memo() {
    return this->_memo_;
}

string Transfer::amount() {
    return this->_amount_;
}

string Transfer::print(){
    string toret = "TR\n";
    toret += "from: " + this->_from_ + "\n";
    toret += "to: " + this->_to_ + "\n";
    toret += "amount: " + this->_amount_ + "\n";
    toret += "memo: " + this->_memo_ + "\n";
    return toret;
}


TransferToVesting::TransferToVesting(string _from, string _to, string _amount) : _from(move(_from)), _to(move(_to)), _amount(move(_amount)) {
    this->kind = TRANSFER_TO_VESTING;
}

string TransferToVesting::from_(){
    return this->_from;
}

string TransferToVesting::to_(){
    return this->_to;
}

string TransferToVesting::amount(){
    return this->_amount;
}

string TransferToVesting::print(){
    string toret = "TR_TO_V\n";
    toret += "from: " + this->_from + "\n";
    toret += "to: " + this->_to + "\n";
    toret += "amount: " + this->_amount + "\n";
    return toret;
}


WithdrawVesting::WithdrawVesting(string account, string vesting_shares){
    this->account = account;
    this->vesting_shares = vesting_shares;
    this->kind = WITHDRAW_VESTING;
}

string WithdrawVesting::print(){
    string toret = "WITHDRAW\n";
    toret += "account: " + this->account + "\n";
    toret += "v_shares: " + this->vesting_shares + "\n";
    return toret;
}


LimitOrderCreate::LimitOrderCreate(string owner, int orderid, string amount_to_sell,
        string min_to_receive, bool fillorkill, string expiration){
            this->owner = move(owner);
            this->expiration = move(expiration);
            this->orderid = orderid;
            this->amount_to_sell = move(amount_to_sell);
            this->min_to_receive = move(min_to_receive);
            this->fillorkill = fillorkill;
            this->kind = LIMIT_ORDER_CREATE;
        }

string LimitOrderCreate::print() {
    string toret = "LO_CREATE\n";
    toret += "owner: " + this->owner + "\n";
    toret += "orderid: " + to_string(this->orderid) + "\n";
    toret += "amount_to_sell: " + this->amount_to_sell + "\n";
    toret += "min_to_rcv: " + this->min_to_receive + "\n";
    toret += "f/kill: " + to_string(this->fillorkill) + "\n";
    toret += "expiration: " + this->expiration + "\n";
    return toret;
}


LimitOrderCancel::LimitOrderCancel(string owner, int orderid) : owner(move(owner)) {
    this->orderid = orderid;
    this->kind = LIMIT_ORDER_CANCEL;
}

string LimitOrderCancel::print(){
    string toret = "LO_CANCEL\n";
    toret += "owner: " + this->owner + "\n";
    toret += "orderid: " + to_string(this->orderid) + "\n";
    return toret;
}


Price::Price(string base, string quote) : base(move(base)), quote(move(quote)){
    this->kind = PRICE;
}

string Price::print(){
    string toret = "PRICE\n";
    toret += "base: " + this->base + "\n";
    toret += "quote: " + this->quote + "\n";
    return toret;
}


FeedPublish::FeedPublish(string publisher, string exchange_rate) : publisher(move(publisher)), exchange_rate(move(exchange_rate)){
    this->kind = FEED_PUBLISH;
}

string FeedPublish::print(){
    string toret = "FEED_PUBLISH\n";
    toret += "publisher: " + this->publisher + "\n";
    toret += "exchange_rate: " + this->exchange_rate + "\n";
    return toret;
}


TransactionBlock::TransactionBlock(vector<Operation*> ops) : operations(move(ops)) {}

TransactionBlock::TransactionBlock() = default;

vector<Operation*> TransactionBlock::get_operations() {
    return this->operations;
}

void TransactionBlock::set_operations(vector<Operation*> ops) {
    if (!ops.empty())
        this->operations = move(ops);
}

string TransactionBlock::get_transaction_id(){
    return this->transaction_id;
}

void TransactionBlock::set_transaction_id(string tid){
    this->transaction_id = move(tid);
}

int TransactionBlock::get_transaction_num(){
    return this->transaction_num;
}

void TransactionBlock::set_transaction_num(int trn){
    this->transaction_num = trn;
}

string TransactionBlock::print(){
    string toret = "TRANSACTION BLOCK";
    toret += "\n";
    for (Operation* op : this->operations)
        toret += op->print();
    toret += "transaction_id: " + this->transaction_id + "\n";
    toret += "transaction_num: " + to_string(this->transaction_num) + "\n";
    return toret;
}



Transaction::Transaction(string tr_id, TransactionBlock  transactionBlock)
        : transaction_id(move(tr_id)), transactionBlock(move(transactionBlock)) {}

string Transaction::get_transaction_id(){
    return this->transaction_id;
}

void Transaction::set_transaction_id(string tid){
    this->transaction_id = move(tid);
}

TransactionBlock Transaction::get_transaction_block(){
    return this->transactionBlock;
}

void Transaction::set_transaction_block(TransactionBlock tbl) {
    this->transactionBlock = move(tbl);
}

void Transaction::print(){
    cout << ("TRANSACTION") << endl;
    cout << ("transaction_id: ") << (this->transaction_id) << endl;
    cout << ("transaction_block: ") << endl;
    cout << this->transactionBlock.print();
}


BlockStructure::BlockStructure() = default;

void                BlockStructure::set_transactions(list<Transaction> transact){ this->transactions = move(transact); }
void                BlockStructure::set_transaction_block(TransactionBlock transactBlock){ this->transactionBlock = move(transactBlock); }
void                BlockStructure::set_timestamp(string ts) { this->timestamp = move(ts); }
string              BlockStructure::get_timestamp() { return this->timestamp; }
void                BlockStructure::set_transaction_ids(list<int> tr_ids){ copy(tr_ids.begin(), tr_ids.end(), back_inserter(this->transaction_ids)); }
void                BlockStructure::set_transaction_block(const TransactionBlock& tb){ this->transactionBlock = tb; }
TransactionBlock    BlockStructure::get_transaction_block(){ return this->transactionBlock; }
void                BlockStructure::set_witness_name(string wn){ this->witness_name = move(wn); }
string              BlockStructure::get_witness_name(){ return this->witness_name; }
list<Transaction>   BlockStructure::get_transactions(){ return this->transactions; }

void BlockStructure::print_block(){
    cout << ("BLOCK\n");
    cout << ("timestamp: ") << (this->timestamp) << "\n";
    cout << ("transactions: ") << "\n";
    cout << ("Num transactions: ") << (this->transactions.size()) << "\n";
    for (Transaction t : this->transactions)
        t.print();
    cout << ("witness_name: ") << (this->witness_name) << "\n";
    cout << endl;
}
