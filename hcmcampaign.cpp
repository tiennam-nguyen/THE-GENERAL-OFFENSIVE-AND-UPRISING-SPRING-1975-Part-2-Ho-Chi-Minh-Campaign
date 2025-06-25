#include "hcmcampaign.h"

////////////////////////////////////////////////////////////////////////
/// STUDENT'S ANSWER BEGINS HERE
////////////////////////////////////////////////////////////////////////
using namespace std;



//3.3 Infantry Forces
int Infantry::personalNumber(int score, int year) {
    int sum = score + year;
    while (sum>=10)
    {
        int temp=0;
        while(sum>0){
            temp+=(sum%10);
            sum/=10;
        }
        sum=temp;
    }
    return sum;
}

    
    
    //3.4 Army
        Army::Army(Unit** unitArray, int size, string name, BattleField* battleField): unitList(nullptr), name(name), battleField(battleField), LF(0), EXP(0) {
        //calculate LF and EXP based on units from unitArray
            for (int i = 0; i < size; i++) {
            if (unitArray[i]) {
                Vehicle* v = dynamic_cast<Vehicle*>(unitArray[i]);
                if (v) {
                    LF += v->getAttackScore();
                }
                Infantry* inf = dynamic_cast<Infantry*>(unitArray[i]);
                if (inf) {
                    EXP += inf->getAttackScore();
                }
            }
        }
        LF = (LF > 1000) ? 1000 : (LF < 0) ? 0 : LF;
        EXP = (EXP > 500) ? 500 : (EXP < 0) ? 0 : EXP;
        //create unitList with capacity from S
        int S = LF + EXP;
        unitList = new UnitList(S);
        //insert to unitList (including merging)
        for (int i = 0; i < size; i++) {
            if (unitArray[i]) {
                unitList->insert(const_cast<Unit*>(unitArray[i]));
            }
        }

        //adjust quantities for merging units
        UnitList::Node* current = unitList->getHead();
        while(current){
            Infantry* inf = dynamic_cast<Infantry*>(current->unit);
            if(inf){
                inf->getAttackScore();//adjust quantity based on new quantity(because pn relies on quantity itself)
            }
            current=current->next;
        }

    }
    

    int Army::nextFibo(int n){
        if(n<=0) return 0;
        int a=0, b=1;
        while(b<n){
            int temp = a+b;
            a=b;
            b=temp;
        }
        return b;
    }

vector<Unit*> LiberationArmy::findMinimalSubset(vector<Unit*> units, int target){//method to find bestsubset from powerset
    if (target < 0) return {};
    int n = units.size();
    double min_sum = 1e9;//to initialize comparison
    vector<Unit*> bestSubset;
    for (int i=1; i < (1 << n); i++){//leave the null subset
        int sum = 0;
        vector<Unit*> current;
        for (int j=0; j<n; j++){
        if(i & (1 << j)){
            sum += units[j]->getAttackScore();
            current.push_back(units[j]);
        }
        }
        if (sum>=target && sum<min_sum){
            min_sum=sum;
            bestSubset = current;
        }
    }
    return bestSubset;
}

vector<Unit*> LiberationArmy::getUnitsByType(UnitList* list, bool isVehicle){//method to get Units by types (vehicles or infantries)
    vector<Unit*> result;
    UnitList::Node* current = list->getHead();
    while (current!=nullptr){
        Unit* unit = current->unit;
        Vehicle* v = dynamic_cast<Vehicle*>(unit);
        Infantry* inf = dynamic_cast<Infantry*>(unit);
        if(isVehicle && v)
            result.push_back(unit);
        else if(!isVehicle && inf)
            result.push_back(unit);
        current=current->next;
    }
    return result;
}

