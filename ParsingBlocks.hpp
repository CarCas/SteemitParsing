//
// Created by Carmine Caserio on 20/12/2019.
//

#include <thread>
#include <future>
#include <fstream>
#include <algorithm>
#include <map>
#include <filesystem>
#include <sstream>

#include "constants.hpp"
#include "UserStructure.hpp"
#include "BlockStructure.hpp"
#include "clock.hpp"

#include "rapidjson/rapidjson.h"
#include "rapidjson/document.h"
#include "rapidjson/error/en.h"
#include "rapidjson/schema.h"

using namespace rapidjson;

using ull = unsigned long long;
using ui = unsigned int;

class ParsingBlocks {
private:
    std::string delim_row = string(1, DELIMITER) + DELIMITER;
    struct ordered_by_how_publish {
        std::string name;
        ui count;

        bool operator < (const ordered_by_how_publish& str) const { return (count < str.count); }
        bool operator > (const ordered_by_how_publish& str) const { return (count > str.count); }
    };

    struct ordered_by_money {
        std::string name;
        ull money;

        bool operator < (const ordered_by_money& str) const { return (money < str.money); }
        bool operator > (const ordered_by_money& str) const { return (money > str.money); }
    };

    struct common_informations {
        std::string user;
        bool is_user_bot;
        std::string user_rep_class;
    };

    struct vote_element {
        std::string voter;
        std::string author;
        std::string ref_v;
        int weight;
        std::string timestamp;
    };

    //Comment *c = new Comment(parent_author, parent_permlink, author, permlink, title, body, json_m);
    struct comment_element {
        char is_post;
        std::string author;
        std::string permlink;
        std::string timestamp;
        std::string how_published;
        std::string tags;
        std::string text;
    };

    struct follow_element {
        std::string following;
        std::string follower;
    };

    struct reblog_element {
        std::string account;
        std::string author;
        std::string permlink;
    };

    vector<ordered_by_money> sbd_users;
    vector<ordered_by_money> vest_users;
    vector<ordered_by_money> balance_users;

    vector<common_informations> info_users;
    

    unordered_map<std::string, User> steemit_users;
    
    //vector<BlockStructure> blocks;
    vector<std::string>    bots;
    
    unordered_map<std::string, std::string> reputations;
    
    vector<ordered_by_how_publish> how_users_published;
    vector<ordered_by_how_publish> how_bots_published;

    vector<vote_element>    votes;
    vector<follow_element>  follows;
    vector<reblog_element>  reblogs;
    vector<comment_element> comments;

    vector<std::string> jump_transactions_id {"4fb6d6455ee6304640925711b3e1b6aaa739f507"};


    int overall_no_vote_bots = 0, overall_no_vote_users = 0;
    int overall_no_post_bots = 0, overall_no_post_users = 0;
    int overall_no_reblog_bots = 0, overall_no_reblog_users = 0;
    int overall_weight_vote_bots = 0, overall_weight_vote_users = 0;
    
    int overall_no_follow_by_bots = 0, overall_no_follow_by_users = 0;
    int overall_no_users_following_bots = 0, overall_no_bots_following_users = 0;
    int overall_no_bots_following_bots = 0, overall_no_users_following_users = 0;


    std::string users_path, rep_path, bot_path;

    enum _Operations { vote, comment, custom_json, transfer, transfer_to_vesting, withdraw_vesting, limit_order_create, limit_order_cancel, other };

    _Operations resolveOperation(std::string input){
        static const map<std::string, _Operations> operationStrings {
            { VOTE,                 vote                },
            { COMMENT,              comment             },
            { CUSTOM_JSON,          custom_json         },
            { TRANSFER,             transfer            },
            { TRANSFER_TO_VESTING,  transfer_to_vesting },
            { WITHDRAW_VESTING,     withdraw_vesting    },
            { LIMIT_ORDER_CREATE,   limit_order_create  },
            { LIMIT_ORDER_CANCEL,   limit_order_cancel  }
        };

        auto itr = operationStrings.find(input);
        if( itr != operationStrings.end() )
            return itr->second;
        return other;
    }

    std::string search_member(Value& js, std::string member){
        Value::ConstMemberIterator itr = js.FindMember(member.c_str());
        std::string els = "";
        if(itr != js.MemberEnd())
            els = parseNode(itr->value) + " ";
        return els;
    }

    std::string comment_hub_of_author(std::string author){
        list<string> to_save = all_comment_hubs(author);
        std::string sav = "";
        if (to_save.size() > 0){
            std::ostringstream stream;
            std::copy(to_save.begin(), to_save.end(), std::ostream_iterator<std::string>(stream, " "));
            sav = stream.str();
        }
        return sav;
    }

    void saving(string folder, string filepath, string tosave){
        fstream f;
        if(!std::filesystem::exists(folder))
            std::filesystem::create_directories(folder);
        string tmp = folder + "/" + filepath;
        f.open(tmp, std::fstream::in | std::fstream::out | std::fstream::app);
        if (!f)
            f.open(tmp, std::fstream::in | std::fstream::out | std::fstream::trunc);
        f << tosave << "\n";
        f.close();
    }

    double check_size(int size1, int size2){
        if(size1 + size2 == 0)
            return 0.;
        return size1 / (size1 + size2);
    }

    pair<ui, std::string> max_el_from_how_published(){
        auto pr = std::max_element(
            this->how_users_published.begin(), this->how_users_published.end(),
            [] (const ordered_by_how_publish& p1, const ordered_by_how_publish& p2) {
                return p1.count < p2.count;
            }
        );
        return pair<ui, std::string>(pr->count, pr->name);
    }

