#include <SFML/Graphics.hpp>
#include <string>
#include <cstdlib>
using namespace std;
using namespace sf;

Sprite make_image(string filepath,Texture& image){
    Sprite temp;
    image.loadFromFile(filepath);
    temp.setTexture(image);
    return temp;
}

class Generator{
public:
    int Row;
    int Col;
    int mines;
    vector<vector<string>>arr;
    Generator(int _row,int _col,int _mines){
        Row=_row;
        Col=_col;
        mines= _mines;
        arr.resize(Row, vector<string>(Col, ""));
        for(int i=0;i<Row;i++){
            for(int j=0;j<Col;j++){
                arr[i][j]="";
            }
        }
        for(int i=0;i<mines;i++){
            while(true){
                int row = rand() % Row;
                int col = rand() % Col;
                if(arr[row][col]=="M"){continue;}
                arr[row][col] = "M";
                break;
            }
        }
    }

};

class Tile{
public:
    string value;
    int row;
    int col;
    bool selected;
    bool flagged;
    Texture image;
    Sprite tile;
    Tile* neighbors[8];

    Tile(string value, int row, int col){
        this->value=value;
        this->row=row;
        this->col=col;
        this->selected= false;
        for(int i=0;i<8;i++){
            neighbors[i]=nullptr;
        }
    }

    void setValue(){
        int n=0;
        for(int i=0;i<8;i++) {
            if(neighbors[i]==nullptr){
                continue;
            }
            if ((*(neighbors[i])).value == "M") {
                n++;
            }
        }
        if (value!="M"){
            value = to_string(n);
        }
    }

    void sprite(){
        if(flagged==true){
            tile=make_image("../files/images/flag.png",image);
        }
        else if(this->selected== false){
            tile=make_image("../files/images/tile_hidden.png",image);
        }
        if(this->selected== true){
            if(value=="M"){
                tile=make_image("../files/images/mine.png",image);
            }
            else if(value=="0"){
                tile=make_image("../files/images/tile_revealed.png",image);
            }
            else{
                string path="../files/images/number_"+value+".png";
                tile=make_image(path,image);
            }
        }
        tile.setPosition(col*32,row*32);
    }

};

class Board{
public:
    vector<vector<Tile>>tiles;
    vector<vector<string>>game;
    int mines;
    Board(int row,int col, int mines) {
        this->mines=mines;
        Generator Two_D(row,col,mines);
        game=Two_D.arr;
        tiles.resize(row, vector<Tile>(col, Tile("", 0, 0)));
    }
    void generate_tiles(){
        for (int i = 0; i < game.size(); ++i) {
            for (int j = 0; j < game[i].size(); ++j) {
                Tile tile(game[i][j],i,j);
                tile.sprite();
                tiles[i][j] = tile;
            }
        }
    }
    void setNeighbors(){
        for (int i = 0; i < game.size(); ++i) {
            for (int j = 0; j < game[i].size(); ++j) {
                if((i+1)<game.size()){
                    tiles[i][j].neighbors[0]=&(tiles[i+1][j]);
                }
                if((i-1)>=0) {
                    tiles[i][j].neighbors[1] = &(tiles[i - 1][j]);
                }
                if((j+1)<game[i].size()) {
                    tiles[i][j].neighbors[2] = &(tiles[i][j+1]);
                }
                if((j-1)>=0) {
                    tiles[i][j].neighbors[3] = &(tiles[i][j-1]);
                }
                if((j-1)>=0 && (i-1)>=0) {
                    tiles[i][j].neighbors[4] = &(tiles[i - 1][j-1]);
                }
                if((j-1)>=0 && (i+1)<game.size()) {
                    tiles[i][j].neighbors[5] = &(tiles[i + 1][j-1]);
                }
                if((j+1)<game[i].size() && (i-1)>=0) {
                    tiles[i][j].neighbors[6] = &(tiles[i - 1][j+1]);
                }
                if((j+1)<game[i].size() && (i+1)<game.size()) {
                    tiles[i][j].neighbors[7] = &(tiles[i+1][j+1]);
                }
            }
        }
    }
    string Mines(Tile& t){
        int n=0;
        for(int i=0;i<8;i++){
            if((*(t.neighbors[i])).value=="M"){
                n++;
            }
        }
        return to_string(n);
    }
    void displayMines(){
        for (int i = 0; i < tiles.size(); ++i) {
            for (int j = 0; j<tiles[i].size(); ++j) {
                if(tiles[i][j].value=="M"){
                    tiles[i][j].selected=true;
                    tiles[i][j].sprite();
                }
            }
        }
    }
    int Win(){
        //-1: Lose, 0:Game still going, 1: Win
        int status=1;
        for (int i = 0; i < tiles.size(); ++i) {
            for (int j = 0; j<tiles[i].size(); ++j) {
                if(tiles[i][j].value=="M" && tiles[i][j].selected==true ){
                    return -1;
                }
                if(tiles[i][j].selected==false){
                    status=0;
                }
            }
        }
        return status;
    }
    void reset(){
        Generator Two_D(game.size(),game[0].size(),mines);
        game=Two_D.arr;
        for (int i = 0; i < game.size(); ++i) {
            for (int j = 0; j < game[i].size(); ++j) {
                tiles[i][j].value =game[i][j];
                tiles[i][j].selected=false;
                tiles[i][j].flagged=false;
                setNeighbors();
            }
        }
    }
};