void Army::updateIndices(){//method to update LF and EXP
    LF=0;
    EXP=0;
    UnitList::Node* current = unitList->getHead();
    while (current != nullptr){
        Unit* unit = current->unit;
        Vehicle* v = dynamic_cast<Vehicle*>(unit);
        Infantry* inf = dynamic_cast<Infantry*>(unit);
        if(v)
        LF += unit->getEffectAttackScore();
        else if(inf)
        EXP += unit->getEffectAttackScore();
        current = current->next;
    }
    LF = (LF > 1000) ? 1000 : (LF < 0) ? 0 : LF;
    EXP = (EXP > 500) ? 500 : (EXP < 0) ? 0 : EXP;
}

    //Liberation Army
        void LiberationArmy::fight(Army* enemy, bool defense) {
            if(defense){//defensive case
                int iterations = 0;
                int MAX_ITERATIONS = 100; 
                checkagain:
                bool cond1 = (static_cast<long long>(ceil(this->LF*1.3))) >= (static_cast<long long>(enemy->getLF()));
                bool cond2 = (static_cast<long long>(ceil(this->EXP*1.3))) >= (static_cast<long long>(enemy->getEXP()));
    
            if (cond1 && cond2) return;//Liberate wins, no actions needed
            else if (!cond1 && !cond2){//2 indexes are smaller
                UnitList::Node* current = unitList->getHead();
                while(current){
                    Unit* unit = current->unit;
                    unit->setQuantity(nextFibo(unit->getQuantity()));
                    libChanged=true;
                    current=current->next;
                }
                this->updateIndices();
                if (++iterations < MAX_ITERATIONS) goto checkagain;
            }
            else{//1 of 2 index is smaller
                UnitList::Node* current = unitList->getHead();
                while(current){
                    Unit* unit = current->unit;
                    unit->setQuantity(static_cast<int>(ceil(unit->getQuantity()*0.9)));//reduce 10% but round up
                    libChanged=true;
                    current=current->next;
            }
            this->updateIndices();
            }
        }
        else{//offensive case: defense = false
            int tempoLF = this->LF;
            int tempoEXP = this->EXP;
            this->LF = static_cast<int>(this->LF*1.5);
            this->EXP = static_cast<int>(this->EXP*1.5);

            vector<Unit*> infantryUnits = getUnitsByType(unitList, false);
            vector<Unit*> vehicleUnits = getUnitsByType(unitList, true);
            //scale down enemy's score instead of multiplying us'
            vector<Unit*> comboA = findMinimalSubset(infantryUnits, static_cast<int>(enemy->getEXP()));
            vector<Unit*> comboB = findMinimalSubset(vehicleUnits, static_cast<int>(enemy->getLF()));

            bool foundA = !comboA.empty();
            bool foundB = !comboB.empty();

            bool battleWon = false;

            if (foundA && foundB){//victory
                for (Unit* unit : comboA){ unitList->remove(unit,false); }//remove combinations
                for (Unit* unit : comboB){ unitList->remove(unit,false); }
                battleWon=true;
            }
            else if(foundA && !foundB && (static_cast<long long>(LF) * 3 > static_cast<long long>(enemy->getLF()) * 2)){//multiply 1.5
                for (Unit* unit : comboA){ unitList->remove(unit,false); }//remove combination and vehicle
                vector<Unit*> allVehicles = getUnitsByType(unitList, true);
                for (Unit* unit : allVehicles){ unitList->remove(unit,false); }
                battleWon=true;
            }
            else if(!foundA && foundB && (static_cast<long long>(EXP) * 3 > static_cast<long long>(enemy->getEXP()) * 2)){//multiply 1.5
                for (Unit* unit : comboB){ unitList->remove(unit,false); }//remove combination and infantry
                vector<Unit*> allInfantry = getUnitsByType(unitList, false);
                for (Unit* unit : allInfantry){ unitList->remove(unit,false); }
                battleWon=true;
            }

            this->LF=tempoLF;//return the original values
            this->EXP=tempoEXP;

            if(battleWon){
            libChanged=true;
            //confiscate enemy's units from tail to head
            vector<UnitList::Node*> enemyNodes;
            UnitList::Node* current = enemy->getUnitList()->getHead();
            while(current){
                enemyNodes.push_back(current);
                current=current->next;
            }
            for(int i = enemyNodes.size()-1; i>=0; --i){
                Unit* newUnit = enemyNodes[i]->unit;
                if(newUnit && unitList->insert(newUnit)){
                    enemy->getUnitList()->remove(newUnit,false);
                    enemyNodes[i]->unit=nullptr;
                }
            }
            //rebuild enemy's unit to remove nulls
            current = enemy->getUnitList()->getHead();
            UnitList::Node* prev = nullptr;
            while(current){
                UnitList::Node* temp = current;
                current = current->next;
                if(temp->unit == nullptr){//remove these
                    if(prev){//not yet end
                        prev->next = current;//skip that node
                    }
                    else{//all nodes are null
                        enemy->getUnitList()->setHead(current);//head is null anyway
                    }
                    delete temp;//delete the middle
                }
                else{
                    prev=temp;
                }
            }
            this->updateIndices();
            enemy->updateIndices();
            }
            else{//no battle
                UnitList::Node* current = unitList->getHead();
                while(current){
                    Unit* unit = current->unit;
                    unit->setWeight(static_cast<int>(ceil(unit->getWeight()*0.9)));
                    libChanged=true;
                    current = current->next;
                }
                this->updateIndices();
            }
        }
}