    std::vector<std::string> split(std::string str,std::string sep){
        char* cstr=const_cast<char*>(str.c_str());
        char* current;
        std::vector<std::string> arr;
        current=strtok(cstr,sep.c_str());
        while(current!=NULL){
            arr.push_back(current);
            current=strtok(NULL,sep.c_str());
        }
        return arr;
    }

    void add_to_votes_file(){
        fstream f;
        if(!std::filesystem::exists(OPERATIONS_FOLDER))
            std::filesystem::create_directories(OPERATIONS_FOLDER);
        f.open(VOTES_FILE, std::fstream::in | std::fstream::out | std::fstream::app);
        if (!f){
            f.open(VOTES_FILE, std::fstream::in | std::fstream::out | std::fstream::trunc);
            f << "voter" << DELIMITER << "author" << DELIMITER << "permlink" << DELIMITER << "weight" << DELIMITER << "timestamp" << delim_row;
        }
        for(auto vt : this->votes)
            f << vt.voter << DELIMITER << vt.author << DELIMITER << vt.ref_v << DELIMITER << vt.weight << DELIMITER << vt.timestamp << delim_row;
        f.close();
    }

    void add_to_comments_file(){
        fstream f;
        if(!std::filesystem::exists(OPERATIONS_FOLDER))
            std::filesystem::create_directories(OPERATIONS_FOLDER);
        f.open(COMMENTS_FILE, std::fstream::in | std::fstream::out | std::fstream::app);
        if (!f){
            f.open(COMMENTS_FILE, std::fstream::in | std::fstream::out | std::fstream::trunc);
            f << "is_post" << DELIMITER << "author" << DELIMITER << "permlink" << DELIMITER << "timestamp"
                << DELIMITER << "how_published" << DELIMITER << "tags" << DELIMITER << "text" << delim_row;
        }
        for (auto cmt : this->comments)
            f << cmt.is_post << DELIMITER << cmt.author << DELIMITER << cmt.permlink << DELIMITER << cmt.timestamp 
                << DELIMITER << cmt.how_published << DELIMITER << cmt.tags << DELIMITER << cmt.text << delim_row;
        /*
        for(auto us : this->steemit_users){
            unordered_map<std::string, CommentHub*> * curr = us.second.get_whole_comments_hub();
            for(auto ch : *curr){
                std::string permlink = ch.first;
                CommentHub* c = ch.second;
                f << string("p") << DELIMITER << us.first << DELIMITER << permlink << DELIMITER << c->get_comment() << delim_row;
                for(CommentHub child : c->get_comment_hub_tree())
                    f << string("c") << DELIMITER << child.get_author()->get_name() << DELIMITER << permlink << DELIMITER << child.get_comment() << delim_row;
            }
        }*/
        f.close();
    }

    void add_to_common_infos_file(){
        fstream f;
        if(!std::filesystem::exists(OPERATIONS_FOLDER))
            std::filesystem::create_directories(OPERATIONS_FOLDER);
        f.open(COMMON_INFO_FILE, std::fstream::in | std::fstream::out | std::fstream::app);
        if (!f){
            f.open(COMMON_INFO_FILE, std::fstream::in | std::fstream::out | std::fstream::trunc);
            f << "user" << DELIMITER << "is_bot" << DELIMITER << "rep_class" << delim_row;
        }
        for(auto us : this->info_users)
            f << us.user << DELIMITER << us.is_user_bot << DELIMITER << us.user_rep_class << delim_row;
        f.close();
    }

    void add_to_follows_file(){
        fstream f;
        if(!std::filesystem::exists(OPERATIONS_FOLDER))
            std::filesystem::create_directories(OPERATIONS_FOLDER);
        f.open(FOLLOWS_FILE, std::fstream::in | std::fstream::out | std::fstream::app);
        if (!f){
            f.open(FOLLOWS_FILE, std::fstream::in | std::fstream::out | std::fstream::trunc);
            f << "following" << DELIMITER << "follower"  << delim_row;
        }
        for(auto fs : this->follows)
            f << fs.following << DELIMITER << fs.follower << delim_row;
        f.close();
    }

    void add_to_reblogs_file(){
        fstream f;
        if(!std::filesystem::exists(OPERATIONS_FOLDER))
            std::filesystem::create_directories(OPERATIONS_FOLDER);
        f.open(REBLOGS_FILE, std::fstream::in | std::fstream::out | std::fstream::app);
        if (!f){
            f.open(REBLOGS_FILE, std::fstream::in | std::fstream::out | std::fstream::trunc);
            f << "reblogger" << DELIMITER << "author" << DELIMITER << "permlink" << delim_row;
        }
        for(auto r : this->reblogs)
            f << r.account << DELIMITER << r.author << DELIMITER << r.permlink << delim_row;
        f.close();
    }


    void add_to_users_file(string users_path, vector<ordered_by_money> us_array){
        fstream f;
        if(!std::filesystem::exists(FIRST_N_USERS_FOLDER))
            std::filesystem::create_directories(FIRST_N_USERS_FOLDER);
        string tmp = FIRST_N_USERS_FOLDER + "/" + users_path;
        f.open(tmp, std::fstream::in | std::fstream::out | std::fstream::app);
        if (!f)
            f.open(tmp, std::fstream::in | std::fstream::out | std::fstream::trunc);
        for(auto r : us_array)
            f << r.name << ", " << r.money << "\n";
        f.close();
    }

    std::string check_presence_for_strings(Value& v, std::string el){
        Value::ConstMemberIterator itr = v.FindMember(el.c_str());
        if(itr != v.MemberEnd())
            return parseNode(itr->value);
        return "";
    }

