#include "can.hpp"
#include "motor.hpp"
#include "thread"
#include "chrono"

int main()
{

    can can1("can0");
    can1.start_rx();
    motor motor1(can1,10,"cg");
    std::this_thread::sleep_for(std::chrono::seconds(2));
    motor1.exit_mit();
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
    motor1.set_mech_zero();
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
    motor1.enter_mit();
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
    
    
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
    while (true)
    {
        
    motor1.cmd(0,5,0,1,0);
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    
    motor1.read();
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    
    }
 
    

    
    return 0;
}