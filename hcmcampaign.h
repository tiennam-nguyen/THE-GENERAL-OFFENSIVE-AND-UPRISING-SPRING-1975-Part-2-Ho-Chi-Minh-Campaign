/*
 * Ho Chi Minh City University of Technology
 * Faculty of Computer Science and Engineering
 * Initial code for Assignment 2
 * Programming Fundamentals Spring 2025
 * Date: 02.02.2025
 */

// The library here is concretely set, students are not allowed to include any other libraries.
#ifndef _H_HCM_CAMPAIGN_H_
#define _H_HCM_CAMPAIGN_H_

#include "main.h"

////////////////////////////////////////////////////////////////////////
/// STUDENT'S ANSWER BEGINS HERE
/// Complete the following functions
/// DO NOT modify any parameters in the functions.
////////////////////////////////////////////////////////////////////////

// Forward declaration
class Unit;
class UnitList;
class Army;
class TerrainElement;

class Vehicle;
class Infantry;

class LiberationArmy;
class ARVN;

class Position;

class Road;
class Mountain;
class River;
class Urban;
class Fortification;
class SpecialZone;

class BattleField;

class HCMCampaign;
class Configuration;

enum VehicleType
{
    TRUCK,
    MORTAR,
    ANTIAIRCRAFT,
    ARMOREDCAR,
    APC,
    ARTILLERY,
    TANK
};
enum InfantryType
{
    SNIPER,
    ANTIAIRCRAFTSQUAD,
    MORTARSQUAD,
    ENGINEER,
    SPECIALFORCES,
    REGULARINFANTRY
};

//3.6 Position
class Position
{
    private:
    int r, c;//row and collumn
public:
    Position(int r = 0, int c = 0) : r(r), c(c) {}//constructor from int

    Position(const string &str_pos) {//constructor from string
        size_t start = str_pos.find('(');
        size_t comma = str_pos.find(',', start);//parsing strings
        size_t end = str_pos.find(')', comma);

        r = stoi(str_pos.substr(start + 1, comma - start - 1));//convert substring to int
        c = stoi(str_pos.substr(comma + 1, end - comma - 1));
    }
    int getRow() const { return r; }
    int getCol() const { return c; } //getter and setter methods
    void setRow(int r) { this->r = r; }
    void setCol(int c) { this->c = c; }
    string str() const {//return formatted strings
        return "(" + to_string(r) + "," + to_string(c) + ")";
    }
    double distanceTo(const Position& other) const {
        int dr = r - other.r;
        int dc = c - other.c;
        return sqrt(dr*dr+dc*dc);
    }
};


//3.1 Unit
class Unit
{
    protected:
    int quantity;
    int weight;
    Position pos;
    double attackScoreMulti = 1.0;
    double attackScoreAdd = 0.0;
public:
Unit(int quantity, int weight, const Position& pos) : quantity(quantity), weight(weight), pos(pos) {}//constructor
virtual ~Unit() {}//destructor
virtual int getAttackScore() = 0;//other subclass will implement this method
Position getCurrentPosition() const {
    return pos;
}
virtual string str() const = 0;//other subclass will implement this method
int getQuantity(){ return quantity; }
void setQuantity(int q){ quantity=q; }
int getWeight(){ return weight; }
void setWeight(int w){ weight=w; }
void applyTerrainMulti(double multi){ attackScoreMulti*=multi; }
void applyTerrainAdd(double add){ attackScoreAdd+=add; }
double getScoreAdd() const { return attackScoreAdd; }
int getEffectAttackScore() {
    int base_score = getAttackScore();
    return static_cast<int>(ceil(base_score*attackScoreMulti+attackScoreAdd));
}

void resetTerrainEffect() {
         attackScoreMulti=1.0;
         attackScoreAdd=0;
        }
};

//3.2 Vehicle
class Vehicle : public Unit{
    private:
    VehicleType vehicleType;//vehicle type

    public:
    Vehicle(int quantity, int weight, const Position& pos, VehicleType vehicleType) : Unit(quantity, weight, pos), vehicleType(vehicleType) {}//constructor
    int getAttackScore() override {
        int score = static_cast<int>(ceil((vehicleType * 304 + quantity * weight)/30.0));//attack score
        return score;
    }

    string getVehicleString(VehicleType type) const {//map enum to string
        switch (type) {
            case TRUCK: return "TRUCK";
            case MORTAR: return "MORTAR";
            case ANTIAIRCRAFT: return "ANTIAIRCRAFT";
            case ARMOREDCAR: return "ARMOREDCAR";
            case APC: return "APC";
            case ARTILLERY: return "ARTILLERY";
            case TANK: return "TANK";
            default: return "UNKNOWN";
        }
    }

