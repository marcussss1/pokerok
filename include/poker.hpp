#pragma once 

#include <utility>
#include <vector>
#include <set>
#include <string>

class Table
{
public:

// private:
    size_t maxElemIndex(std::vector<std::pair<size_t, std::string>> cards);
    size_t countElem(std::vector<std::pair<size_t, std::string>> cards, size_t elem, std::vector<int>& retryElems);
    int AtoiCards(std::string card);
    void SortCards(std::vector<std::string>& cards);
    bool pairKomb(std::vector<std::string> komba);
    bool twopairKomb(std::vector<std::string> komba);
    bool setKomb(std::vector<std::string> komba);
    bool streetKomb(std::vector<std::string> komba);
    bool flashKomb(std::vector<std::string> komba);
    bool fullhouseKomb(std::vector<std::string> komba);
    bool kareKomb(std::vector<std::string> komba);
    bool streetflashKomb(std::vector<std::string> komba);
    
    std::vector<std::string> deck_ = {"2w","3w","4w","5w","6w","7w","8w","9w","10w","Jw","Qw","Kw","Aw",
                                      "2p","3p","4p","5p","6p","7p","8p","9p","10p","Jp","Qp","Kp","Ap",
                                      "2c","3c","4c","5c","6c","7c","8c","9c","10c","Jc","Qc","Kc","Ac",
                                      "2b","3b","4b","5b","6b","7b","8b","9b","10b","Jb","Qb","Kb","Ab"};
    std::vector<std::vector<std::string>> board_ = {};
    size_t numberOfCards_ = 52;
    unsigned int bank_ = 0;

};

class Player
{
public:
    Player();
    void GetCards(Table& table);
    // bool pairKomb(std::vector<std::string> komba);
    // void ShowCards(std::vector<std::pair<int, Player>> players);
    ~Player();
// private:
    std::pair<std::string, std::string> hand_ = {};
    unsigned int bank_player_ = 0;
};
