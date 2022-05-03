#include "server.hpp"
#include "poker.hpp"
#include "exceptions.hpp"

#define MAX_EVENTS 1024

std::string NewGame = "\nStart of a new game\n";
std::string Connect = "\nConnect new client\n";
std::string Disconnect = "\nDisconnect client\n";
std::string ChoosingAnAction = "1. Call blind\n2. Raise\n3. Fold\n";
std::string ChoosingAnActionCheck = "1. Check\n2. Raise\n3. Fold\n";
std::string EnterTheNumberOfChips = "Enter the number of chips: \n";
std::string EnterTheNumberOfRaise = "Enter the number of raise chips: \n";
std::vector<std::string> deck = {"2w","3w","4w","5w","6w","7w","8w","9w","10w","Jw","Qw","Kw","Aw",
                                      "2p","3p","4p","5p","6p","7p","8p","9p","10p","Jp","Qp","Kp","Ap",
                                      "2c","3c","4c","5c","6c","7c","8c","9c","10c","Jc","Qc","Kc","Ac",
                                      "2b","3b","4b","5b","6b","7b","8b","9b","10b","Jb","Qb","Kb","Ab"};


// std::set<int> removeSockets;
// std::vector<std::string> boardCards;
// int blind = 20;
// Table table;
const int kReadBufferSize = 1024;

int Server::SetNonblock(int fileDescriptor)
{
    int flags;
#if defined(O_NONBLOCK)
    if (-1 == (flags = fcntl(fileDescriptor, F_GETFL, 0)))
        flags = 0;
    return fcntl(fileDescriptor, F_SETFL, O_NONBLOCK);
#else
    flags = 1;
    return ioctl(fileDescriptor, FIONBIO, &flags);
#endif
}

bool Server::SocketIsConnected(int socket)
{
    int Errors_sock = send(socket, "s ", 0, 1);
    Errors_sock = send(socket, "s ", 0, 1);
    if (Errors_sock == -1)
    {
        return false;
        close(socket);
    }
    return true;
}

Server::Server()
{
    masterSocket_ = 0;
    // sockaddr_in addr_ = NULL;
    // ePoll_ = NULL;
    // events_ = NULL;
    // setSockets_ = NULL;
}

Server::~Server()
{

}

void Server::CreateSocket()
{
    masterSocket_ = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (masterSocket_ == -1)
    {
        close(masterSocket_);
        throw SocketException();
    }
}

void Server::Bind()
{
    addr_.sin_family = AF_INET;
    addr_.sin_port = htons(3241);
    addr_.sin_addr.s_addr = htonl(INADDR_ANY);
    if(bind(masterSocket_, (struct sockaddr*)(&addr_), sizeof(addr_)) == -1)
    {
        close(masterSocket_);
        throw BindException();
    }
}

void Server::Listen()
{
    SetNonblock(masterSocket_);
    if (listen(masterSocket_, SOMAXCONN) == -1)
    {
        close(masterSocket_);
        throw ListenException();
    }
}

int maxSocketId(int MasterSocket, const std::set<int>& clientSockets_)
{
    if (clientSockets_.empty()) {
        return MasterSocket;
    } else {
        return std::max(MasterSocket, *clientSockets_.rbegin());
    }
}

int minSocketId(int MasterSocket, const std::set<int>& clientSockets_)
{
    if (clientSockets_.empty()) {
        return MasterSocket;
    } else {
        return std::min(MasterSocket, *clientSockets_.rbegin());
    }
}

void InitializeSend(std::string hand, std::string& buf)
{
    for (size_t i = 0; i < hand.size(); ++i)
    {
        if (hand[i] == 'w')
            buf += "♥";
        else if (hand[i] == 'c')
            buf += "♣";
        else if (hand[i] == 'p')
            buf += "♠";
        else if (hand[i] == 'b')
            buf += "♦";
        else
            buf += hand[i];
    }
}

void ShowCards(const std::vector<std::pair<int, Player>> players)
{
    std::string buf = "\n\n";
    size_t i = 0;
    for (auto itt : players)
    {
        buf += "Player №";
        buf += std::to_string(i);
        ++i;
        buf += "  Cards:";
        InitializeSend(itt.second.hand_.first, buf);
        buf += "  ";
        InitializeSend(itt.second.hand_.second, buf);
        buf += "\n\n";
        send(itt.first, buf.data(), buf.length(), MSG_NOSIGNAL);
        buf.clear();
    }
}

