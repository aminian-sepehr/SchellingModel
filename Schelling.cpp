#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <utility>
#include <experimental/random>
using namespace std;

const string fileAddressString = "-f";
const string stepString = "-s";
const string happinessThresholdString = "-p";
const string EMPTY = "E";
const string RED = "R";
const string BLUE = "B";
const string PPMFileName = "out.ppm";

const vector<vector<int> > Sides = { {0, -1}, 
                                     {0, 1}, 
                                     {1, 0}, 
                                     {-1, 0}
                                    };


struct Cell {

    string color;
    double happiness;
 };
typedef vector<vector< Cell*> > Table;
typedef vector< Cell*> Row;

void readFromFile(string fname, Table& table)
{
    string line;
    fstream infile;
    infile.open(fname, ios::in);
    if (infile.is_open())
    {   
        Row lineSeperated;
        while(getline(infile,line))
        {   
            lineSeperated.clear();
            for (int i = 0; i < line.size(); i++)
            {
                Cell* temp = new Cell;
                temp->color = line[i];
                lineSeperated.push_back(temp);
            }
            table.push_back(lineSeperated);
            
        }
        
    }

    
};

void createPPMFile(Table& t)
{
    ofstream output(PPMFileName, ios::binary);
    if(output.is_open())
    {
        output<< "P3"<<endl;
        output<< t.size()<<endl;
        output<< t[0].size()<<endl;
        output<< 255 <<endl;
        for (int i = 0; i < t.size(); i++)
        {
            for (int j = 0; j < t[0].size(); j++)
            {   
                if (t[i][j]->color == BLUE)
                {
                    output<< 0<<' '<<0<<' '<<255<<endl;

                }else if (t[i][j]->color == RED)
                {
                    output<< 255<<' '<<0<<' '<<0<<endl;
                }else
                {
                    output<< 255<<' '<<255<<' '<<255<<endl;
                }
                
            }
            
        }
        output.close();

    }
}

void printTableHappiness(Table& table)
{
    for (int i = 0; i < table.size(); i++)
    {
        for (int j = 0; j < table[0].size(); j++)
        {
            cout<<table[i][j]->happiness<<"  ";
        }
        cout<<endl;
    }
    
}

void printTableColor(Table& table)
{
    for (int i = 0; i < table.size(); i++)
    {
        for (int j = 0; j < table[0].size(); j++)
        {
            cout<<table[i][j]->color;
        }
        cout<<endl;
    }
    
}

double calcHappiness(Table& t, int x, int y)
{   
    double happy = 0.0;
    int devide = 0;
    for (int i = 0; i < Sides.size(); i++)
    {
        if ( ((x + Sides[i][0]) >= 0) && ((y + Sides[i][1]) >= 0) && ((x + Sides[i][0]) < t[0].size()) && ((y + Sides[i][1]) < t.size()) )
        {
            devide ++;
            if (t[ (x + Sides[i][0]) ][(y + Sides[i][1]) ]->color == t[x][y]->color || t[ (x + Sides[i][0]) ][ (y + Sides[i][1]) ]->color == "E")
            {   
                happy++;
            }
        }
    }

    return happy / devide;
}

void calcTableHappiness(Table& table)
{   
    for (int i = 0; i < table.size(); i++)
    {
        for (int j = 0; j < table[0].size(); j++)
        {
            table[i][j]->happiness = calcHappiness(table, i, j);
        }  
    }
   
}



void readinputs(int numberOfIn, char* flags[], Table& table, int& step, double& happinessThreshold)
{
    for (int i = 1; i < numberOfIn; i+=2)
    {
        if (flags[i] == fileAddressString)
        {
            readFromFile(flags[i+1], table);
            calcTableHappiness(table);

        }else if(flags[i] == stepString)
        {
            step = stoi(flags[i+1]);
        
        }else if(flags[i] == happinessThresholdString)
        {
            happinessThreshold = stof(flags[i+1]) / 100;
        }
        
    }
    
}

int calcDisatisfied(Table& t, double satisfaction)
{
    int counter = 0; 
    for (int i = 0; i < t.size(); i++)
    {
        for (int j = 0; j < t[0].size(); j++)
        {
            if (t[i][j]->happiness < satisfaction && t[i][j]->color != EMPTY)
            {
                counter++;
            }
        }
    }
    return counter;
    
}

bool areAllSatisfied(Table& t, double thresh)
{
    for (int i = 0; i < t.size(); i++)
    {
        for (int j = 0; j < t[0].size(); j++)
        {
            if (t[i][j]->happiness < thresh && t[i][j]->color != EMPTY)
            {
                return false;
            }
            
        }
    }
    return true;
    
}


void swapPeople(Table& t, int x, int y, pair<int, int> coord)
{
    t[coord.first][coord.second]->color = t[x][y]->color;
    t[x][y]->color = EMPTY;
}



pair<int, int> randomCoordinateGen(int highEnd)
{
    pair<int, int> coord;
    coord.first = std::experimental::randint(0, highEnd);
    coord.second = std::experimental::randint(0, highEnd);

    return coord;
}


bool checkAcceptability(Table& t, pair<int, int> coord )
{
    return t[coord.first][coord.second]->color == EMPTY? true:false;
}

pair<int, int> chooseRandomCoord(Table& t, int x, int y)
{
    string c = t[x][y]->color;
    pair<int, int> coord = randomCoordinateGen(t.size()-1);
    while(!checkAcceptability(t, coord))
    {
        coord = randomCoordinateGen(t.size()-1);
    }
    return coord;
}
void checkSatisfaction(Table& t, int x, int y, double satisfaction )
{
    if ((t[x][y]->color != EMPTY) && (t[x][y]->happiness < satisfaction))
    {
        pair<int, int> coord = chooseRandomCoord(t, x, y);
        swapPeople(t, x, y, coord);
    }

}

void jumpProcess(Table& t, int step, double satisfaction)
{
    int stage = 1;
    while(!areAllSatisfied(t, satisfaction))
    {   
        
        calcTableHappiness(t);
        for (int i = 0; i < t.size(); i++)
        {
            for (int j = 0; j < t[0].size(); j++)
            {
                checkSatisfaction(t, i, j, satisfaction);
            }
            
        }
        if (stage == step)
        {   
            break;
        }      
        stage++;
    }
}

int main(int argc, char* argv[])
{
    Table table;
    int step = -1;
    double happinessThreshold = 0.30;

    readinputs(argc, argv, table, step, happinessThreshold);
    
    jumpProcess(table, step, happinessThreshold);

    cout<<calcDisatisfied(table, happinessThreshold)<<endl;
    printTableColor(table);

    createPPMFile(table);
    return 0;
}



