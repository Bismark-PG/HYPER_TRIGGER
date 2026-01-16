/*==============================================================================

	Manage Game Screen [Game_Screen_Manager.h]

	Author : Choi HyungJoon

==============================================================================*/
#ifndef GAME_SCREEN_MANAGER_H
#define GAME_SCREEN_MANAGER_H

enum class Main_Screen
{
    M_WAIT,
    MAIN,
    MENU_SELECT,
    SELECT_GAME,
    SELECT_SETTINGS,
    EXIT,
    M_DONE
};

enum class Sub_Screen
{
    S_WAIT,
    SETTINGS,
    S_DONE
};

enum class Game_Select_Screen
{
    G_WAIT,
    GAME_MENU_SELECT,
    GAME_PLAYING,
    GAME_IN_GAME_MENU,
    GAME_SETTING,
    G_DONE
};

class Game_Manager
{
public:
    static Game_Manager* GetInstance();

	Game_Manager(const Game_Manager&) = delete;
	Game_Manager& operator=(const Game_Manager&) = delete;

    void Change_Screen(Main_Screen M_Screen, Sub_Screen S_Screen, Game_Select_Screen G_Screen);

    void Update_Main_Screen(Main_Screen Main_Input);
    Main_Screen Get_Current_Main_Screen() const;

    void Update_Sub_Screen(Sub_Screen Sub_Input);
    Sub_Screen Get_Current_Sub_Screen() const;

    void Update_Game_Select_Screen(Game_Select_Screen Game_Select_Input);
    Game_Select_Screen Get_Current_Game_Select_Screen() const;

    void Apply_Screen_Changes();

private:
    Game_Manager() = default;
    ~Game_Manager() = default;

    Main_Screen Current_Main_Screen = Main_Screen::M_WAIT;
    Sub_Screen Current_Sub_Screen = Sub_Screen::S_WAIT;
    Game_Select_Screen Current_Game_Screen = Game_Select_Screen::G_WAIT;

    Main_Screen Next_Main_Screen = Main_Screen::M_WAIT;
    Sub_Screen Next_Sub_Screen = Sub_Screen::S_WAIT;
    Game_Select_Screen Next_Game_Screen = Game_Select_Screen::G_WAIT;
};
#endif // GAME_SCREEN_MANAGER_H