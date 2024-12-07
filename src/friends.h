#ifndef FRIENDS_H
#define FRIENDS_H

#include <stdbool.h>

bool add_friend(int user_id, int friend_id);
bool remove_friend(int user_id, int friend_id);
bool are_friends(int user_id, int friend_id);

#endif // FRIENDS_H
