#ifndef CREDIT_H
#define CREDIT_H

#include <string>

class Credit
{
    Credit();
public:
    Credit( const std::string & id, const std::string & currencySym );
    void updateCredit();
    void loadPeer();

public:
    std::string m_id;
    std::string m_currency;
    std::string m_our_credit; // our credit with peer
    std::string m_credit;     // their credit with us
    std::string m_balance;    // negative means we owe them money
};

#endif // CREDIT_H
