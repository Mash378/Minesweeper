#include <iostream>
#include <chrono>
#include <fstream>
#include <SFML/Graphics.hpp>
#include <string>
#include "Game.h"
#include<map>
#include <sstream>
using namespace std;
using namespace sf;

Sprite loadimage(string filepath,Texture& image){
    Sprite temp;
    image.loadFromFile(filepath);
    temp.setTexture(image);
    return temp;
}
void setText(sf::Text &text, float x, float y){
    sf::FloatRect textRect = text.getLocalBounds();
    text.setOrigin(textRect.left + textRect.width/2.0f,
                     textRect.top + textRect.height/2.0f);
    text.setPosition(sf::Vector2f(x, y));
}

void LeaderBoard_Screen(int row, int column,string Name){
    int Width= column*16;
    int Height= row*16+50;
    RenderWindow RankWindow(VideoMode(Width,Height),"Leader Board",Style::Close);
    ifstream file("files/leaderboard.txt");
    string line,name,time;
    map<int,string>players;
    while(getline(file,line)){
        stringstream s(line);
        getline(s,time,',');
        cout<<time<<endl;
        time.erase(2,1);
        getline(s,name);
        cout<<stoi(time)<<endl;
        players[stoi(time)]=name;
    }

    sf::Font font;
    font.loadFromFile("files/font.ttf");
    Text title("LeaderBoard",font,20);
    title.setFillColor(Color::White);
    title.setStyle(sf::Text::Bold | sf::Text::Underlined);
    setText(title,Width/2,Height/2-120);
    while(RankWindow.isOpen()){
        sf::Event event;
        vector<Sprite>digits;
        while(RankWindow.pollEvent(event)) {
            if (event.type == Event::Closed) {
                RankWindow.close();
            }
        }
        RankWindow.clear(Color::Blue);
        RankWindow.draw(title);
        vector<Text>Ranks;
        int value;
        if(players.size()>5){value=5;}
        else{value=players.size();}
        auto iter=players.end();
        for(int i=1;i<=value;i++){
            iter--;
            string player_time="";
            if(iter->first/1000==0){player_time="0";}
            player_time+= to_string(iter->first);
            cout<<player_time<<endl;
            player_time.insert(2,":");
            string winner= to_string(i)+".\t"+player_time+"\t"+iter->second;
            if(iter->second==Name){
                winner+="*";
            }
            Text person(winner,font,18);
            person.setFillColor(Color::White);
            setText(person,Width/2,Height/2+20*i);
            RankWindow.draw(person);
        }
        RankWindow.display();

    }
    file.close();
}
void runGame(int row,int column, int mines,string name){
    RenderWindow GameWindow(sf::VideoMode(column*32, row*32+100), "SFML Works!");
    Texture happy,win,lose,debug,pause,play,leaderboard,minutes,seconds,counter;

    Sprite HappyFace= loadimage("../files/images/face_happy.png",happy);
    HappyFace.setPosition((column*32)/2.0-32,32*(row+0.5));
    Sprite Lose= loadimage("../files/images/face_lose.png",lose);
    Lose.setPosition((column*32)/2.0-32,32*(row+0.5));
    Sprite Win= loadimage("../files/images/face_win.png",win);
    Win.setPosition((column*32)/2.0-32,32*(row+0.5));
    Sprite Debug = loadimage("../files/images/debug.png",debug);
    Debug.setPosition(((column*32)-304),(32*(row+0.5)));

    Sprite Pause= loadimage("../files/images/pause.png",pause);
    Pause.setPosition((column*32)-240,32*(row+0.5));
    Sprite Play= loadimage("../files/images/play.png",play);
    Play.setPosition((column*32)-240,32*(row+0.5));

    Sprite Leaderboard=loadimage("../files/images/leaderboard.png",leaderboard);
    Leaderboard.setPosition((column*32)-176,32*(row+0.5));

    Board minesweeper(row,column,mines);
    minesweeper.generate_tiles();
    minesweeper.setNeighbors();
    bool is_Paused=false;
    int mines_flagged= mines;
    sf::Clock clock;

    ofstream file("../files/leaderboard.txt",ios::app);

    while(GameWindow.isOpen()){
        sf::Event event;
        vector<Sprite>digits;
        while(GameWindow.pollEvent(event)) {
            if(event.type == Event::Closed) {
                GameWindow.close();
            }
            if(event.type == Event::MouseButtonPressed){
                Vector2i click= Mouse::getPosition(GameWindow);

                if (Mouse::isButtonPressed(Mouse::Left)){
                    if(HappyFace.getGlobalBounds().contains(click.x,click.y)){
                        minesweeper.reset();
                    }
                    else if(click.y<(row*32) && is_Paused==false && minesweeper.tiles[click.y/32][click.x/32].flagged==false &&minesweeper.Win()==0){
                        minesweeper.tiles[click.y/32][click.x/32].selected=true;
                        minesweeper.tiles[click.y/32][click.x/32].setValue();
                        minesweeper.tiles[click.y/32][click.x/32].sprite();
                    }
                    else if(Leaderboard.getGlobalBounds().contains(click.x,click.y)){
                        LeaderBoard_Screen(row,column,name);
                    }
                    else if((minesweeper.Win()==0) && (is_Paused==false) && (Debug.getGlobalBounds().contains(click.x,click.y))){
                        minesweeper.displayMines();
                    }
                    else if(minesweeper.Win()==0 && is_Paused==false && Pause.getGlobalBounds().contains(click.x,click.y)){
                        is_Paused=true;
                    }
                    else if(minesweeper.Win()==0 && is_Paused==true && Play.getGlobalBounds().contains(click.x,click.y)){
                        is_Paused=false;
                    }
                }
                if (Mouse::isButtonPressed(Mouse::Right)){
                    if(click.y<(row*32) && is_Paused==false && minesweeper.Win()==0){
                        bool flag=minesweeper.tiles[click.y/32][click.x/32].flagged;
                        if(flag==true){
                            minesweeper.tiles[click.y/32][click.x/32].flagged=false;
                            mines_flagged++;
                        }
                        else {
                            minesweeper.tiles[click.y / 32][click.x / 32].flagged = true;
                            mines_flagged--;
                        }
                    }
                }
            }
        }
        GameWindow.clear(Color::White);
        for (int i = 0; i<row; ++i) {
            for (int j = 0; j <column ; ++j){
                minesweeper.tiles[i][j].sprite();
                GameWindow.draw(minesweeper.tiles[i][j].tile);
            }
        }
        GameWindow.draw(Leaderboard);
        GameWindow.draw(Debug);
        int temp= abs(mines_flagged);
        int shift=0;
        while(temp>=10){
            counter.loadFromFile("../files/images/digits.png",sf::IntRect (21*(temp/10),0,21,32));
            Sprite Counter;
            Counter.setTexture(counter);
            Counter.setPosition(33+21*shift,(32*(row+0.5)+16));
            GameWindow.draw(Counter);
            digits.push_back(Counter);
            temp= temp%10;
            shift++;
        }
        counter.loadFromFile("../files/images/digits.png",sf::IntRect(21*abs(temp),0,21,32));
        Sprite Counter;
        Counter.setTexture(counter);
        Counter.setPosition(33+21*shift,(32*(row+0.5)+16));
        //digits.push_back(Counter);
        GameWindow.draw(Counter);
        if(mines_flagged<0){
            counter.loadFromFile("../files/images/digits.png",IntRect(21*10,0,21,32));
            Counter.setPosition(12,32*(row+0.5)+16);
            GameWindow.draw(Counter);
        }
        Time time1 = clock.getElapsedTime();
        int minute= time1.asSeconds()/60;
        int second= time1.asSeconds()-minute*60;

        int minute_tens = minute / 10;
        int minute_units = minute % 10;

        minutes.loadFromFile("../files/images/digits.png", IntRect(21 *minute_tens, 0, 21, 32));
        Sprite MinutesTens;
        MinutesTens.setTexture(minutes);
        MinutesTens.setPosition(column * 32 - 97, 32 * (row + 0.5) + 16);
        GameWindow.draw(MinutesTens);

        minutes.loadFromFile("../files/images/digits.png", IntRect(21 * minute_units, 0, 21, 32));
        Sprite MinutesUnits;
        MinutesUnits.setTexture(minutes);
        MinutesUnits.setPosition(column * 32 - 76, 32 * (row + 0.5) + 16);
        GameWindow.draw(MinutesUnits);

        int second_tens = second / 10;
        int second_units = second % 10;

        seconds.loadFromFile("../files/images/digits.png", IntRect(21 * second_tens, 0, 21, 32));
        Sprite SecondsTens;
        SecondsTens.setTexture(seconds);
        SecondsTens.setPosition(column * 32 - 54, 32 * (row + 0.5) + 16);
        GameWindow.draw(SecondsTens);

        seconds.loadFromFile("../files/images/digits.png", IntRect(21 * second_units, 0, 21, 32));
        Sprite SecondsUnits;
        SecondsUnits.setTexture(seconds);
        SecondsUnits.setPosition(column * 32 - 33, 32 * (row + 0.5) + 16);
        GameWindow.draw(SecondsUnits);

        if(is_Paused==false){GameWindow.draw(Pause);}
        else{GameWindow.draw(Play);}
        if(minesweeper.Win()==1){
            GameWindow.draw(Win);
            file<<minute_tens<<minute_units<<":"<<second_tens<<second_units<<","<<name<<"\n";
        }
        else if(minesweeper.Win()==-1){
            GameWindow.draw(Lose);
        }
        else{
            GameWindow.draw(HappyFace);
        }
        GameWindow.display();
    }
    file.close();
}


