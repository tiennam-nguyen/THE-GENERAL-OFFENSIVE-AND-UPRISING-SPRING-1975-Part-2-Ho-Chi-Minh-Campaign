#include "hcmcampaign.h"
#include "unit_test_Task4.hpp"
#include <fstream>
#include <string>

bool Task401() {
    UNIT_TEST_Task4 tester;
    std::string cfg = "config401.txt";
    std::cout << "Starting Task401\n";
    std::string name = "Task429";
    
    //! data ------------------------------------
    Vehicle tankOfLiber(20, 20, Position(1, 2), MORTAR);
    Vehicle truckOfLiber(15, 10, Position(2, 2), TRUCK);
    Infantry sniperOfLiber(10, 20, Position(1, 1), SNIPER);

    Vehicle tankOfAr(5, 20, Position(3, 2), MORTAR);
    Vehicle truckOfAr(1, 10, Position(3, 1), TRUCK);
    Infantry sniperOfAr(7, 20, Position(3, 3), SNIPER);

    Unit** unitArrayOfLiber = new Unit*[3];
    unitArrayOfLiber[0] = &tankOfLiber;
    unitArrayOfLiber[1] = &truckOfLiber;
    unitArrayOfLiber[2] = &sniperOfLiber;

    Unit** unitArrayOfAr = new Unit*[3];
    unitArrayOfAr[0] = &tankOfAr;
    unitArrayOfAr[1] = &truckOfAr;
    unitArrayOfAr[2] = &sniperOfAr;
    
    //! process ---------------------------------
    stringstream output;
    LiberationArmy* liberationArmy = new LiberationArmy(unitArrayOfLiber, 3, "LiberationArmy", 0);
    output << liberationArmy->str() << endl;
    ARVN* arvn = new ARVN(unitArrayOfAr, 3, "ARVN", 0);
    output << arvn->str() << endl;

    Army* enemyLiber = arvn;
    liberationArmy->fight(enemyLiber, false);
    Army* enemyAr = liberationArmy;
    arvn->fight(enemyAr, false);

    output << "* After the fight" << endl;
    output << liberationArmy->str() << endl;
    output << arvn->str() << endl;
    
    //! expect ----------------------------------
    string expect = 
"LiberationArmy[LF=29,EXP=200,unitList=UnitList[count_vehicle=2;count_infantry=1;Infantry[infantryType=SNIPER,quantity=10,weight=20,position=(1,1)],Vehicle[vehicleType=MORTAR,quantity=20,weight=20,position=(1,2)],Vehicle[vehicleType=TRUCK,quantity=15,weight=10,position=(2,2)]],battleField=]\n"
"ARVN[LF=15,EXP=180,unitList=UnitList[count_vehicle=2;count_infantry=1;Infantry[infantryType=SNIPER,quantity=9,weight=20,position=(3,3)],Vehicle[vehicleType=MORTAR,quantity=5,weight=20,position=(3,2)],Vehicle[vehicleType=TRUCK,quantity=1,weight=10,position=(3,1)]],battleField=]\n"
"* After the fight\n"
"LiberationArmy[LF=20,EXP=180,unitList=UnitList[count_vehicle=2;count_infantry=1;Infantry[infantryType=SNIPER,quantity=9,weight=20,position=(3,3)],Vehicle[vehicleType=TRUCK,quantity=16,weight=10,position=(2,2)],Vehicle[vehicleType=MORTAR,quantity=5,weight=20,position=(3,2)]],battleField=]\n"
"ARVN[LF=0,EXP=0,unitList=UnitList[count_vehicle=0;count_infantry=0],battleField=]\n";
    
    //! output ----------------------------------
    
    //! remove data -----------------------------
    delete liberationArmy;
    delete arvn;
    delete[] unitArrayOfLiber;
    delete[] unitArrayOfAr;
    
    //! result ----------------------------------
    return tester.printResult(output.str(), expect, name);
}

int main() {
    std::cout << "Running Task401\n";
    bool passed401 = Task401();
    std::cout.flush();
    std::cout << "Summary:\n";
    std::cout << "Task401: " << (passed401 ? "PASSED" : "FAILED") << "\n";
    return (passed401) ? 0 : 1;
}