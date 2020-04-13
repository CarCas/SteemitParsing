//
// Created by Carmine Caserio on 20/12/2019.
//
#include <string>
#include <unordered_map>
#include <deque>
#include <iostream>
#include <list>

//#include "CommentHub.hpp"
#include "constants.hpp"

using namespace std;

enum ReputationClasses{Whale, Shark, Dolphin, Minnow, Plankton, Error};

// CommentHub and User's classes!!

class CommentHub;

class User{


private:

    unordered_map<string, string>       info;
    
    
    ReputationClasses                   reputation_class;
    float                               steem_pow;
    double                              rep;
    
    
    string                              name;


    unordered_map<string, CommentHub*>  comments_hub;
    

    int                                 no_post;
    int                                 no_comment;
    int                                 no_reblog;
    
    list<User*>                         followers;
    list<User*>                         following;

    int                                 no_followers_bots;
    int                                 no_following_bots;

    int                                 no_followers_users;
    int                                 no_following_users;


    bool                                isbot;


public:
    User(unordered_map<string, string> info, bool isbot) : isbot(isbot){
        if (!info.empty()){
            this->info = move(info);
            this->name = this->info["name"];
            string amount = this->info["vesting_shares"];
            amount = amount.substr(0, amount.find(" "));
            this->steem_pow = stof(amount) * VS_TO_SP;
            this->reputation_class = compute_rep(this->steem_pow);
            this->rep = stod(this->info["reputation"]);
            this->no_post = 0;
            this->no_comment = 0;
            this->no_reblog = 0;
            this->no_followers_bots = 0;
            this->no_following_bots = 0;
            this->no_followers_users = 0;
            this->no_following_users = 0;
        } else {
            this->name = "";
        }
    }

    bool is_bot(){ return this->isbot; }

    void increase_num_post()   { no_post++;    }
    void increase_num_comment(){ no_comment++; }
    void increase_num_reblog() { no_reblog++;  }

    int get_no_post()   { return no_post;    }
    int get_no_comment(){ return no_comment; }
    int get_no_reblog() { return no_reblog;  }

    void print(){
        for(auto& it : this->info)
            cout << it.first << ": " << it.second;
    }

    ReputationClasses compute_rep(float steem_power){
        if (steem_power >    WHALE_LOWER)  return Whale;
        if (steem_power >    SHARK_LOWER)  return Shark;
        if (steem_power >  DOLPHIN_LOWER)  return Dolphin;
        if (steem_power >   MINNOW_LOWER)  return Minnow;
        if (steem_power > PLANKTON_LOWER)  return Plankton;
        else                               return Error;
    }

    ReputationClasses rep_class()               { return this->reputation_class; }
    string            rep_class_name()          {
        std::string s("Error");
        switch (this->reputation_class){
            case Whale:    { s = "Whale";    } break;
            case Shark:    { s = "Shark";    } break;
            case Dolphin:  { s = "Dolphin";  } break;
            case Minnow:   { s = "Minnow";   } break;
            case Plankton: { s = "Plankton"; } break;
            default: {
                break; 
            }
        }
        return s;
    }
    void rep_class(ReputationClasses rep_class) { this->reputation_class = rep_class; }

    int  steem_power()       { return this->steem_pow; }
    void steem_power(int sp) { this->steem_pow = sp; }
    int  reputation()        { return this->rep; }
    void reputation(int r)   { this->rep = r; }

    void add_comment_hub(string& permlink, CommentHub * ch) { this->comments_hub.insert({permlink, ch}); }

    CommentHub * get_single_comment_hub(string &permlink) {
        auto search = this->comments_hub.find(permlink);
        return (search != this->comments_hub.end()) ? search->second : nullptr;
    }
    
    unordered_map<string, CommentHub*> * get_whole_comments_hub(){ return &this->comments_hub; }

    void add_follower(User* u) { 
        this->followers.push_back(u);
        if(u->get_name().find((string)"bot") || u->is_bot())  no_followers_bots++;
        else                                                  no_followers_users++;
    }

    void add_following(User* u) { 
        this->following.push_back(u);
        if(u->get_name().find((string)"bot") || u->is_bot())  no_following_bots++;
        else                                                  no_following_users++;
    }
    
    list<User*> get_followers(){ return this->followers; }
    list<User*> get_following(){ return this->following; }

    double get_bot_ratio_followers() { return this->no_followers_bots / this->followers.size(); }
    double get_bot_ratio_following() { return this->no_following_bots / this->following.size(); }