int main() {
    ifstream file("../files/config.cfg");
    int row,column, mines;
    string read;

    getline(file, read);
    column= stoi(read);
    getline(file, read);
    row= stoi(read);
    getline(file, read);
    mines= stoi(read);

    file.close();

    float  Height= (row*32)+100;
    float Width=(column*32);
    sf::RenderWindow WelcomeWindow(sf::VideoMode(Width,Height), "SFML Works!");

    sf::Font font;
    font.loadFromFile("../files/font.ttf");
    sf::Text text1("WELCOME TO MINESWEEPER!",font,24);
    sf::Text text2("Enter Your Name:",font,20);
    text1.setFillColor(sf::Color::White );
    text1.setStyle(sf::Text::Bold | sf::Text::Underlined);
    setText(text1,Width/2,Height/2-150);
    text2.setFillColor(sf::Color::White);
    text2.setStyle(sf::Text::Bold);
    setText(text2,Width/2,Height/2-75);

    string user_input="";
    while(WelcomeWindow.isOpen()) {
        sf::Event event;
        while(WelcomeWindow.pollEvent(event)) {
            if(event.type == sf::Event::Closed) {
                WelcomeWindow.close();
            }
            if(event.type == sf::Event::KeyPressed){
                if((user_input.size()>0) && (event.key.code==Keyboard::Enter)){
                    WelcomeWindow.close();
                    runGame(row,column,mines,user_input);
                }
                else if((user_input.size()>0) && (event.key.code==Keyboard::BackSpace)){
                    user_input= user_input.substr(0, user_input.size() - 1);
                }
            }
            else if (event.type == sf::Event::TextEntered ){
                if ( (event.text.unicode < 0x80 && event.text.unicode>=0x61)&& user_input.size()<=10){
                    user_input+=static_cast<char>(event.text.unicode);
                }
            }
        }

        WelcomeWindow.clear(Color::Blue);;
        WelcomeWindow.draw(text1);
        WelcomeWindow.draw(text2);

        for(int i=0;i<user_input.size();i++){
            char c= toupper(static_cast<unsigned char>(user_input[i]));
            user_input[i]=c;
        }
        Text character(user_input+"|",font,18);
        character.setFillColor(Color::Yellow);
        setText(character,Width/2,Height/2-45);
        WelcomeWindow.draw(character);
        WelcomeWindow.display();
    }
    return 0;
}
