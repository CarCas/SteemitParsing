/*#include <list>
#include <algorithm>
#include "BlockStructure.hpp"

using namespace std;

class CommentHub {

private:
    string author = "";
    string comment = "";
    //string permlink = "";
    CommentHub* father;
    list<CommentHub> comment_tree;
    list<string> reblogs_user;

    string howpublished = "";

    int no_bots_reblogs = 0;
    int no_users_reblogs = 0;

    int no_bot_votes = 0;
    int no_user_votes = 0;

    int weight_bot_votes = 0;
    int weight_user_votes = 0;

public:
*/
    /*CommentHub () {
        this->father = new CommentHub("");
        this->comment_tree = new list<CommentHub>();
        this->reblogs_user = list<string>();
    }*/
/*    
    CommentHub (const string& comment, const string& author) {
        this->comment = comment;
        this->author = author;
        this->father = nullptr;
    }
    CommentHub (string& comment, string& author, CommentHub* father) {
        this->comment = comment;
        this->author = author;
        this->father = father;
    }
    //CommentHub (string comment, string permlink) : comment(move(comment)), permlink(move(permlink)){}

    void         how_published(string hp)                    { this->howpublished = move(hp); }
    
    int          get_no_votes()                              { return this->no_bot_votes + no_user_votes; }
    
    int          get_no_votes_users()                        { return this->no_user_votes; }
    int          get_no_votes_bots()                         { return this->no_bot_votes; }

    int          get_weights()                               { return this->weight_user_votes + this->weight_bot_votes; }
    
    int          get_weight_users()                          { return this->weight_user_votes; }
    int          get_weight_bots()                           { return this->weight_bot_votes; }
    

    string       get_comment()                               { return this->comment; }
    string       get_author()                                { return this->author; }
    // string    get_permlink()                              { return this->permlink; }
    void         add_in_comment_tree(CommentHub& cmt_hub)    { comment_tree.push_back(cmt_hub); }
    list         <CommentHub> get_comment_hub_tree()         { return this->comment_tree; }
    

    void         add_reblog(string u, bool is_bot)           { this->reblogs_user.push_back(u); if (is_bot) no_bots_reblogs++; else no_users_reblogs++; }
    list<string> get_reblogs()                               { return this->reblogs_user; }

    int          get_no_reblogs_users()                      { return this->no_users_reblogs; }
    int          get_no_reblogs_bots()                       { return this->no_bots_reblogs; }


    CommentHub*  get_father_comment_hub()                    { return this->father; }
    void         set_father_comment_hub(CommentHub * father) { this->father = father; }

    void add_vote(int weight, bool is_bot) {
        if (is_bot) { no_bot_votes++; weight_bot_votes += weight; }
        else {no_user_votes++; weight_user_votes += weight;}
    }

    string get_relevant_comments() {
        string toret = "";
        for (auto c : this->comment_tree)
            if(c.get_comment().find_first_not_of(" ") != std::string::npos)
                toret += c.get_comment();
        return toret;
    }
};
*/