void SendNewGame(int sock)
{    
    send(sock, (void*)NewGame.c_str(), NewGame.length(), MSG_NOSIGNAL);
}

void SendTurnPlayerCheck(std::pair<int, Player> player)
{
    if (send(player.first, ChoosingAnActionCheck.c_str(), ChoosingAnActionCheck.length(), MSG_NOSIGNAL) == -1)
    {
        close(player.first);
        throw SocketException();
    }
}

void SendTurnPlayer(std::pair<int, Player> player)
{
    if (send(player.first, ChoosingAnAction.c_str(), ChoosingAnAction.length(), MSG_NOSIGNAL) == -1)
    {
        close(player.first);
        throw SocketException();
    }
}

void SendBetPlayer(std::pair<int, Player> player)
{
    if (send(player.first, EnterTheNumberOfChips.c_str(), EnterTheNumberOfChips.length(), MSG_NOSIGNAL) == -1)
    {
        close(player.first);
        throw SocketException();
    }
}

void SendGameBoard(const std::vector<std::pair<int, Player>> players, Table table)
{
    std::string tab = "****************************";
    for (auto ittt : table.board_)
    { 
        for (auto itttt : ittt)    
            InitializeSend(itttt, tab);
        tab += "\n";
    }
    tab += "****************************\n";
    for (size_t r = 0; r < players.size(); ++r)
        send(players[r].first, (void*)tab.c_str(), tab.length(), MSG_NOSIGNAL);
}

void SendRaisePlayer(std::pair<int, Player> player)
{
    if (send(player.first, EnterTheNumberOfRaise.c_str(), EnterTheNumberOfRaise.length(), MSG_NOSIGNAL) == -1)
    {
        close(player.first);
        throw SocketException();
    }
}

void InitializeTable(const std::vector<std::pair<int, Player>> players, Table& table)
{
    table.board_.resize(4);
    for (size_t i = 0; i < 4; ++i)
        table.board_[i].resize(players.size());
    for (size_t i = 0; i < players.size(); i++)
    {
        table.board_[3][i] = "\\o/    "; 
        if (i == 0)
        {
            table.board_[2][0] = "";
            continue;
        }   
        table.board_[2][i] = "      ";
    }
}

void PutOneCard(Table& table, std::vector<std::string>& boardCards)
{
    srand(time(0));
    size_t index = rand() % table.numberOfCards_;
    table.numberOfCards_ -= 1;
    table.deck_.erase(table.deck_.begin() + index);
    std::string buf = table.deck_[index];
    boardCards.push_back(buf);
    buf += "  ";
    table.board_[1].push_back(buf); 
}

void LboardIsEnd(std::vector<std::pair<int, Player>> players,  
                 size_t& lborder)
{
    if (lborder >= players.size() - 1)
        lborder = 0;
    else
        lborder++;
}

bool findElem(int value, std::set<int> removeSockets)
{
    for (auto it : removeSockets)
        if (value == it)
            return true;
    return false;
}

bool EqualizeTheBet(const std::vector<std::pair<int, Player>> players, Table table)
{
    size_t count = 0;
    for (size_t i = 0; i < players.size() - 1; ++i)
    {
        if (atoi(table.board_[2][i].c_str()) == atoi(table.board_[2][i + 1].c_str()) || 
            atoi(table.board_[2][i].c_str()) == 0 || 
            atoi(table.board_[2][i + 1].c_str()) == 0)
            count++;
    }
    if (count == players.size() - 1)
        return true;
    return false;
}

void callBlind(std::vector<std::pair<int, Player>>& players,
               size_t& lborder,
               int& blind,
               Table& table)
{
    players[lborder].second.bank_player_ -= blind;
    table.bank_ += blind;
    if (atoi(table.board_[2][lborder].c_str()) != 0)
    {
        players[lborder].second.bank_player_ += atoi(table.board_[2][lborder].c_str());
        table.bank_ -= atoi(table.board_[2][lborder].c_str());
    }
    if (lborder == 0)
        table.board_[2][lborder] = "";
    else
        table.board_[2][lborder] = "     ";
    table.board_[2][lborder] += std::to_string(blind);
    LboardIsEnd(players, lborder);
}