    int check_presence_for_ints(Value& v, std::string el){
        Value::ConstMemberIterator itr = v.FindMember(el.c_str());
        if(itr != v.MemberEnd())
            return itr->value.GetInt();
        return -1;
    }

public:
    ParsingBlocks(std::string users_path, std::string rep_path, std::string bot_path) : 
        users_path(move(users_path)), rep_path(move(rep_path)), bot_path(move(bot_path)) {}


    User * read_single_user(const char * us){
        Document root;
        root.Parse<0>(us);
        auto search = reputations.find(root["name"].GetString());
        auto bot_search = std::find(this->bots.begin(), this->bots.end(), root["name"].GetString());
        if(search == reputations.end()){
            std::cout << root["name"].GetString() << " is not present in the reputations array!" << endl;
            return (User*)nullptr;
        }
        unordered_map<std::string, std::string> community {
            { "id",                             parseNode(root["id"                       ])},
            { "name",                           parseNode(root["name"                     ])}, 
            //{ "owner",                          parseNode(root["owner"                    ])},
            { "reputation",                     search->second                              },
            //{ "posting",                        parseNode(root["posting"                  ])},
            //{ "follower_count",                 parseNode(root["follower_count"           ])},
            { "json_metadata",                  parseNode(root["json_metadata"            ])}, 
            { "last_owner_update",              parseNode(root["last_owner_update"        ])},
            { "last_account_update",            parseNode(root["last_account_update"      ])},
            { "created",                        parseNode(root["created"                  ])},
            { "mined",                          parseNode(root["mined"                    ])},
            { "comment_count",                  parseNode(root["comment_count"            ])},
            { "lifetime_vote_count",            parseNode(root["lifetime_vote_count"      ])},
            { "post_count",                     parseNode(root["post_count"               ])},
            { "can_vote",                       parseNode(root["can_vote"                 ])},
            { "balance",                        parseNode(root["balance"                  ])},
            { "savings_balance",                parseNode(root["savings_balance"          ])}, 
            { "sbd_balance",                    parseNode(root["sbd_balance"              ])}, 
            { "savings_sbd_balance",            parseNode(root["savings_sbd_balance"      ])}, 
            { "savings_withdraw_requests",      parseNode(root["savings_withdraw_requests"])}, 
            { "reward_sbd_balance",             parseNode(root["reward_sbd_balance"       ])}, 
            { "reward_steem_balance",           parseNode(root["reward_steem_balance"     ])}, 
            { "reward_vesting_balance",         parseNode(root["reward_vesting_balance"   ])}, 
            { "reward_vesting_steem",           parseNode(root["reward_vesting_steem"     ])}, 
            { "vesting_shares",                 parseNode(root["vesting_shares"           ])}, 
            { "delegated_vesting_shares",       parseNode(root["delegated_vesting_shares" ])}, 
            { "received_vesting_shares",        parseNode(root["received_vesting_shares"  ])}, 
            { "vesting_withdraw_rate",          parseNode(root["vesting_withdraw_rate"    ])}, 
            { "next_vesting_withdrawal",        parseNode(root["next_vesting_withdrawal"  ])}, 
            { "curation_rewards",               parseNode(root["curation_rewards"         ])}, 
            { "posting_rewards",                parseNode(root["posting_rewards"          ])}, 
            //{ "proxied_vsf_votes",              parseNode(root["proxied_vsf_votes"        ])}, 
            { "witnesses_voted_for",            parseNode(root["witnesses_voted_for"      ])}, 
            { "last_post",                      parseNode(root["last_post"                ])}, 
            { "last_root_post",                 parseNode(root["last_root_post"           ])}, 
            { "last_vote_time",                 parseNode(root["last_vote_time"           ])}, 
        };
        return (bot_search != this->bots.end()) ? new User(community, true) : new User(community, false);
    }

    std::string get_perm_string(std::string permlink){
        std::vector<std::string> link = split(permlink, "-");
        if(link.size() == 0)
            return permlink;
        for(unsigned int i = 0; i < link.size()-1; i++)
            if (link[i].compare(link[i+1]) == 0)
                link[i] = " ";
        unsigned int i = 0;
        while(i < link.size()){
            if(link[i].compare("re") == 0){
                unsigned int j = i+1;
                while(j < link.size())
                    if(link[j] == " ") j++;
                    else break;
                link[i] = link[j] = " ";
                i = j+1;
            } else{
                i++;
                if(i < link.size()){
                    if(link[i].compare("re") == 0) link[i] = " "; 
                    else break;
                }
            }
        }
        i = link.size() - 1;
        bool b = true;
        while(b){
            if (i > 0){
                auto lk = link[i];
                if((find(lk.begin(), lk.end(), 't') != lk.end()) || (find(lk.begin(), lk.end(), 'z') != lk.end())){
                    lk.erase(std::remove(lk.begin(), lk.end(), 't'), lk.end());
                    lk.erase(std::remove(lk.begin(), lk.end(), 'z'), lk.end());
                    if(lk.size() > 0){
                        b = all_of(lk.begin(), lk.end(), ::isdigit);
                        if(b) link[i--] = " ";
                    } else b = false;
                } else b = false;
            } else b = false;
        }
        permlink = "";
        link.erase(std::remove(link.begin(), link.end(), " "), link.end());
        i = 0;
        while(i < link.size())
            permlink = (i < link.size() - 1) ? permlink + link[i++] + "-" : permlink + link[i++];
        return permlink;
    }

