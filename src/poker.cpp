#include "poker.hpp"

Player::Player() : bank_player_(0) {}
Player::~Player() { bank_player_ = 0; }

void Player::GetCards(Table& table)
{
    srand(time(0));
    int index = rand() % table.numberOfCards_--;
    hand_.first = table.deck_[index];
    table.deck_.erase(table.deck_.begin() + index);
    index = rand() % table.numberOfCards_--;
    hand_.second = table.deck_[index];
    table.deck_.erase(table.deck_.begin() + index);
    bank_player_ = 4000;
} 

size_t Table::maxElemIndex(std::vector<std::pair<size_t, std::string>> cards)
{
    size_t max = cards[0].first;
    size_t i_max = 0;
    for (size_t i = 1; i < cards.size(); ++i)
        if (cards[i].first > max)
        {
            std::swap(cards[i].first, max);
            i_max = i;
        }
    return i_max;
}   

size_t Table::countElem(std::vector<std::pair<size_t, std::string>> cards, size_t elem, std::vector<int>& retryElems)
{
    size_t count = 0;
    for (size_t i = 0; i < cards.size(); ++i)
        if (cards[i].first == elem)
        {
            count++;
            retryElems.push_back(i);
        }
    return count;
} 

int Table::AtoiCards(std::string card)
{
    if (card[0] == 'J')
        return 11;
    if (card[0] == 'Q')
        return 12;
    if (card[0] == 'K')
        return 13;
    if (card[0] == 'A')
        return 14;   
    std::string temp = "";
    for (size_t i = 0; i < card.size() - 1; ++i)
        temp += card[i];
    return atoi(temp.c_str());
}

void Table::SortCards(std::vector<std::string>& cards)
{
    for (size_t i = 1; i < cards.size(); ++i)
        for (size_t j = 0; j < cards.size() - 1; ++j)
        {
            size_t num1 = AtoiCards(cards[j]);
            size_t num2 = AtoiCards(cards[j + 1]);
            if (num1 > num2)
                std::swap(cards[j], cards[j + 1]);
        }
}

bool Table::pairKomb(std::vector<std::string> komba)
{
    for (size_t i = 0; i < komba.size() - 1; ++i)
    {
        std::string card1;
        std::string card2;
        card1.clear();
        card2.clear();
        if (komba[i][0] == '1')
            card1 = "10";
        else
            card1 = komba[i][0];
        if (komba[i + 1][0] == '1')
            card2 = "10";
        else
            card2 = komba[i + 1][0];
        if (card1 == card2)
            return true;
    }
    return false;
}

bool Table::twopairKomb(std::vector<std::string> komba)
{
    size_t count = 0;
    for (size_t i = 0; i < komba.size() - 1; ++i)
    {
        std::string card1;
        std::string card2;
        card1.clear();
        card2.clear();
        if (komba[i][0] == '1')
            card1 = "10";
        else
            card1 = komba[i][0];
        if (komba[i + 1][0] == '1')
            card2 = "10";
        else
            card2 = komba[i + 1][0];
        if (card1 == card2)
            count++;
        if (count == 2)
            return true;
    }
    return false;
}

bool Table::setKomb(std::vector<std::string> komba)
{
    for (size_t i = 0; i < komba.size() - 2; ++i)
    {
        std::string card1;
        std::string card2;
        std::string card3;
        card1.clear();
        card2.clear();
        card3.clear();
        if (komba[i][0] == '1')
            card1 = "10";
        else
            card1 = komba[i][0];
        if (komba[i + 1][0] == '1')
            card2 = "10";
        else
            card2 = komba[i + 1][0];
        if (komba[i + 2][0] == '1')
            card3 = "10";
        else
            card3 = komba[i + 2][0];
        if ((card1 == card2) && (card2 == card3))
            return true;
    }
    return false;
}

bool Table::streetKomb(std::vector<std::string> komba)
{
    size_t streetcounter = 0;
    int num2card = AtoiCards(komba[0]);
    int numAcard = AtoiCards(komba[komba.size() - 1]);
    for (size_t i = 0; i < komba.size() - 1; ++i)
    {
        int num1 = AtoiCards(komba[i]);
        int num2 = AtoiCards(komba[i + 1]);
        if ((num2 - num1 == 1) || (numAcard - num2card == 12))
            streetcounter++;
        else
            streetcounter = 0;
    }
    if (streetcounter == 4)
        return true;
    return false;
}

bool Table::flashKomb(std::vector<std::string> komba)
{
    size_t worms = 0;
    size_t cross = 0;
    size_t peaks = 0;
    size_t booby = 0;
    for (size_t i = 0; i < komba.size(); ++i)
    {
        if (komba[i][komba[i].size() - 1] == 'p')
            peaks++;
        else if (komba[i][komba[i].size() - 1] == 'c')
            cross++;
        else if (komba[i][komba[i].size() - 1] == 'b')
            booby++;
        else
            worms++;
    }
    if (worms == 5 || cross == 5 || peaks == 5 || booby == 5)
        return true;
    return false;
}

bool Table::fullhouseKomb(std::vector<std::string> komba)
{
    std::string cardbuf = "no";
    std::set<int> kolvo;
    bool set = false;
    bool pair = false;
    for (size_t i = 0; i < komba.size() - 1; ++i)
    {
        size_t count = 0;
        std::string card1;
        std::string card2;
        card1.clear();
        card2.clear();
        if (komba[i][0] == '1')
            card1 = "10";
        else
            card1 = komba[i][0];
        if (komba[i + 1][0] == '1')
            card2 = "10";
        else
            card2 = komba[i + 1][0];
        while (card1 == card2)
        {
            if (i + 1 == komba.size())
                break;
            card1.clear();
            card2.clear();
            for (size_t r = 0; r < komba[i].size() - 1; ++r)
                card1 += komba[i][r];
            for (size_t r = 0; r < komba[i + 1].size() - 1; ++r)
                card2 += komba[i + 1][r];
            ++count;
            ++i;
        }
        kolvo.insert(count);
    }
    for (auto it : kolvo)
    {
        if (it == 2)
            pair = true;
        if (it == 3)
            set = true;
    }
    if (set && pair)
        return true;
    return false;
}

bool Table::kareKomb(std::vector<std::string> komba)
{
    for (size_t i = 0; i < komba.size() - 3; ++i)
    {
        std::string card1;
        std::string card2;
        std::string card3;
        std::string card4;
        card1.clear();
        card2.clear();
        card3.clear();
        card4.clear();
        for (size_t r = 0; r < komba[i].size() - 1; ++r)
            card1 += komba[i][r];
        for (size_t r = 0; r < komba[i + 1].size() - 1; ++r)
            card2 += komba[i + 1][r];
        for (size_t r = 0; r < komba[i + 2].size() - 1; ++r)
            card3 += komba[i + 2][r];
        for (size_t r = 0; r < komba[i + 3].size() - 1; ++r)
            card4 += komba[i + 3][r];
        if ((card1 == card2) && (card2 == card3) && (card3 == card4))
            return true;
    }
    return false;
}

bool Table::streetflashKomb(std::vector<std::string> komba)
{
    size_t streetcounter = 0;
    for (size_t i = 0; i < komba.size() - 1; ++i)
    {
        int num1 = AtoiCards(komba[i]);
        int num2 = AtoiCards(komba[i + 1]);
        if (((num2 - num1 == 1) || 
            (num2 - num1 == 12)) && 
            (komba[i][komba[i].size() - 1] == komba[i + 1][komba[i + 1].size() - 1]))
            streetcounter++;
        else
            streetcounter = 0;
    }
    if (streetcounter == 4)
        return true;
    return false;
}