void raiseBet(std::vector<std::pair<int, Player>>& players,
              size_t& lborder,
              size_t& check,
              int& blind,
              Table& table)
{
    check = 0;        
    SendRaisePlayer(players[lborder]);
    std::string bufsend;
    while(recv(players[lborder].first, (void*)bufsend.c_str(), kReadBufferSize, MSG_NOSIGNAL) <= 0)
        sleep(0.1);
    players[lborder].second.bank_player_ -= atoi(bufsend.c_str());
    int prevbet = atoi(table.board_[2][lborder].c_str());
    table.bank_ += atoi(bufsend.c_str());
    prevbet += atoi(bufsend.c_str());
    if (lborder == 0)
        table.board_[2][lborder] = "";
    else
        table.board_[2][lborder] = "     ";
    table.board_[2][lborder] += std::to_string(prevbet);
    blind = prevbet;
    LboardIsEnd(players, lborder);
}

void Fold(std::vector<std::pair<int, Player>>& players,
          size_t& lborder,
          size_t& check,
          std::set<int>& removeSockets, 
          Table& table)
{
    if (check > 1)
        check--;
    table.board_[2][lborder].clear();
    table.board_[3][lborder].clear();
    table.board_[3][lborder] = "       ";
    std::string temp = "   ";
    for (size_t i = 0; i < table.board_[2][lborder].length(); ++i)
        temp += " ";
    table.board_[2][lborder] = temp;
    removeSockets.insert(lborder);
    LboardIsEnd(players, lborder);
}

void PreFlopRiverTurn(std::vector<std::pair<int, Player>>& players,
                      size_t lborder,
                      size_t check,
                      int& blind,
                      std::set<int>& removeSockets, 
                      Table& table)
{
    if (lborder == players.size())
        lborder = 0;
    while (1)
    {
        if (EqualizeTheBet(players, table) && (check == 0))
            break; 
        if (findElem(lborder, removeSockets))
        {
            LboardIsEnd(players, lborder);
            continue; 
        }
        if (atoi(table.board_[2][lborder].c_str()) != blind)
            SendTurnPlayer(players[lborder]);
        else
            SendTurnPlayerCheck(players[lborder]);
        std::string bufturn;  
        while(recv(players[lborder].first, (void*)bufturn.c_str(), kReadBufferSize, MSG_NOSIGNAL) <= 0)
            sleep(0.1);
        sleep(0.1);    
        if (bufturn[0] == '1' && (atoi(table.board_[2][lborder].c_str()) == blind))
        {
            if (check != 0)
                --check;
            LboardIsEnd(players, lborder);
        }
        else if (bufturn[0] == '1')
            callBlind(players, lborder, blind, table);
        else if (bufturn[0] == '2')
            raiseBet(players, lborder, check, blind, table);
        else if (bufturn[0] == '3')
            Fold(players, lborder, check, removeSockets, table);   
        else { /*exceptions...*/ }
        SendGameBoard(players, table);
    }
}   

