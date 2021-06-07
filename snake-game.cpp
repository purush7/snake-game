#include <stdlib.h>
#include <stdio.h>

#include <iostream>
#include <array>
#include <vector>
#include <time.h>
#include <climits>
// bash script for win32 and linux

#ifdef _WIN32
    #include <windows.h>
    #include <conio.h>
    int unit = 500 ;     // milliseconds
    char  *cc = (char*)"cls"; // cls for windows
#else
    #include <unistd.h>
    #include <sys/select.h>
    #include <termios.h>
    #include <sys/ioctl.h>
    int unit = 1000000;    // microseconds
    char  *cc = (char*)"clear"; //  clear for linux 
        
    int bytesWaiting;
    int kbhit() {
        static const int STDIN = 0;
        static bool initialized = false;

        if (! initialized) {
            // Use termios to turn off line buffering
            termios term;
            tcgetattr(STDIN, &term);
            term.c_lflag &= ~ICANON;
            tcsetattr(STDIN, TCSANOW, &term);
            setbuf(stdin, NULL);
            initialized = true;
        }

        //int bytesWaiting;
        ioctl(STDIN, FIONREAD, &bytesWaiting);
        return bytesWaiting;
    }
#endif

void sslp(int timeperiod){
#ifdef _WIN32
    Sleep(timeperiod);
#else
    usleep(timeperiod);
#endif
}


using namespace  std;

//////////////////////////////////////////////
//    Simple demo of _kbhit()

class draw_board {                   // Board
    
    private:
    // public:
        int he;            // Board height
        int wi ;            // Board width
        char boundary_vertical = '|';
        char boundary_horizontal = '-';
        char snake_char = '#';
        char food_char = '+';
        vector<vector<char>> board;

    public:
        draw_board(int height,int width){
            this->he = height;
            this->wi = width;
            board = vector<vector<char>>(he,vector<char>(wi,'0'));
        }

        void intialize_boundary(){
            for(int h=0;h<he;h++){
                board[h][0] = boundary_vertical ;          // BOUNDARY VERTICAL CHARACTER
                board[h][wi-1] = boundary_vertical;
            }
            for(int w=1;w<wi-1;w++){
                board[0][w] = boundary_horizontal;          // BOUNDARY HORIZONTAL CHARACTER
                board[he-1][w] = boundary_horizontal;
            }
            for(int h=1;h<he-1;h++){
                for(int w=1;w<wi-1;w++){
                    board[h][w] =' ';
                }
            }
        }

        

        void print_board(){
            for(int h=0;h<he;h++){
                for(int w=0;w<wi;w++){
                    cout << board[h][w];
                    if(w!=wi-1)cout<<" ";
                }
                cout<<endl;
            }
        }
        
        // update food 
        void addFood(pair<int,int> p){
            board[p.first][p.second] = food_char;
        }

        //update snake
        void addSnake(vector<vector<int>> snake){
            for(int i =0;i<snake.size();i++){
                if(snake[i][2]==1){
                    board[snake[i][0]][snake[i][1]] = snake_char;
                }
            }
        }

};

//          Snake class consists of checking movement,food_eaten ,hiting the wall and updating score, length

class snake_class{

        int len = 1 ;           // length of snake
        //int score = 0;
        int he,wi;                  // existing heights and width of board
        vector<vector<int>> sn;     // snake
        int mov = 1;          // movement of snake 1 -> right , -1 -> left , 2-> up , -2 -> down

    public:
        bool snk_flag_hit = false;
        bool snk_flag_food = false;

        snake_class(int width,int height){
            sn = vector<vector<int>>(1,vector<int>(3));
            he = height;
            wi = width;
            srand (time(NULL));
            int x = (rand() % (height/2)) + 1;
            int y = (rand() % (width/2)) + 1;
            sn[0][0] = x;
            sn[0][1] = y;
            sn[0][2] = 1; 
        }
        // for movements just change x and y for id [i][2] is 1; and 


    // check movement
        void checkmov(char k){
            if((k == 'w' || k == 'W' || k == '8') && mov != -2 ) mov = 2;
            else if ((k == 's' || k == 'S' || k == '2') && mov != 2 ) mov = -2;
            else if ((k == 'd' || k == 'D' || k == '6') && mov != -1 ) mov = 1;
            else if ((k == 'a' || k == 'A' || k == '4') && mov != 1 ) mov = -1;

            for (int i = len-1; i >0; i--)
            {
                if(sn[i][2] == 1){
                    sn[i][0] =sn[i-1][0];
                    sn[i][1] = sn[i-1][1];
                }
                else sn[i][2]++;
            }
            
            
            if(mov == 1 ) sn[0][1]++;
            else if(mov ==-1) sn[0][1]--;
            else if(mov == 2) sn[0][0]--;
            else if(mov == -2) sn[0][0]++;
        }


    // check if snake has eaten food
        void check_food_eaten(int x_food, int y_food){  
            if( sn[0][0] == x_food && sn[0][1] == y_food ){
               // score++;
                sn.push_back({x_food,y_food,1-len});
                snk_flag_food=true;
                len++;
            }
            else snk_flag_food = false;
        }


        // check the snake whether it hits the wall
        void check_hit(){
            if(sn[0][0] == 0 || sn[0][1] == 0 || sn[0][0] == wi -1 || sn[0][1] == he -1 ) snk_flag_hit =true;
        }


        // get the snake
        vector<vector<int>> getSnake(){
            return sn;
        }
};


class food {

    int x_pos;
    int y_pos ;
    int he;
    int wi;

    public:
        food(int height,int width){
            he = height;
            wi = width;           
        }    
        void intialize_food(){              // generate food
            //srand (time(NULL));
            x_pos = (rand() % (he-2)) +1;
            y_pos = (rand() % (wi-2)) +1 ;
        }
        pair<int,int> getFood(){
            return {x_pos,y_pos};
        }
        
};



int main(int argc, char** argv) {
    
    // data
    int width,height;
    vector<vector<int>> sn ;
    int timeperiod = 2 * unit/10;       // sleep time period
    pair<int,int> p;
    int score = 0; 
    char buf[40];

    system(cc); 
    cout <<"Enter width and height of Board"<<endl;
    cin >> width>>height;
    
    //Intialization
    
    draw_board db(height,width);
    snake_class snake(width,height);
    food fd(height,width);
    
    // intializing data
    db.intialize_boundary();
    fd.intialize_food();
    p = fd.getFood();
    sn = snake.getSnake();
    char k = 'd';
    do
    {
        sslp(timeperiod);
        
        if(kbhit()){
           
        #ifdef _WIN32
            k = getch();            // for windows
        #else
            cin >> k;               // for linux
        #endif



            if(k=='c'||k=='C'){cout<<endl; exit(0);}
            else if(k=='q'||k=='Q')break;
            else if(k=='z'||k=='Z'){
                sslp(5*unit);
            }
        }
        snake.checkmov(k);
        snake.check_hit();
        snake.check_food_eaten(p.first,p.second);
        sn = snake.getSnake();
        if(snake.snk_flag_hit==true) break;
        if(snake.snk_flag_food==true){
            fd.intialize_food();
            p = fd.getFood();
            score++;
            timeperiod = timeperiod - timeperiod/10 ;      // Increasing speed
        }
        db.intialize_boundary();
        db.addFood(p); 
        db.addSnake(sn);
        system(cc);
        db.print_board();
        cout<<"score: "<<score<<"   To Quit press c     To pause for 5 secs press z     To quit with a score press q"<<endl;
    } while (true);
    
    cout<<"\n**************** Game Over with score " << score<<" ******************* "<< endl;
    return 0;
} 