string LiberationArmy::str() const {
    stringstream ss;
    ss << "LiberationArmy["
        << "LF=" << LF << ",EXP=" << EXP << ",unitList=" << unitList->str() << ",battleField=";
            if(battleField) ss << battleField->str() << "]";
            else ss << "]";
    return ss.str();
}

void ARVN::fight(Army* enemy, bool defense){
    if(defense){
        int initialLF = LF;
        int initialEXP = EXP;
        enemy->fight(this, false);
        if (unitList->getHead() && (initialLF != LF || initialEXP != EXP)){//battle takes place
            UnitList::Node* current = unitList->getHead();
            while(current){
                Unit* unit = current->unit;
                unit->setWeight(static_cast<int>(ceil(unit->getWeight()*0.8)));
                arvnChanged=true;
                current=current->next;
            }
        this->updateIndices();
    }
    }
    else{//offense (defense==false)
        vector<Unit*> toRemove;
        UnitList::Node* current = unitList->getHead();
        while(current){
            Unit* unit = current->unit;
            int newQuantity = static_cast<int>(ceil(unit->getQuantity()*0.8));
            arvnChanged=true;
            if(newQuantity<=1){
                toRemove.push_back(unit);
            }
            else{
                unit->setQuantity(newQuantity);
            }
            current=current->next;
        }
        for(Unit* unit : toRemove){
            unitList->remove(unit,false);
        }
        updateIndices();
    }
}

string ARVN::str() const {
    stringstream ss;
        ss << "ARVN["
            << "LF=" << LF << ",EXP=" << EXP << ",unitList=" << unitList->str() << ",battleField=";
            if(battleField) ss << battleField->str() << "]";
            else ss << "]";
        return ss.str(); 
}

//3.5 Military Unit List
bool UnitList::checkPrimeNum(int n){
    for (int i=2; i<=sqrt(n); i++){
        if (n%i==0) return false;
    }
    return true;
}

double UnitList::log_base(double x, double base) {
    return log(x) / log(base);
}

bool UnitList::checkSpecial(int n, int base, int maxExpo){
    if (n==0) return true;
    else if (n<0 || maxExpo<0) return false;
    return checkSpecial(n-pow(base,maxExpo),base,maxExpo-1) || checkSpecial(n,base,maxExpo-1);//either use that expo or skip
    }

bool UnitList::checkSpecialNum(int n){
    if(n==0) return false; //handle zero explicitly
    int primes[] = {3,5,7};
    for (int base : primes){
    int maxExpo = static_cast<int>(log_base(n,base));
    if(checkSpecial(n,base,maxExpo)) return true;
    }
    return false;
}
int UnitList::getMaxCapacity(int S){
    if(checkSpecialNum(S)) return 12;
    else return 8;
}
UnitList::UnitList(int S): capacity(getMaxCapacity(S)), count_infantry(0), count_vehicle(0), head(nullptr) {}