    string str() const override {
        stringstream ss;
        ss << "Vehicle[vehicleType=" << getVehicleString(vehicleType)
           << ",quantity=" << quantity 
           << ",weight=" << weight 
           << ",position=" << pos.str() << "]";
        return ss.str();
    }
    VehicleType getVehicleType() const { return vehicleType; }
};

//3.3 Infantry Forces
class Infantry: public Unit {
private:
    InfantryType infantryType;

    public:
    bool isPerfectSquare(int num) {
        int sqrtNum = static_cast <int> (sqrt(num));
        return (sqrtNum * sqrtNum == num);
    }
    int personalNumber(int score, int year = 1975);//forward declaration

    Infantry(int quantity, int weight, const Position& pos, InfantryType infantryType): Unit(quantity, weight, pos), infantryType(infantryType) {}//constructor
    int getAttackScore() override {
        int score = static_cast<int>(infantryType) * 56 + quantity * weight;//calculate score to check for personal number
        if(infantryType==SPECIALFORCES && isPerfectSquare(weight)){//check for commandos
            score+=75;
        }
        int pn = personalNumber(score);
        if(pn>7){
            quantity=static_cast<int>(ceil(quantity*1.2));//round up ceil(x/y)=(x+y-1)/y
        }
        else if(pn<3){
            quantity=static_cast<int>(ceil(quantity*0.9));//round up
        }
        score = static_cast<int>(infantryType) * 56 + quantity * weight;//recalculate the score
        if(infantryType==SPECIALFORCES && isPerfectSquare(weight)){//check for commandos again after recalculation
            score+=75;
        }
        return score;
    }

    string getInfantryString(InfantryType type) const {//map enum to string
        switch(type){
            case SNIPER: return "SNIPER";
            case ANTIAIRCRAFTSQUAD: return "ANTIAIRCRAFTSQUAD";
            case MORTARSQUAD: return "MORTARSQUAD";
            case ENGINEER: return "ENGINEER";
            case SPECIALFORCES: return "SPECIALFORCES";
            case REGULARINFANTRY: return "REGULARINFANTRY";
            default: return "UNKNOWN";
    }
    }

    string str() const override {
        stringstream ss;
        ss << "Infantry[infantryType=" << getInfantryString(infantryType)
           << ",quantity=" << quantity
           << ",weight=" << weight
           << ",position=" << pos.str() << "]";
           return ss.str();
    }
    InfantryType getInfantryType() const { return infantryType; }
};

//3.5 Military Unit List
class UnitList
{
private:
    int capacity;
    // TODO
    int count_vehicle;
    int count_infantry;
public:
    struct Node{
    Unit* unit;//of abstract class Unit
    Node* next;
    Node(Unit* u, Node* n = nullptr): unit(u), next(n){}
    ~Node() {}//unit deletion handled by methods
    };
private:
    Node* head;
public:
    bool checkPrimeNum(int n);
    double log_base(double x, double base);
    bool checkSpecial(int n, int base, int maxExpo);
    bool checkSpecialNum(int n);
    int getMaxCapacity(int S);

    UnitList(int S);//constructor
    bool insert(Unit *unit);                   // return true if insert successfully
    string str() const;
    // TODO
    Node* getHead() const { return head; }//head getter
    void setHead(Node* head) { this->head=head; }
    
    ~UnitList(){ //destructor
        while(head){
            Node* temp = head;
            head = head->next;
            delete temp;
        }
    }
    bool isContain (VehicleType vehicleType){//check vehicleType is already there // return true if it exists
        Node* check = head;
        while (check){
            Vehicle* V = dynamic_cast<Vehicle*>(check->unit); //downcasting Unit to Vehicle
            if(V && V->getVehicleType()==vehicleType) return true;//if cast successfully and match
            check=check->next;//update parameter
        }
        return false;
    }
    bool isContain (InfantryType infantryType){//check if infantryType is already here // return true if it exists
        Node* check = head;
        while (check){
            Infantry* I = dynamic_cast<Infantry*>(check->unit); //downcasting Unit to Infantry
            if(I && I->getInfantryType()==infantryType) return true;//if cast successfully and match
            check=check->next;//update parameter
        }
        return false;
    }
    bool remove(Unit* unit, bool deleteUnit=true);
    vector<Unit*> getUnits() const;
};