void DeterminingWinner(std::vector<std::pair<int, Player>>& players,
                       std::vector<std::string> boardCards,
                       std::set<int> removeSockets,
                       Table table)
{
    size_t pair = 1;
    size_t twopair = 2;
    size_t set = 3;
    size_t street = 4;
    size_t flash = 5;
    size_t fullhouse = 6;
    size_t kare = 7;
    size_t streetflash = 8;
    std::vector<std::pair<size_t, std::string>> priorityKomba;
    priorityKomba.resize(players.size() - removeSockets.size());
    for (size_t i = 0; i < priorityKomba.size(); ++i)
    {
        priorityKomba[i].first = 0;
        priorityKomba[i].second = "high card";
    }
    std::vector<std::pair<int, Player>> tempPlayers;
    for (size_t i = 0; i < players.size(); ++i)
        if (!findElem(i, removeSockets))
            tempPlayers.push_back(players[i]);
    std::vector<std::vector<std::string>> combinations;
    std::vector<int> combinationsPlayers;
    combinations.resize(tempPlayers.size());
    for (size_t i = 0; i < combinations.size(); ++i)
    {
        combinationsPlayers.push_back(tempPlayers[i].first);
        combinations[i].push_back(tempPlayers[i].second.hand_.first);
        combinations[i].push_back(tempPlayers[i].second.hand_.second);
        for (size_t j = 0; j < boardCards.size(); ++j)
            combinations[i].push_back(boardCards[j]);
        table.SortCards(combinations[i]);
    }
    for (size_t i = 0; i < priorityKomba.size(); ++i)
    {
        if (table.pairKomb(combinations[i]))
        {
            priorityKomba[i].first = pair;
            priorityKomba[i].second = "pair";
        }
        if (table.twopairKomb(combinations[i]))
        {
            priorityKomba[i].first = twopair;
            priorityKomba[i].second = "twopair";
        }
        if (table.setKomb(combinations[i]))
        {
            priorityKomba[i].first = set;
            priorityKomba[i].second = "set";
        }
        if (table.streetKomb(combinations[i]))
        {
            priorityKomba[i].first = street;
            priorityKomba[i].second = "street";
        }
        if (table.flashKomb(combinations[i]))
        {
            priorityKomba[i].first = flash;
            priorityKomba[i].second = "flash";
        }
        if (table.fullhouseKomb(combinations[i]))
        {
            priorityKomba[i].first = fullhouse;
            priorityKomba[i].second = "fullhouse";
        }
        if (table.kareKomb(combinations[i]))
        {
            priorityKomba[i].first = kare;
            priorityKomba[i].second = "kare";
        }
        if (table.streetflashKomb(combinations[i]))
        {
            priorityKomba[i].first = streetflash;
            priorityKomba[i].second = "streetflash";
        }
    }
    ShowCards(players);
    std::string buf = "";
    for (size_t i = 0; i < tempPlayers.size(); ++i)
    {
        std::cout << priorityKomba[i].first << " ";
        buf.clear(); 
        buf += "combinations: ";
        buf += priorityKomba[i].second;
        buf += "\n";
        send(players[i].first, buf.c_str(), buf.length(), MSG_NOSIGNAL);
    }
    std::vector<int> retryElems;
    size_t maxIndex = table.maxElemIndex(priorityKomba);
    size_t countElems = table.countElem(priorityKomba, priorityKomba[maxIndex].first, retryElems);
    if (countElems > 1)
    {
        int maxPlayerInd = retryElems[0];
        std::vector<size_t> winners;
        for (size_t i = 1; i < retryElems.size(); ++i)
        {
            size_t playerCard1 = table.AtoiCards(players[i].second.hand_.first);
            size_t playerCard2 = table.AtoiCards(players[i].second.hand_.first);
            size_t playerCard = std::max(playerCard1, playerCard2);
            size_t maxPlayerCard1 = table.AtoiCards(players[maxPlayerInd].second.hand_.first);
            size_t maxPlayerCard2 = table.AtoiCards(players[maxPlayerInd].second.hand_.first);
            size_t maxPlayerCard = std::max(maxPlayerCard1, maxPlayerCard2);
            if (playerCard > maxPlayerCard) 
                std::swap(players[i].first, maxPlayerInd);
            if (playerCard == maxPlayerCard)
                winners.push_back(i);
        }
        winners.push_back(maxPlayerInd);
        if (winners.size() == 1)
        {
            buf = "winner player №";
            buf += std::to_string(winners[0]);
            buf += "\n";
            for (size_t i = 0; i < players.size(); ++i)   
                send(players[i].first, buf.c_str(), buf.length(), MSG_NOSIGNAL);
        }
        else
        {
            std::string buff = "winners: ";
            for (size_t i = 0; i < winners.size(); ++i)
            {
                buff += "№";
                buff += std::to_string(winners[i]);
                buff += " ";
            }
            buff += "\n";
            for (size_t i = 0; i < players.size(); ++i)   
                send(players[i].first, buff.c_str(), buff.length(), MSG_NOSIGNAL);
        }
    }
    else
    {
        int num = 0;
        for (size_t i = 0; i < players.size(); ++i)
            if (players[i].first == combinationsPlayers[maxIndex])
                num = i;
        players[0].second.bank_player_ += table.bank_;
        table.bank_ = 0;
        buf = "winner player №";
        buf += std::to_string(num);
        buf += "\n";
        for (size_t i = 0; i < players.size(); ++i)   
            send(players[i].first, buf.c_str(), buf.length(), MSG_NOSIGNAL);
    }
}