    string get_id()                        { return this->info["id"                         ]; }
    string get_name()                      { return this->info["name"                       ]; }
    string get_owner()                     { return this->info["owner"                      ]; }
    string get_posting()                   { return this->info["posting"                    ]; }
    string get_json_metadata()             { return this->info["json_metadata"              ]; }
    string get_last_owner_update()         { return this->info["last_owner_update"          ]; }
    string get_last_account_update()       { return this->info["last_account_update"        ]; }
    string get_created()                   { return this->info["created"                    ]; }
    string get_mined()                     { return this->info["mined"                      ]; }
    string get_comment_count()             { return this->info["comment_count"              ]; }
    string get_lifetime_vote_count()       { return this->info["lifetime_vote_count"        ]; }
    string get_post_count()                { return this->info["post_count"                 ]; }
    string get_can_vote()                  { return this->info["can_vote"                   ]; }
    string get_balance()                   { return this->info["balance"                    ]; }
    string get_savings_balance()           { return this->info["savings_balance"            ]; }
    string get_sbd_balance()               { return this->info["sbd_balance"                ]; }
    string get_savings_sbd_balance()       { return this->info["savings_sbd_balance"        ]; }
    string get_savings_withdraw_requests() { return this->info["savings_withdraw_requests"  ]; }
    string get_reward_sbd_balance()        { return this->info["reward_sbd_balance"         ]; }
    string get_reward_steem_balance()      { return this->info["reward_steem_balance"       ]; }
    string get_reward_vesting_balance()    { return this->info["reward_vesting_balance"     ]; }
    string get_reward_vesting_steem()      { return this->info["reward_vesting_steem"       ]; }
    string get_vesting_shares()            { return this->info["vesting_shares"             ]; }
    string get_delegated_vesting_shares()  { return this->info["delegated_vesting_shares"   ]; }
    string get_received_vesting_shares()   { return this->info["received_vesting_shares"    ]; }
    string get_vesting_withdraw_rate()     { return this->info["vesting_withdraw_rate"      ]; }
    string get_next_vesting_withdrawal()   { return this->info["next_vesting_withdrawal"    ]; }
    string get_curation_rewards()          { return this->info["curation_rewards"           ]; }
    string get_posting_rewards()           { return this->info["posting_rewards"            ]; }
    string get_proxied_vsf_votes()         { return this->info["proxied_vsf_votes"          ]; }
    string get_witnesses_voted_for()       { return this->info["witnesses_voted_for"        ]; }
    string get_last_post()                 { return this->info["last_post"                  ]; }
    string get_last_root_post()            { return this->info["last_root_post"             ]; }
    string get_last_vote_time()            { return this->info["last_vote_time"             ]; }

    //~User(){
        /*for(auto el : this->comments_hub){
            delete el.second;
        }*/
    //    unordered_map<string, CommentHub*>().swap(this->comments_hub);
    //    unordered_map<string, string>().swap(this->info);
    //    list<User*>().swap(this->followers);
    //    list<User*>().swap(this->following);
    //}

    /*
    string get_active()                             { return this->info["active"                            ]; }
    string get_memo_key()                           { return this->info["memo_key"                          ]; }
    string get_proxy()                              { return this->info["proxy"                             ]; }
    string get_recovery_account()                   { return this->info["recovery_account"                  ]; }
    string get_last_account_recovery()              { return this->info["last_account_recovery"             ]; }
    string get_reset_account()                      { return this->info["reset_account"                     ]; }
    string get_voting_manabar()                     { return this->info["voting_manabar"                    ]; }
    string get_sbd_seconds()                        { return this->info["sbd_seconds"                       ]; }
    string get_sbd_seconds_last_update()            { return this->info["sbd_seconds_last_update"           ]; }
    string get_sbd_last_interest_payment()          { return this->info["sbd_last_interest_payment"         ]; }
    string get_savings_sbd_seconds()                { return this->info["savings_sbd_seconds"               ]; }
    string get_savings_sbd_seconds_last_update()    { return this->info["savings_sbd_seconds_last_update"   ]; }
    string get_savings_sbd_last_interest_payment()  { return this->info["savings_sbd_last_interest_payment" ]; }
    string get_withdrawn()                          { return this->info["withdrawn"                         ]; }
    string get_to_withdraw()                        { return this->info["to_withdraw"                       ]; }
    string get_withdraw_routes()                    { return this->info["withdraw_routes"                   ]; }
    string get_post_bandwidth()                     { return this->info["post_bandwidth"                    ]; }
    string get_pending_claimed_accounts()           { return this->info["pending_claimed_accounts"          ]; }
    string get_is_smt()                             { return this->info["is_smt"                            ]; }
    */
};



