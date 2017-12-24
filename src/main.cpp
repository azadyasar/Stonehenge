
#include <game.hh>
#include <enum_list.hh>
#include <cstdio>

int main(int argc, char *argv[])
{

    int prune;
    int diff;
    printf("Prune - Difficulty (1-3): ");
    scanf("%d - %d", &prune, &diff);
    Game game(WHITE, prune, diff);

    game.start_game();

    return 0;
}