void CleaningTable(std::set<int>& removeSockets,
                   std::vector<std::string>& boardCards,
                   Table& table,
                   std::vector<std::pair<int, Player>>& players,
                   int& blind)
{
    removeSockets.clear();
    boardCards.clear();
    blind = 20;
    table.deck_ = deck;
    table.bank_ = 0;
    table.board_.clear();
    table.numberOfCards_ = 52;
    for (size_t i = 0; i < players.size(); ++i)
    {
        players[i].second.hand_.first.clear();
        players[i].second.hand_.second.clear();
        players[i].second.GetCards(table);
    }
}

void Server::StartGame()
{
    std::set<int> removeSockets;
    std::vector<std::string> boardCards;
    int blind = 20;
    Table table;
    std::vector<std::pair<int, Player>> players;
    size_t check = 0;
    size_t bigBlind = 0;
    int max_do = 0;
    bool run = true;
    fd_set fds;
    while (run) 
    {
        FD_ZERO(&fds);
        FD_SET(masterSocket_, &fds);;
        for (auto sock : clientSockets_) 
            FD_SET(sock, &fds);
        int max = maxSocketId(masterSocket_, clientSockets_);
        if (max > max_do && max != masterSocket_)
            std::cout << Connect;
        if (max < max_do)
            std::cout << Disconnect;
        max_do = max;
        select(max + 1, &fds, nullptr, nullptr, nullptr);
        auto it = clientSockets_.begin();
        while (it != clientSockets_.end()) 
        {
            int sock = *it;
            if (FD_ISSET(sock, &fds)) 
            {
                std::string buf;
                while(recv(sock, (void*)buf.c_str(), kReadBufferSize, MSG_NOSIGNAL) <= 0)
                    sleep(0.1);       
                // SendNewGame(players);         
                ShowCards(players);
                InitializeTable(players, table);
                srand(time(0));
                bigBlind = rand() % players.size();
                if (bigBlind == 0)
                {
                    table.board_[2][bigBlind] += std::to_string(blind);
                    table.board_[2][players.size() - 1] += std::to_string(blind / 2);
                    table.bank_ += (3 * blind / 2);
                }
                else
                {
                    table.board_[2][bigBlind] += std::to_string(blind);
                    table.board_[2][bigBlind - 1] += std::to_string(blind - blind / 2);
                    table.bank_ += (3 * blind / 2);
                }
                //Initizialize
                check = 1;
                SendGameBoard(players, table);
                //Preflop...
                PreFlopRiverTurn(players, bigBlind + 1, check, blind, removeSockets, table);
                PutOneCard(table, boardCards);
                PutOneCard(table, boardCards);
                PutOneCard(table, boardCards);
                SendGameBoard(players, table);
                //Flop...
                check = players.size() - removeSockets.size();
                PreFlopRiverTurn(players, bigBlind + 1, check, blind, removeSockets, table);
                PutOneCard(table, boardCards);
                SendGameBoard(players, table);
                //River...
                check = players.size() - removeSockets.size();
                PreFlopRiverTurn(players, bigBlind + 1, check, blind, removeSockets, table);
                PutOneCard(table, boardCards);
                SendGameBoard(players, table);
                //Turn...
                check = players.size() - removeSockets.size();
                PreFlopRiverTurn(players, bigBlind + 1, check, blind, removeSockets, table);
                DeterminingWinner(players, boardCards, removeSockets, table);
                //New game...
                CleaningTable(removeSockets, boardCards, table, players, blind);
            }  
            ++it;
        }
        if (FD_ISSET(masterSocket_, &fds)) 
        {
            int sock = accept(masterSocket_, 0, 0);
            if (sock == -1) 
            {
                close(masterSocket_);
                // throw SocketException();
            }   
            Player temp;
            temp.GetCards(table);
            players.push_back({sock, temp});
            SetNonblock(sock);
            clientSockets_.insert(sock);
        }
    }
}

void Server::run()
{
    CreateSocket();
    Bind();
    Listen();    
    StartGame();
}