    //void switch_operations(std::string op, Value *op_struct, vector<Operation*>* op_array){
    void switch_operations(std::string op, Value *op_struct, std::string timestamp, std::string bid){
        switch (resolveOperation(op)) {
            case vote: {
                //cout << "vote" << endl;
                std::string voter = (*op_struct)["voter"].GetString();
                std::string author = (*op_struct)["author"].GetString();
                std::string ref_v  = get_perm_string((*op_struct)["permlink"].GetString());
                if((voter.compare("") == 0) || (author.compare("") == 0) || (ref_v.compare("") == 0))
                    break;
                try {
                    User * current = &steemit_users.at(author);
                    if (current->get_single_comment_hub(ref_v) != nullptr){
                        int weight = (*op_struct)["weight"].GetInt();
                        bool vtr_is_bot = steemit_users.at(voter).is_bot();
                        //Vote * v = new Vote((*op_struct)["voter"].GetString(), author, ref_v, (*op_struct)["weight"].GetInt(), timestamp);
                        //op_array->push_back((Operation*)v);
                        votes.push_back({voter, author, ref_v, weight, timestamp});
                        current->get_single_comment_hub(ref_v)->add_vote(voter, weight, vtr_is_bot);
                        if(vtr_is_bot) { overall_no_vote_bots++;  overall_weight_vote_bots  += weight; }
                        else           { overall_no_vote_users++; overall_weight_vote_users += weight; }
                    }
                } catch(const std::out_of_range &e){
                    break;
                }
                break;
            }
            case comment: {
                //cout << "comment" << endl;
                if(op_struct->IsObject()){
                    std::string parent_author   =          check_presence_for_strings(*op_struct, "parent_author");
                    std::string author          =          check_presence_for_strings(*op_struct, "author");
                    std::string permlink = get_perm_string(check_presence_for_strings(*op_struct, "permlink"));
                    std::string title           =          check_presence_for_strings(*op_struct, "title");
                    std::string body            =          check_presence_for_strings(*op_struct, "body");
                    std::string json_m          =          check_presence_for_strings(*op_struct, "json_metadata");
                    if (author.compare("") == 0 || permlink.compare("") == 0)
                        break;
                    //Comment *c = new Comment(parent_author, parent_permlink, author, permlink, title, body, json_m);
                    //op_array->push_back((Operation*)c);
                    std::string community = "";
                    std::string hp = "";
                    std::string tag = "";
                    if((json_m.compare("") != 0) && (json_m.compare("{}") != 0) && (json_m.compare("\"\"") != 0)){
                        Document js;
                        js.Parse<0>(json_m.c_str());
                        community = search_member(js, "community");
                        tag = search_member(js, "tags");
                        hp = search_member(js, "app");
                        
                    }
                    User * auth = &steemit_users.at(author);
                    CommentHub * current_ch = new CommentHub(title + " " + body + " " + community, auth);
                    char ispost = (parent_author == "") ? 'p' : 'c';
                    this->comments.push_back({ispost, author, permlink, timestamp, hp, tag, title + " " + body + " " + community});
                    current_ch->set_how_published(hp);
                    current_ch->set_tags(tag);
                    // se è un commento, aggiungo il commento al tree dei commenti che partono dal post
                    if (parent_author != ""){
                        std::string parent_permlink = get_perm_string(check_presence_for_strings(*op_struct, "parent_permlink"));
                        CommentHub * parent_ch = steemit_users.at(parent_author).get_single_comment_hub(parent_permlink);
                        if(parent_ch != nullptr){
                            current_ch->set_father_comment_hub(parent_ch);
                            while(parent_ch->get_father_comment_hub() != nullptr)
                                parent_ch = parent_ch->get_father_comment_hub();
                            parent_ch->add_in_comment_tree(*current_ch);
                        }
                        auth->increase_num_comment();
                    } else {
                        auth->increase_num_post();
                        if (auth->is_bot())   overall_no_post_bots++;
                        else                  overall_no_post_users++;
                    }
                    if (auth->is_bot()) {
                        auto search = find_if(
                            how_bots_published.begin(), 
                            how_bots_published.end(), 
                            [&chp = hp](const ordered_by_how_publish& hbp) -> bool { return chp.compare(hbp.name) == 0; }
                        );
                        if(search != how_bots_published.end())
                            search->count++;
                    } else {
                        auto search = find_if(
                            how_users_published.begin(), 
                            how_users_published.end(), 
                            [&chp = hp](const ordered_by_how_publish& hup) -> bool { return chp.compare(hup.name) == 0; }
                        );
                        if(search != how_users_published.end())
                            search->count++;
                    }
                    // se è un post o se è un commento aggiungo il post/commento nei contenuti prodotti dall'utente
                    auth->add_comment_hub(permlink, current_ch);
                }
                break;
            }
            case custom_json: {
                Value::ConstMemberIterator itr = op_struct->FindMember("id");
                if(itr == op_struct->MemberEnd())                          break;
                if(strcmp(parseNode(itr->value).c_str(), "follow") != 0)   break;
                vector<std::string> req_posting_auths;
                Value::ConstMemberIterator itr2 = op_struct->FindMember("required_posting_auths");
                if(itr2 != op_struct->MemberEnd()){
                    if(itr2->value.IsArray()){
                        for(SizeType i = 0; i < itr2->value.Size(); i++)
                            req_posting_auths.push_back( parseNode(itr2->value[i]) );
                    }
                }
                Document json_el;
                std::string j = check_presence_for_strings(*op_struct, "json");
                if(j.compare("") == 0)
                    break;
                json_el.Parse<0>(j.c_str());
                if(json_el.IsArray()){
                    if(json_el[0].IsString()){
                        if(strcmp(json_el[0].GetString(), "follow") == 0){
                            //cout << "follow" << endl;
                            if(json_el[1].IsObject()){
                                std::string follower  = json_el[1]["follower" ].GetString();
                                std::string following = json_el[1]["following"].GetString();
                                User * fing = nullptr;
                                User * fer = nullptr;
                                auto search = steemit_users.find(following);
                                if(search != steemit_users.end())
                                    fing = &search->second;
                                auto search2 = steemit_users.find(follower);
                                if(search != steemit_users.end())
                                    fer = &search2->second;
                                if((fing != nullptr) && (fer != nullptr)){
                                    //Value& wh = json_el[1]["what"];
                                    //vector<std::string> what;
                                    //for (Value::ConstValueIterator itr = wh.Begin(); itr != wh.End(); itr++)
                                    //    what.push_back(itr->GetString());
                                    //op_array->push_back((Operation*) new CustomJsonFollow(follower, following, what, req_posting_auths));
                                    //try { steemit_users.at(following).add_follower(&steemit_users.at(follower)); } // User following is followed by user follower
                                    //catch (const out_of_range& err){ cerr << err.what() << endl; }
                                    //try { steemit_users.at(follower).add_following(&steemit_users.at(following)); } // User follower follows user following 
                                    //catch (const out_of_range& err){ cerr << err.what() << endl; }
                                    follows.push_back({following, follower});
                                    fing->add_follower(fer); // follower follows following
                                    fer->add_following(fing); // following is followed by follower
                                    if(fer->is_bot()){
                                        if(fing->is_bot()) this->overall_no_bots_following_bots++;
                                        else               this->overall_no_bots_following_users++;
                                        this->overall_no_follow_by_bots++;
                                    } else {
                                        if(fing->is_bot()) this->overall_no_users_following_bots++;
                                        else               this->overall_no_users_following_users++;
                                        this->overall_no_follow_by_users++;
                                    }
                                    break;
                                } else {
                                    cout << "following: " << following << " is not in the users list" << endl;
                                    cout << "follower: " << follower << " is not in the users list" << endl;
                                    break;
                                }
                            } else cout << "is not object" << endl;
                            //cout << "end follow" << endl;
                            break;
                        } else if (strcmp(json_el[0].GetString(), "reblog") == 0){
                            if(json_el[1].IsObject()){
                                std::string account  = json_el[1]["account" ].GetString();
                                std::string author   = json_el[1]["author"  ].GetString();

                                auto search = steemit_users.find(account);
                                if(search != steemit_users.end()){
                                    auto search2 = steemit_users.find(author);
                                    if(search2 != steemit_users.end()){
                                        if((&search->second != nullptr) && (&search2->second != nullptr)){
                                            User * raccount = &search->second;
                                            User * rauthor = &search2->second;
                                            std::string permlink = get_perm_string(json_el[1]["permlink"].GetString());
                                            reblogs.push_back({account, author, permlink});
                                            //op_array->push_back((Operation*) new CustomJsonReblog(account, author, permlink, req_posting_auths));
                                            try { 
                                                CommentHub * ch = rauthor->get_single_comment_hub(permlink);
                                                if (ch != nullptr)
                                                    ch->add_reblog(account, raccount->is_bot());
                                                if (raccount->is_bot())  overall_no_reblog_bots++;
                                                else                     overall_no_reblog_users++;
                                            }
                                            catch(const out_of_range& err) { cerr << err.what() << endl; }
                                        } else {
                                            cout << "one of the two accounts involved in the reblog are not in the steemit_users: account: " << account << "\t author: " << author << endl; 
                                            break;
                                        }
                                    }

                                }
                            }
                            break;
                            //cout << "end reblog" << endl;
                        }
                    }

                } else
                    cout << "json_el is not an array, type: " << (*op_struct)["json"].GetType() << endl;
                //cout << "end custsom_json" << endl;
                break;
            }
            /*
            case transfer_to_vesting: {
                //op_array->push_back((Operation*) new TransferToVesting(
                //    (*op_struct)["from"].GetString(), (*op_struct)["to"].GetString(), (*op_struct)["amount"].GetString()));
                break;
            }
            case transfer: {
                //op_array->push_back((Operation*) new Transfer(
                //    (*op_struct)["from"].GetString(), (*op_struct)["to"].GetString(), (*op_struct)["amount"].GetString(), (*op_struct)["memo"].GetString()));
                break;
            }
            case withdraw_vesting: {
                //op_array->push_back((Operation*) new WithdrawVesting((*op_struct)["account"].GetString(), (*op_struct)["vesting_shares"].GetString()));
                break;
            }
            case limit_order_create: {
                //op_array->push_back((Operation*) new LimitOrderCreate(
                //    (*op_struct)["owner"].GetString(),          (*op_struct)["orderid"].GetInt64(),     (*op_struct)["amount_to_sell"].GetString(), 
                //    (*op_struct)["min_to_receive"].GetString(), (*op_struct)["fill_or_kill"].GetBool(), (*op_struct)["expiration"].GetString()));
                break;
            }
            case limit_order_cancel: {
                //op_array->push_back((Operation*) new LimitOrderCancel((*op_struct)["owner"].GetString(), (*op_struct)["orderid"].GetInt64()));
                break;
            }
            */
            default: {
                break;
            }
        }
    }