class CommentHub {

private:
    std::string delim_row = string(1, DELIMITER) + DELIMITER;

    struct weighted_user{
        string user;
        int weight;
    };

    string comment = "";
    //string permlink = "";
    User* author;
    CommentHub* father;
    list<CommentHub> comment_tree;
    list<string> reblogs_user;
    list<weighted_user> votes_user;
    
    string tags = "";
    string howpublished = "";

    //int no_bots_reblogs = 0;
    //int no_users_reblogs = 0;

    //int no_bot_votes = 0;
    //int no_user_votes = 0;

    //int weight_bot_votes = 0;
    //int weight_user_votes = 0;

    bool already_written;

public:
    /*CommentHub () {
        this->father = new CommentHub("");
        this->comment_tree = new list<CommentHub>();
        this->reblogs_user = list<string>();
    }*/
    
    CommentHub (const string& comment, User* author) {
        this->comment = comment;
        this->author = author;
        this->father = nullptr;
        this->already_written = false;
    }
    CommentHub (string& comment, User* author, CommentHub* father) {
        this->comment = comment;
        this->author = author;
        this->father = father;
        this->already_written = false;
    }
    //CommentHub (string comment, string permlink) : comment(move(comment)), permlink(move(permlink)){}

    void         set_how_published(string hp)                { this->howpublished = move(hp); }
    void         set_tags(string t)                          { this->tags = move(t); }

    string       get_how_published()                         { return this->howpublished; }
    string       get_tags()                                  { return this->tags; }
    
//    int          get_no_votes()                              { return this->no_bot_votes + no_user_votes; }
    
//    int          get_no_votes_users()                        { return this->no_user_votes; }
//    int          get_no_votes_bots()                         { return this->no_bot_votes; }

//    int          get_weights()                               { return this->weight_user_votes + this->weight_bot_votes; }
    
//    int          get_weight_users()                          { return this->weight_user_votes; }
//    int          get_weight_bots()                           { return this->weight_bot_votes; }
    

    User*        get_author()                                { return this->author; }
    string       get_comment()                               { return this->comment; }
    // string    get_permlink()                              { return this->permlink; }
    void         add_in_comment_tree(CommentHub& cmt_hub)    { comment_tree.push_back(cmt_hub); }
    list         <CommentHub> get_comment_hub_tree()         { return this->comment_tree; }
    

    void         add_reblog(string u, bool is_bot)           { this->reblogs_user.push_back(u); }//if (is_bot) no_bots_reblogs++; else no_users_reblogs++; }
    list<string> get_reblogs()                               { return this->reblogs_user; }

    //int          get_no_reblogs_users()                      { return this->no_users_reblogs; }
    //int          get_no_reblogs_bots()                       { return this->no_bots_reblogs; }


    CommentHub*  get_father_comment_hub()                    { return this->father; }
    void         set_father_comment_hub(CommentHub * father) { this->father = father; }

    void add_vote(string username, int weight, bool is_bot) {
        //if (is_bot) { no_bot_votes++; weight_bot_votes += weight; }
        //else {no_user_votes++; weight_user_votes += weight;}
        this->votes_user.push_back({username, weight});
    }

    bool is_already_written(){ return this->already_written; }

    string get_relevant_comments() {
        string toret = "";
        this->already_written = true;
        for (auto c : this->comment_tree){
            string header = c.author->get_name() + DELIMITER;
            string body = "";
            string trailing = "";
            if(c.get_comment().find_first_not_of(" ") != std::string::npos)
                body = c.get_comment() + DELIMITER;
            if(body != ""){
                for(auto s : this->votes_user)
                    trailing = trailing + s.user + DELIMITER + to_string(s.weight) + DELIMITER;
                for(string s : this->reblogs_user)
                    trailing = trailing + s + DELIMITER;
                toret = toret + header + body + c.tags + DELIMITER + trailing + delim_row;
            }
        }
        return toret;
    }

    /*
    void swap(CommentHub& ch){
        CommentHub tmp = move(ch);
        ch = *this;
        *this = tmp; 
    }
    */
    /*~CommentHub(){
        delete this->author;
        if(this->father != nullptr)
            delete this->father;
        list<CommentHub>().swap(comment_tree);
        list<string>().swap(reblogs_user);
    }*/
    
};