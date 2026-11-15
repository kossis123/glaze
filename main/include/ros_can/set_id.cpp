#include "can.hpp"
#include "motor.hpp"
#include "thread"
#include "chrono"

int main()
{

    can can1;
    can1.start_rx();
    motor motor1(can1,10,"cg");
    std::this_thread::sleep_for(std::chrono::seconds(2));
    motor1.exit_mit();
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
    motor1.set_can_id(1);

    
    return 0;
}