    //vector<Operation*> fill_ops(Value& trans_operations){
    void fill_ops(Value& trans_operations, std::string tr_timestamp, string bid){
        //vector<Operation*> op_array = {};
        for(SizeType i = 0; i < trans_operations.Size(); i++){
            std::string transformed = trans_operations[i][0].GetString();
            transform(transformed.begin(), transformed.end(), transformed.begin(), ::toupper);
            //switch_operations(transformed, &trans_operations[i][1], &op_array);
            switch_operations(transformed, &trans_operations[i][1], tr_timestamp, bid);
        }
        //return op_array;
    }

    void evaluate_block(const char* line){
        Document root;
        root.Parse<0>(line);
        //BlockStructure * bs = new BlockStructure();
        //bs->set_timestamp   (parseNode(root["timestamp"]));
        //bs->set_witness_name(parseNode(root["witness"  ]));
        //list<Transaction> * transss = new list<Transaction>();
        if(root["transactions"].IsArray()){
            for(SizeType i = 0; i < root["transactions"].Size(); i++){
                string bd = parseNode(root["block_id"]);
                Value& transact = root["transactions"][i];
                if(transact.HasMember("operations")){
                    //vector<Operation*> operations = fill_ops(transact["operations"]);
                    //if(!operations.empty())
                    //    t_block->set_operations(operations);
                    if (std::find(this->jump_transactions_id.begin(), this->jump_transactions_id.end(), 
                            transact["transaction_id"].GetString()) == this->jump_transactions_id.end())
                        //fill_ops(transact["operations"], bs->get_timestamp(), transact["transaction_id"].GetString());
                        fill_ops(transact["operations"], parseNode(root["timestamp"]), transact["transaction_id"].GetString());
                }

                //TransactionBlock * t_block = new TransactionBlock();
                //std::string tr_id = "";
                //Value::ConstMemberIterator itr = transact.FindMember("transaction_num");
                //if(itr != transact.MemberEnd())
                //    t_block->set_transaction_num(itr->value.GetInt());
                //Value::ConstMemberIterator itr2 = transact.FindMember("transaction_id");
                //if(itr2 != transact.MemberEnd())
                //    tr_id = itr2->value.GetString();
                //Transaction * t = new Transaction(tr_id, *t_block);
                //transss->push_back(*t);
            }
        }
        //bs->set_transactions(*transss);
        //bs->print_block();
        //return *bs;
    }