bool UnitList::insert(Unit* unit){
    Vehicle* v = dynamic_cast<Vehicle*>(unit);
    Infantry* inf = dynamic_cast<Infantry*>(unit);

    //check for repetition
    UnitList::Node* current = head;
    while(current){
        if(v && dynamic_cast<Vehicle*>(current->unit) && dynamic_cast<Vehicle*>(current->unit)->getVehicleType()==v->getVehicleType()){
            current->unit->setQuantity(current->unit->getQuantity()+unit->getQuantity());
            if(unit->getWeight()>current->unit->getWeight()) current->unit->setWeight(unit->getWeight());//take the larger weight
            return true;
        }
        else if(inf && dynamic_cast<Infantry*>(current->unit) && dynamic_cast<Infantry*>(current->unit)->getInfantryType()==inf->getInfantryType()){
            current->unit->setQuantity(current->unit->getQuantity()+unit->getQuantity());
            if(unit->getWeight()>current->unit->getWeight()) current->unit->setWeight(unit->getWeight());//take the larger weight
            return true;
        }
        current = current->next;
    }


    if(count_vehicle + count_infantry >= capacity) return false;//check capacity
    
    //Insert
    UnitList::Node* newNode = new Node(unit);//create newNode and assign the unit "needed to insert to unitList"
    if(v){//vehicle, insertAtEnd
        count_vehicle++;
        if(!head){
            head = newNode;//if linked list hasn't existed
        }
        else{
        current = head;//reset current
        while (current->next){
            current=current->next;//loop til reach the final node
        }
        current->next= newNode;//create a new node after the final leave null at the next
        }
    }
    else{//infantry, insertFront
        count_infantry++;
        newNode->next=head;
        head=newNode;
    }
    return true;
}

bool UnitList::remove(Unit* unit, bool deleteUnit){
    if(!head) return false;

    Node* current=head;
    Node* prev = nullptr;

    while(current && current->unit!=unit){
        prev = current;
        current=current->next;
    }

    if(!current) return false;//unit not found

    if(prev){
        prev->next=current->next;
    }
    else{
        head=current->next;
    }

    Vehicle* v = dynamic_cast<Vehicle*>(unit);
    if(v) count_vehicle--;
    else count_infantry--;

    if(deleteUnit) delete current->unit;
    delete current;//node
    return true;
}

string UnitList::str() const{
    stringstream ss;
    ss << "UnitList[count_vehicle=" << count_vehicle
       << ";count_infantry=" << count_infantry;
    
       if(head) ss << ";";//if unitlist was removed,  not put commas
    Node* current = head;
    while(current){
        ss << current->unit->str();
        if(current->next) ss << ',';
        current=current->next;
    }
    ss << ']';
    return ss.str();
}

vector<Unit*> UnitList::getUnits() const{
    vector<Unit*> units;
    Node* current = head;
    while(current){//parse the linked list
        units.push_back(current->unit);
        current=current->next;
    }
    return units;
}

//3.7 TerrainElement
void Mountain::getEffect(Army *army) {
    LiberationArmy* libArm = dynamic_cast<LiberationArmy*>(army);
    ARVN* arvnArm = dynamic_cast<ARVN*>(army);
    double radius = libArm? 2 : arvnArm? 4 : 0;
    double infantryMulti =  libArm? 1.3 : arvnArm? 1.2 : 0;
    double vehicleMulti =  libArm? 0.9 : arvnArm? 0.95 : 0;
    
    if(!army || radius==0.0) return;

    for(Unit* unit: army->getUnitList()->getUnits()){
        if(pos.distanceTo(unit->getCurrentPosition()) <= radius){
            if(dynamic_cast<Infantry*>(unit))
            unit->applyTerrainMulti(infantryMulti);
            else if(dynamic_cast<Vehicle*>(unit))
            unit->applyTerrainMulti(vehicleMulti);
        }
    }
}

void River::getEffect(Army *army){
    for(Unit* unit: army->getUnitList()->getUnits()){
        if((pos.distanceTo(unit->getCurrentPosition()) <= 2.0) && (dynamic_cast<Infantry*>(unit))){
            unit->applyTerrainMulti(0.9);//reduce 10%
        }
    }
}

void Urban::getEffect(Army *army){
    LiberationArmy* libArm = dynamic_cast<LiberationArmy*>(army);
    ARVN* arvnArm = dynamic_cast<ARVN*>(army);
    for(Unit* unit: army->getUnitList()->getUnits()){
    double distance = pos.distanceTo(unit->getCurrentPosition());
    if(libArm){
        if(distance <= 5.0){
            Infantry* inf = dynamic_cast<Infantry*>(unit);
            if(inf && (inf->getInfantryType()==SPECIALFORCES || inf->getInfantryType()==REGULARINFANTRY)){
                double additive = (2*unit->getAttackScore()/distance);
                unit->applyTerrainAdd(additive);
            }
        }
        if(distance <= 2.0){
            Vehicle* v = dynamic_cast<Vehicle*>(unit);
            if(v && (v->getVehicleType() == ARTILLERY)){
                unit->applyTerrainMulti(0.5);
           }
        }
    }
    else if (arvnArm){
        if(distance <= 3.0){
        Infantry* inf = dynamic_cast<Infantry*>(unit);
            if(inf && (inf->getInfantryType()==REGULARINFANTRY)){
                double additive = ((3*unit->getAttackScore())/(2*distance));
                unit->applyTerrainAdd(additive);
            }
        }
    }
}
}

