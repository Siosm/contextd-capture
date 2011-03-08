#ifndef PROMPTUSER_H
#define PROMPTUSER_H

#include <QString>

class PromptUser
{
public:
    static bool promptUserForAutorisation(const QString& scope, const QString& current_state, const QString& new_state, const QString& app_name);
};

#endif // PROMPTUSER_H