    static std::string parseNode(const Value &node){
        string toret = "";
        if (node.IsBool())          toret = toret + to_string(node.GetBool());
        else if (node.IsInt())      toret = toret + to_string(node.GetInt());
        else if (node.IsUint())     toret = toret + to_string(node.GetUint());
        else if (node.IsInt64())    toret = toret + to_string(node.GetInt64());
        else if (node.IsUint64())   toret = toret + to_string(node.GetUint64());
        else if (node.IsDouble())   toret = toret + to_string(node.GetDouble());
        else if (node.IsString())   toret = toret + node.GetString();
        else if (node.IsArray())    toret = toret + parseArray(node);
        else if (node.IsObject())   toret = toret + parseObject(node);
        return toret;
    }

    static std::string parseObject(const Value &node){
        string toret = "";
        for (Value::ConstMemberIterator childNode = node.MemberBegin(); childNode != node.MemberEnd(); childNode++)
            toret = toret + parseNode(childNode->value);
        return toret;
    }

    static std::string parseArray(const Value& node){
        string toret = "";
        for (SizeType i = 0; i < node.Size(); i++)
            toret = toret + parseNode(node[i]);
        return toret;
    }

    void parsing_users(){
        std::string line;
        ifstream rep_file;
        rep_file.open(this->rep_path);
        if(!rep_file.is_open()){
            perror("Error opening blocks, users or reputations file");
            exit(EXIT_FAILURE);
        }
        auto read_reps = [&](){
            std::string rep = "";
            while(getline(rep_file, rep)){
                auto fpos = rep.find(' ');
                std::string username   = rep.substr(0, fpos);
                std::string reputation = rep.substr(fpos+1, rep.size());
                if(!this->reputations.insert({username, reputation}).second)
                    std::cout << "Reputation not inserted for " << username << "!" << endl;
            }
            rep_file.close();
        };

        auto read_users = [&](const char * us){ return read_single_user(us); };

        auto timer1 = timer::start();
        read_reps();
        auto stop1 = timer::milli_step(timer1);
        std::cout << "Finished to read reputations in " << stop1 << " millisec." << endl;

        auto timer2 = timer::start();

        ifstream bot_file(this->bot_path);
        std::string bot_line = "";
        auto t = timer::start();
        while(getline(bot_file, bot_line))
            this->bots.push_back(bot_line);
        bot_file.close();
        auto st = timer::milli_step(t);
        std::cout << "Finished to read bots in " << st << " millisec." << endl;
        ifstream users_file(this->users_path);
        std::string users_line = "";
        while(getline(users_file, users_line)){
            auto fut  = std::async(read_users, users_line.c_str());
            auto _get = fut.get();
            if(_get != (User*)nullptr){
                std::string name = _get->get_name();
                sbd_users.push_back({name, stoull((split(_get->get_sbd_balance(), " ")).at(0)) });
                vest_users.push_back({name, stoull((split(_get->get_vesting_shares(), " ")).at(0)) });
                balance_users.push_back({name, stoull((split(_get->get_balance(), " ")).at(0)) });
                info_users.push_back({name, _get->is_bot(), _get->rep_class_name()});

                steemit_users.insert({name, *_get});
            }
        }
        users_file.close();

        auto stop_2 = timer::milli_step(timer2);
        std::cout << "Finished to read bots and users in " << stop_2 << " millisec." << endl;

        auto start3 = timer::start();
        std::sort(sbd_users.begin(), sbd_users.end(), greater<ordered_by_money>());
        std::sort(vest_users.begin(), vest_users.end(), greater<ordered_by_money>());
        std::sort(balance_users.begin(), balance_users.end(), greater<ordered_by_money>());
        sbd_users.resize(N_MAX_USERS);
        vest_users.resize(N_MAX_USERS);
        balance_users.resize(N_MAX_USERS);
        this->add_to_users_file(SBD_USERS, sbd_users);
        this->add_to_users_file(VEST_USERS, vest_users);
        this->add_to_users_file(BALANCE_USERS, balance_users);
        auto stop3 = timer::milli_step(start3);
        std::cout << "Finished to sort and save users on sbd, vesting_shares and balance in " << stop3 << " millisec." << endl;

    }