void Fortification::getEffect(Army* army){
    LiberationArmy* libArm = dynamic_cast<LiberationArmy*>(army);
    ARVN* arvnArm = dynamic_cast<ARVN*>(army);
    double multi = libArm? 0.8 : (arvnArm? 1.2 : 1.0);
    for(Unit* unit: army->getUnitList()->getUnits()){
        if(pos.distanceTo(unit->getCurrentPosition())<=2.0){
            unit->applyTerrainMulti(multi);
        }
    }
}

void SpecialZone::getEffect(Army* army){
    for(Unit* unit: army->getUnitList()->getUnits()){
        if(pos.distanceTo(unit->getCurrentPosition())<=1.0){
        unit->applyTerrainMulti(0.0);//set to 0
        unit->applyTerrainAdd(-unit->getScoreAdd());
        }
    }
}
    
//3.8 BattleField
BattleField::BattleField(int n_rows, int n_cols, vector<Position *> arrayForest,
    vector<Position *> arrayRiver, vector<Position *> arrayFortification,
    vector<Position *> arrayUrban, vector<Position *> arraySpecialZone):n_rows(n_rows),n_cols(n_cols){//constructor
        terrain.resize(n_rows, vector<TerrainElement*>(n_cols, nullptr));
        
        // Initialize with Road objects
        for (int i = 0; i < n_rows; i++) {
        for (int j = 0; j < n_cols; j++) {
            terrain[i][j] = new Road();
            if (!terrain[i][j]) {
                cerr << "Failed to allocate Road at (" << i << "," << j << ")\n";
                throw bad_alloc();
            }
        }
        }

        for(Position* p : arrayForest){
            if (p) {
            int r = p->getRow(), c = p->getCol();
            if (r >= 0 && r < n_rows && c >= 0 && c < n_cols) {
            delete terrain[r][c];
            terrain[r][c] = new Mountain(*p);
            if (!terrain[r][c]) {
                    cerr << "Failed to allocate Mountain at (" << r << "," << c << ")\n";
                    throw bad_alloc();
            }
        }
        }
        }
        for(Position* p : arrayRiver){
            if (p) {
            int r = p->getRow(), c = p->getCol();
            if (r >= 0 && r < n_rows && c >= 0 && c < n_cols) {
            delete terrain[r][c];
            terrain[r][c] = new River(*p);
            if (!terrain[r][c]) {
                    cerr << "Failed to allocate River at (" << r << "," << c << ")\n";
                    throw bad_alloc();
            }
            }
        }
        }
        for(Position* p : arrayFortification){
            if (p) {
            int r = p->getRow(), c = p->getCol();
            if (r >= 0 && r < n_rows && c >= 0 && c < n_cols) {
            delete terrain[r][c];
            terrain[r][c] = new Fortification(*p);
            if (!terrain[r][c]) {
                    cerr << "Failed to allocate Fortification at (" << r << "," << c << ")\n";
                    throw bad_alloc();
            }
            }
        }
        }
        for(Position* p : arrayUrban){
            if (p) {
            int r = p->getRow(), c = p->getCol();
            if (r >= 0 && r < n_rows && c >= 0 && c < n_cols) {
            delete terrain[r][c];
            terrain[r][c] = new Urban(*p);
            if (!terrain[r][c]) {
                    cerr << "Failed to allocate Urban at (" << r << "," << c << ")\n";
                    throw bad_alloc();
            }
            }
        }
        }
        for(Position* p : arraySpecialZone){
            if (p) {
            int r = p->getRow(), c = p->getCol();
            if (r >= 0 && r < n_rows && c >= 0 && c < n_cols) {
            delete terrain[r][c];
            terrain[r][c] = new SpecialZone(*p);
            if (!terrain[r][c]) {
                    cerr << "Failed to allocate SpecialZone at (" << r << "," << c << ")\n";
                    throw bad_alloc();
            }
            }
        }
        }
    }

    BattleField::~BattleField(){//destructor
        for(auto& row : terrain){
            for(auto& elem : row){
                delete elem;
            }
        }
    }

    //3.9 Configuration
    Configuration::Configuration(const string & filepath){
            num_rows=0;
            num_cols=0;
            eventCode=0;
            liberationUnits.clear();
            ARVNUnits.clear();

        ifstream ifs(filepath);//open
        if(!ifs.is_open()){//if open failed
            return;//leaving defaults
        }
        
        //parse each line
        string line;
        while(getline(ifs,line)){
            if(line.find("NUM_ROWS=")!=string::npos){
                num_rows=stoi(line.substr(9));
            }
            else if(line.find("NUM_COLS=")!=string::npos){
                num_cols=stoi(line.substr(9));
            }
            else if(line.find("ARRAY_FOREST=")!=string::npos){
                arrayForest = parsePositions(line.substr(13));
            }
            else if(line.find("ARRAY_RIVER=")!=string::npos){
                arrayRiver = parsePositions(line.substr(12));
            }
            else if(line.find("ARRAY_FORTIFICATION=")!=string::npos){
                arrayFortification = parsePositions(line.substr(20));
            }
            else if(line.find("ARRAY_URBAN=")!=string::npos){
                arrayUrban = parsePositions(line.substr(12));
            }
            else if(line.find("ARRAY_SPECIAL_ZONE=")!=string::npos){
                arraySpecialZone = parsePositions(line.substr(19));
            }
            else if(line.find("UNIT_LIST=")!=string::npos){
                parseUnits(line.substr(10));
            }
            else if(line.find("EVENT_CODE=")!=string::npos){
                int code = stoi(line.substr(11));
                if(code<0) eventCode = 0;
                else if(code>99) eventCode = code % 100;//take the last 2 digits
                else eventCode = code;
            }
        }
        ifs.close();
    }

    vector<Position*> Configuration::parsePositions(const string& data){
        vector<Position*> positions;
        if(data=="[]") return positions;//empty array
        string posStr = data.substr(1,data.size()-2);//rule out []
        size_t start = 0;
        while(start<posStr.size()){
            size_t end = posStr.find(')',start);
            if(end==string::npos) break;//invalid, malformed input
            string pos = posStr.substr(start,end-start+1);
            positions.push_back(new Position(pos));
            start=end+2;//skip commas and proceed
        }
        return positions;
    }

    void Configuration::parseUnits(const string& data){
        if(data=="[]") return;
        string unitStr = data.substr(1,data.size()-2);//rule out []
        vector<string> unitDefs;
        size_t start=0;
        size_t parenCount=0;
        for(size_t i = 0; i<unitStr.size(); i++){//take each unit def to a vector
            if(unitStr[i]=='(') ++parenCount;
            else if(unitStr[i]==')') --parenCount;
            else if (unitStr[i]==',' && parenCount==0){
                unitDefs.push_back(unitStr.substr(start, i-start));
                start=i+1;
            }
        }
        unitDefs.push_back(unitStr.substr(start));//add the last definition

        //parse each unit def
        for(const string& unitDef : unitDefs){
            size_t paren = unitDef.find('(');
            if(paren==string::npos) continue;//skip malformed units

            string type = unitDef.substr(0,paren);
            string params = unitDef.substr(paren+1,unitDef.size()-paren-2);
            stringstream ss(params);
            string quantityStr, weightStr, posStr, armyStr;

            getline(ss, quantityStr, ',');
            getline(ss, weightStr, ',');

            size_t posStart = quantityStr.size()+weightStr.size()+2;
            size_t armyComma = params.rfind(',');
            if(armyComma==string::npos) continue;//check malformed
            posStr=params.substr(posStart,armyComma-posStart);
            armyStr=params.substr(armyComma+1);//take til end

            int quantity, weight, armyBelong;
            Position pos;
            try
            {
                quantity=stoi(quantityStr);
                weight=stoi(weightStr);
                pos = Position(posStr);
                armyBelong=stoi(armyStr);
            }
            catch(const std::exception& e)
            {
                continue;//skip invalid units
            }
            

            Unit* unit = nullptr;
            if(type=="TRUCK") unit = new Vehicle(quantity, weight, pos, TRUCK);
            else if(type=="MORTAR") unit = new Vehicle(quantity, weight, pos, MORTAR);
            else if(type=="ANTIAIRCRAFT") unit = new Vehicle(quantity, weight, pos, ANTIAIRCRAFT);
            else if(type=="ARMOREDCAR") unit = new Vehicle(quantity, weight, pos, ARMOREDCAR);
            else if(type=="APC") unit = new Vehicle(quantity, weight, pos, APC);
            else if(type=="ARTILLERY") unit = new Vehicle(quantity, weight, pos, ARTILLERY);
            else if(type=="TANK") unit = new Vehicle(quantity, weight, pos, TANK);
            else if(type=="SNIPER") unit = new Infantry(quantity, weight, pos, SNIPER);
            else if(type=="ANTIAIRCRAFTSQUAD") unit = new Infantry(quantity, weight, pos, ANTIAIRCRAFTSQUAD);
            else if(type=="MORTARSQUAD") unit = new Infantry(quantity, weight, pos, MORTARSQUAD);
            else if(type=="ENGINEER") unit = new Infantry(quantity, weight, pos, ENGINEER);
            else if(type=="SPECIALFORCES") unit = new Infantry(quantity, weight, pos, SPECIALFORCES);
            else if(type=="REGULARINFANTRY") unit = new Infantry(quantity, weight, pos, REGULARINFANTRY);
            else continue;//skip unknown type

            if(unit){
            if(armyBelong==0) liberationUnits.push_back(unit);
            else if(armyBelong==1) ARVNUnits.push_back(unit);
            else continue;//invalid army belong
            }
        }
    }

    Configuration::~Configuration(){
        for(Position* p : arrayForest) delete p;
        for(Position* p : arrayRiver) delete p;
        for(Position* p : arrayFortification) delete p;
        for(Position* p : arrayUrban) delete p;
        for(Position* p : arraySpecialZone) delete p;
        for(Unit* u : liberationUnits) delete u;
        for(Unit* u : ARVNUnits) delete u;
    }

    string Configuration::str() const {
        stringstream ss;
        ss << "[";
        ss << "num_rows=" << num_rows << ",";
        ss << "num_cols=" << num_cols << ",";
        
        ss << "arrayForest=[";
        for(size_t i = 0; i < arrayForest.size(); i++){
            ss << arrayForest[i]->str();
            if(i < arrayForest.size()-1) ss << ",";
        }
        ss << "],";

        ss << "arrayRiver=[";
        for(size_t i = 0; i < arrayRiver.size(); i++){
            ss << arrayRiver[i]->str();
            if(i < arrayRiver.size()-1) ss << ",";
        }
        ss << "],";

        ss << "arrayFortification=[";
        for(size_t i = 0; i < arrayFortification.size(); i++){
            ss << arrayFortification[i]->str();
            if(i < arrayFortification.size()-1) ss << ",";
        }
        ss << "],";

        ss << "arrayUrban=[";
        for(size_t i = 0; i < arrayUrban.size(); i++){
            ss << arrayUrban[i]->str();
            if(i < arrayUrban.size()-1) ss << ",";
        }
        ss << "],";

        ss << "arraySpecialZone=[";
        for(size_t i = 0; i < arraySpecialZone.size(); i++){
            ss << arraySpecialZone[i]->str();
            if(i < arraySpecialZone.size()-1) ss << ",";
        }
        ss << "],";

        ss << "liberationUnits=[";
        for(size_t i = 0; i < liberationUnits.size(); i++){
            ss << liberationUnits[i]->str();
            if(i < liberationUnits.size()-1) ss << ",";
        }
        ss << "],";

        ss << "ARVNUnits=[";
        for(size_t i = 0; i < ARVNUnits.size(); i++){
            ss << ARVNUnits[i]->str();
            if(i < ARVNUnits.size()-1) ss << ",";
        }
        ss << "],";

        ss << "eventCode=" << eventCode;
        ss << "]";
        return ss.str();
    }

    //3.10 HCMCampaign
    HCMCampaign::HCMCampaign(const string &config_file_path){
        config = new Configuration(config_file_path);
        battleField = new BattleField(config->getNumRows(), config->getNumCols(), config->getArrayForest(), config->getArrayRiver(), config->getArrayFortification(), config->getArrayUrban(), config->getArraySpecialZone());
        liberationArmy = new LiberationArmy(const_cast<Unit**>(config->getLiberationUnits().data()), config->getLiberationUnits().size(), "LiberationArmy", battleField);
        ARVN = new class ARVN(const_cast<Unit**>(config->getARVNUnits().data()), config->getARVNUnits().size(), "ARVN", battleField);
        config->clearUnits();
    }

    void HCMCampaign::run(){
        //reset terrain effect to defaults
        for(Unit* unit : liberationArmy->getUnitList()->getUnits()) unit->resetTerrainEffect();    
        for(Unit* unit : ARVN->getUnitList()->getUnits()) unit->resetTerrainEffect();

        vector<TerrainElement*> mountains, rivers, urbans, fortifications, specialZones;
        for(int i = 0; i<battleField->getRows(); i++){
            for(int j = 0; j<battleField->getCols(); j++){
                TerrainElement* elem = battleField->getTerrainElement(i,j);
                if(elem){
                if(dynamic_cast<Mountain*>(elem)) mountains.push_back(elem);
                else if(dynamic_cast<River*>(elem)) rivers.push_back(elem);
                else if(dynamic_cast<Urban*>(elem)) urbans.push_back(elem);
                else if(dynamic_cast<Fortification*>(elem)) fortifications.push_back(elem);
                else if(dynamic_cast<SpecialZone*>(elem)) specialZones.push_back(elem);
                //skip road because it has no effects
                }
            }
        }

        //apply effects in order
        for(TerrainElement* mountain : mountains){
            mountain->getEffect(liberationArmy);
            mountain->getEffect(ARVN);
        }
        for(TerrainElement* river : rivers){
            river->getEffect(liberationArmy);
            river->getEffect(ARVN);
        }
        for(TerrainElement* urban : urbans){
            urban->getEffect(liberationArmy);
            urban->getEffect(ARVN);
        }
        for(TerrainElement* fortification : fortifications){
            fortification->getEffect(liberationArmy);
            fortification->getEffect(ARVN);
        }
        for(TerrainElement* specialZone : specialZones){
            specialZone->getEffect(liberationArmy);
            specialZone->getEffect(ARVN);
        }

        //update indices after effects
        liberationArmy->updateIndices();
        ARVN->updateIndices();


        //Fight begins
        if(config->getEventCode() < 75){
            ARVN->fight(liberationArmy,true);//ARVN defends, meaning Liberation is attacking
        }
        else{
            ARVN->fight(liberationArmy,false);//ARVN offends
            liberationArmy->fight(ARVN, false);//liberation counterattack
        }


        //remove units
        vector<Unit*> toRemoveLib, toRemoveARVN;
        for (Unit* unit : liberationArmy->getUnitList()->getUnits()){
            if(liberationArmy->libChanged){
                if(unit->getAttackScore()<=5) toRemoveLib.push_back(unit);
            }
            else{
                if(unit->getEffectAttackScore()<=5) toRemoveLib.push_back(unit);
            }
        }
        for (Unit* unit : ARVN->getUnitList()->getUnits()){
            if(ARVN->arvnChanged){
                if(unit->getAttackScore()<=5) toRemoveARVN.push_back(unit);
            }
            else{
                if(unit->getEffectAttackScore()<=5) toRemoveARVN.push_back(unit);
            }
        }
        for(Unit* unit : toRemoveLib) liberationArmy->getUnitList()->remove(unit,false);
        for(Unit* unit : toRemoveARVN) ARVN->getUnitList()->remove(unit,false);

        //reset terrain effects before final updating indices
        for(Unit* unit : liberationArmy->getUnitList()->getUnits()) unit->resetTerrainEffect();
        for(Unit* unit : ARVN->getUnitList()->getUnits()) unit->resetTerrainEffect();

        //update indices after removals
        liberationArmy->updateIndices();
        ARVN->updateIndices();
    }

    string HCMCampaign::printResult(){
        stringstream ss;
        ss << "LIBERATIONARMY[LF=" << liberationArmy->getLF() << ",EXP=" << liberationArmy->getEXP()
            << "]-ARVN[LF=" << ARVN->getLF() << ",EXP=" << ARVN->getEXP() << "]";
            return ss.str();
    }

////////////////////////////////////////////////
/// END OF STUDENT'S ANSWER
////////////////////////////////////////////////