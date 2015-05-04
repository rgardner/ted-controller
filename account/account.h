/* An account in our payment system. This provides authentication.
 *
 */
#ifndef Account_H_
#define Account_H_

class Account
{
  public:
    // Create a new account.
    Account(int account_number, string password);

    // Find an account matching the corresponding account_number.
    bool find_account(int account_number);

    // Authenticate the account.
    bool sign_in(string password);
  private:
    int account_number;
    string password;
};

#endif