    void parsing_blocks(std::string blocks_path){

        this->votes.clear();
        this->follows.clear();
        this->reblogs.clear();
        this->comments.clear();
        this->votes.shrink_to_fit();
        this->follows.shrink_to_fit();
        this->reblogs.shrink_to_fit();
        this->comments.shrink_to_fit();
        auto parser = [&](std::string line_ps){ return evaluate_block(line_ps.c_str()); };
        ifstream blocks_file(blocks_path);
        auto start1 = timer::start();
        std::string users_line = "";
        while(getline(blocks_file, users_line)){
            auto fut  = std::async(parser, users_line);
            //auto _get = fut.get();
            //blocks.push_back(_get);
        }
        auto stop1 = timer::milli_step(start1);
        std::cout << blocks_path << ": finished to read blocks in " << stop1 << " millisec." << endl;

        auto start2 = timer::start();
        this->add_to_votes_file();
        this->add_to_follows_file();
        this->add_to_reblogs_file();
        this->add_to_comments_file();
        this->add_to_common_infos_file();
        auto stop2 = timer::milli_step(start2);
        std::cout << "Finished to save votes, follows and reblogs in " << stop2 << " millisec." << endl;

    }

/*    std::string whole_comment_hub(CommentHub* current){
        if(current->get_father_comment_hub() != nullptr){
            return current->get_comment() + " " + whole_comment_hub(current->get_father_comment_hub());
        } else
            return current->get_comment();
    }
*/
/*    std::string whole_comment_tree(CommentHub* current){
        std::string toret = "";
        for(auto cmt : current->get_comment_hub_tree())
            toret = toret + cmt.get_comment() + " ";
        return toret;
    }
*/

    double get_post_bot_frequency()  { return check_size(this->overall_no_post_bots, this->overall_no_post_users); }
    double get_post_user_frequency() { return check_size(this->overall_no_post_users, this->overall_no_post_bots); }

    double get_vote_bot_frequency()    { return check_size(this->overall_no_vote_bots, this->overall_no_vote_users); }
    double get_vote_user_frequency()   { return check_size(this->overall_no_vote_users, this->overall_no_vote_bots); }

    double get_weight_bot_frequency()  { return check_size(this->overall_weight_vote_bots, this->overall_weight_vote_users); }
    double get_weight_user_frequency() { return check_size(this->overall_weight_vote_users, this->overall_weight_vote_bots); }

    double get_reblog_bot_frequency()  { return check_size(this->overall_no_reblog_bots, this->overall_no_reblog_users); }
    double get_reblog_user_frequency() { return check_size(this->overall_no_reblog_users, this->overall_no_reblog_bots); }

    double get_follow_bot_frequency()  { return check_size(this->overall_no_follow_by_bots, this->overall_no_follow_by_users); }
    double get_follow_user_frequency() { return check_size(this->overall_no_follow_by_users, this->overall_no_follow_by_bots); }


    double get_bot_follow_bot_over_bot_follows_frequency()
        { return check_size(this->overall_no_bots_following_bots, this->overall_no_bots_following_users); }
    double get_bot_follow_users_over_bot_follows_frequency() 
        { return check_size(this->overall_no_bots_following_users, this->overall_no_bots_following_bots); }
    
    double get_bot_follow_bot_overall_frequency() {
        return check_size(
            this->overall_no_bots_following_bots,
            this->overall_no_bots_following_users + this->overall_no_users_following_bots + this->overall_no_users_following_users);
    }
    double get_bot_follow_users_overall_frequency() {
        return check_size(
            this->overall_no_bots_following_users, 
            this->overall_no_bots_following_bots + this->overall_no_users_following_bots + this->overall_no_users_following_users);
    }


    double get_users_follow_bot_over_user_follows_frequency()
        { return check_size(this->overall_no_users_following_bots, this->overall_no_users_following_users); }
    double get_users_follow_users_over_user_follows_frequency()
        { return check_size(this->overall_no_users_following_users, this->overall_no_users_following_bots); }
    
    double get_users_follow_bot_overall_frequency() {
        return check_size(
            this->overall_no_users_following_bots, 
            this->overall_no_bots_following_bots + this->overall_no_bots_following_users + this->overall_no_users_following_users);
    }
    double get_users_follow_users_overall_frequency() {
        return check_size(
            this->overall_no_users_following_users,
            this->overall_no_bots_following_bots + this->overall_no_bots_following_users + this->overall_no_users_following_bots);
    }

    vector<std::string> how_publish(vector<ordered_by_how_publish> how_published){
        std::sort(
            std::begin(how_published), std::end(how_published),
            [] (const ordered_by_how_publish& p1, const ordered_by_how_publish& p2) {
                return p1.count < p2.count;
            }
        );
        vector<std::string> to_ret;
        for(auto hp: this->how_users_published)
            to_ret.push_back(hp.name);
        return to_ret;
    }

    vector<std::string> get_how_publish_users(){ return how_publish(this->how_users_published); }
    vector<std::string> get_how_publish_bots() { return how_publish(this->how_bots_published);  }