class Army
{
protected:
    int LF, EXP;
    string name;
    UnitList *unitList;
    BattleField *battleField;

public:
    Army(Unit** unitArray, int size, string name, BattleField* battleField);//constructor
    virtual ~Army(){
        delete unitList;
    }//destructor
    int nextFibo(int n);
    virtual void fight(Army *enemy, bool defense = false) = 0;
    virtual string str() const = 0;
    int getLF(){ return LF; } 
    int getEXP(){ return EXP; }
    void updateIndices();
    UnitList* getUnitList() const { return unitList; }
};
//Liberation Army
class LiberationArmy : public Army{
    public:
    LiberationArmy(Unit** unitArray, int size, string name, BattleField* battleField): Army (unitArray, size, name, battleField){}
    void fight(Army* enemy, bool defense = false) override;
    vector<Unit*> findMinimalSubset(vector<Unit*> units, int target);
    vector<Unit*> getUnitsByType(UnitList* list, bool isVehicle);
    string str() const override;
    bool libChanged = false;
};
//ARVN
class ARVN : public Army{//put here to avoid name clash with ARVN of HCMCampaign
    public:
    ARVN(Unit** unitArray, int size, string name, BattleField* battleField): Army (unitArray, size, name, battleField){}
    void fight(Army* enemy, bool defense = false);
    string str() const override;
    bool arvnChanged = false;
};

//3.7 Terrain
class TerrainElement
{
public:
    TerrainElement(){}
    ~TerrainElement(){}
    virtual void getEffect(Army *army) = 0;
};

class Road : public TerrainElement{
    public:
    Road(){}
    ~Road(){}
    void getEffect(Army *army) override {}   //no effects
};
class Mountain : public TerrainElement{
    private:
    Position pos;
    public:
    Mountain(const Position& pos): pos(pos){}
    ~Mountain(){}
    void getEffect(Army *army) override;
};
class River: public TerrainElement{
    private:
    Position pos;
    public:
    River(const Position& pos): pos(pos){}
    ~River(){}
    void getEffect(Army *army) override;
};
class Urban: public TerrainElement{
    private:
    Position pos;
    public:
    Urban(const Position& pos): pos(pos){}
    ~Urban(){}
    void getEffect(Army *army) override;
};
class Fortification: public TerrainElement{
    private:
    Position pos;
    public:
    Fortification(const Position& pos): pos(pos){}
    ~Fortification(){}
    void getEffect(Army *army) override;
};
class SpecialZone: public TerrainElement{
    private:
    Position pos;
    public:
    SpecialZone(const Position& pos): pos(pos){}
    ~SpecialZone(){}
    void getEffect(Army *army) override;
};

//3.8 BattleField
class BattleField
{
private:
    int n_rows, n_cols;
    // TODO
    vector<vector<TerrainElement*>> terrain;
public:
    BattleField(int n_rows, int n_cols, vector<Position *> arrayForest,
                vector<Position *> arrayRiver, vector<Position *> arrayFortification,
                vector<Position *> arrayUrban, vector<Position *> arraySpecialZone);
    ~BattleField();
    string str() const {
        stringstream ss;
        ss << "BattleField[n_rows=" << n_rows << ",n_col=" << n_cols << "]";
        return ss.str();
    }
    int getRows() const { return n_rows; }
    int getCols() const { return n_cols; }
    TerrainElement* getTerrainElement(int i, int j) const { return terrain[i][j]; }
};

class Configuration{
    private:
    int num_rows, num_cols;
    vector<Position *> arrayForest, arrayRiver, arrayFortification, arrayUrban, arraySpecialZone;
    vector<Unit*> liberationUnits;
    vector<Unit*> ARVNUnits;
    int eventCode;
    public:
    Configuration(const string & filepath);
    ~Configuration();
    string str() const;
    vector<Position*> parsePositions(const string& data);
    void parseUnits(const string& data);
    //getters
    int getNumRows() const { return num_rows; }
    int getNumCols() const { return num_cols; }
    const vector<Position*>& getArrayForest() const { return arrayForest; }
    const vector<Position*>& getArrayRiver() const { return arrayRiver; }
    const vector<Position*>& getArrayFortification() const { return arrayFortification; }
    const vector<Position*>& getArrayUrban() const { return arrayUrban; }
    const vector<Position*>& getArraySpecialZone() const { return arraySpecialZone; }
    const vector<Unit*>& getLiberationUnits() const { return liberationUnits; }
    const vector<Unit*>& getARVNUnits() const { return ARVNUnits; }
    int getEventCode() const { return eventCode; }

    void clearUnits(){
        liberationUnits.clear();
        ARVNUnits.clear();
    }
};

class HCMCampaign
{
private:
    Configuration *config;
    BattleField *battleField;
    LiberationArmy *liberationArmy;
    class ARVN *ARVN;

public:
    HCMCampaign(const string &config_file_path);
    void run();
    string printResult();
    struct UnitStats{
        Unit* unit;
        int quantity;
        int weight;
    };
};



#endif