    list<std::string> all_comment_hubs(std::string author){
        unordered_map<std::string, CommentHub*> * tmp = steemit_users.at(author).get_whole_comments_hub();
        list<std::string> hubs;
        for(auto it = tmp->begin(); it != tmp->end(); it++){
            string temp = it->first;
            CommentHub *parent_ch = steemit_users.at(author).get_single_comment_hub(temp);
            if (parent_ch != nullptr)
                if (!parent_ch->is_already_written())
                    //hubs.push_back(whole_comment_tree(steemit_users.at(author).get_single_comment_hub(tmp)));
                    hubs.push_back(parent_ch->get_relevant_comments());
        }
        return hubs;
    }

    //vector<BlockStructure> get_blocks(){ return this->blocks; }

    unordered_map<std::string, User> get_users(){ return this->steemit_users; }

    void save_by_repclass_file(std::string author){
        User u = steemit_users.at(author);
        list<string> to_save = all_comment_hubs(author);
        if(to_save.size() > 0){
            std::ostringstream stream;
            std::copy(to_save.begin(), to_save.end(), std::ostream_iterator<std::string>(stream, " "));
            std::string sav = stream.str();
            switch (u.rep_class()) {
                case Whale:     saving(CMT_HUBS_DIR, CMT_HUBS_WHALE,    sav); break;
                case Shark:     saving(CMT_HUBS_DIR, CMT_HUBS_SHARK,    sav); break;
                case Dolphin:   saving(CMT_HUBS_DIR, CMT_HUBS_DOLPHIN,  sav); break;
                case Minnow:    saving(CMT_HUBS_DIR, CMT_HUBS_MINNOW,   sav); break;
                case Plankton:  saving(CMT_HUBS_DIR, CMT_HUBS_PLANKTON, sav); break;
                default:                                                      break;
            }
        }
    }

    void save_by_repclass_file(){
        for(auto u : this->steemit_users){
            list<string> to_save = all_comment_hubs(u.first);
            if(to_save.size() > 0){
                std::ostringstream stream;
                std::copy(to_save.begin(), to_save.end(), std::ostream_iterator<std::string>(stream, " "));
                std::string sav = stream.str();
                switch (u.second.rep_class()) {
                    case Whale:     saving(CMT_HUBS_DIR, CMT_HUBS_WHALE,    sav); break;
                    case Shark:     saving(CMT_HUBS_DIR, CMT_HUBS_SHARK,    sav); break;
                    case Dolphin:   saving(CMT_HUBS_DIR, CMT_HUBS_DOLPHIN,  sav); break;
                    case Minnow:    saving(CMT_HUBS_DIR, CMT_HUBS_MINNOW,   sav); break;
                    case Plankton:  saving(CMT_HUBS_DIR, CMT_HUBS_PLANKTON, sav); break;
                    default:                                                      break;
                }
            }
            
        }
    }

    void save_comment_hubs_to_file(std::string author){
        string sav = comment_hub_of_author(author);
        if(sav != "")
            saving(CMT_HUBS_4_AUTHOR_DIR, author + ".cmt", sav);
    }

    void save_comment_hubs_to_file(){
        string to_save = "";
        for (auto su : steemit_users){
            std::string sav = comment_hub_of_author(su.first);
            if (sav != "") 
                to_save = to_save + sav + "\n";
        }
        saving(".", CMT_HUBS_FILE, to_save);
    }

    void save_frequencies_to_file(){
        fstream f;
        f.open(FREQUENCIES, std::fstream::in | std::fstream::out | std::fstream::app);
        if (!f)
            f.open(FREQUENCIES, std::fstream::in | std::fstream::out | std::fstream::trunc);
        
        f << "\nThe post bot frequency is: "  << this->get_post_bot_frequency()   << endl;
        f << "The vote bot frequency is: "    << this->get_vote_bot_frequency()   << endl;
        f << "The reblog bot frequency is: "  << this->get_reblog_bot_frequency() << endl;
        f << "The follows bot frequency is: " << this->get_follow_bot_frequency() << endl;
        f << "The weights given by bots is: " << this->get_weight_bot_frequency() << endl;

        f << "\nThe post users frequency is: "  << this->get_post_user_frequency()   << endl;
        f << "The vote users frequency is: "    << this->get_vote_user_frequency()   << endl;
        f << "The reblog users frequency is: "  << this->get_reblog_user_frequency() << endl;
        f << "The follows users frequency is: " << this->get_follow_user_frequency() << endl;
        f << "The weights users frequency is: " << this->get_weight_user_frequency() << endl;

        f << "\n\nFollows: " << endl;
        f << "The bots following only:" << endl;
        f << "- bots over (the bots following bots and users) is: "  << this->get_bot_follow_bot_over_bot_follows_frequency()   << endl;
        f << "- users over (the bots following bots and users) is: " << this->get_bot_follow_users_over_bot_follows_frequency() << endl;
        f << "- bots over (the bots following bots and users) and (the users following bots and users) is: "  << this->get_bot_follow_bot_overall_frequency()   << endl;
        f << "- users over (the bots following bots and users) and (the users following bots and users) is: " << this->get_bot_follow_users_overall_frequency() << endl;

        f << "\nThe users following only:" << endl;
        f << "- bots over (the users following bots and users) is: "  << this->get_users_follow_bot_over_user_follows_frequency()   << endl;
        f << "- users over (the users following bots and users) is: " << this->get_users_follow_users_over_user_follows_frequency() << endl;
        f << "- bots over (the bots following bots and users) and (the users following bots and users) is: "  << this->get_users_follow_bot_overall_frequency()   << endl;
        f << "- users over (the bots following bots and users) and (the users following bots and users) is: " << this->get_users_follow_users_overall_frequency() << endl;

        f.close();
    }

};
