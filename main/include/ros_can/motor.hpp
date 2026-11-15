#ifndef MOTOR_HPP
#define MOTOR_HPP

#define P_MIN -12.5f
#define P_MAX 12.5f
#define V_MIN -30.0f
#define V_MAX 30.0f
#define KP_MIN 0.0f
#define KP_MAX 500.0f
#define KD_MIN 0.0f
#define KD_MAX 5.0f
#define T_MIN -12.0f
#define T_MAX 12.0f

#define CG_MOTOR_CONTROL 0x01
#define CG_MOTOR_FEEDBACK 0x02
#define CG_MOTOR_ENABLE 0x03
#define CG_MOTOR_DISABLE 0x04
#define CG_MECH_ZERO_POSE 0x06
#define CG_SET_CAN_ID 0x07


#include "can.hpp"
using namespace std;



int float_to_uint(float x, float x_min, float x_max, int bits) {
    float span = x_max - x_min;
    float offset = x_min;
    if(x > x_max) x=x_max;
    else if(x < x_min) x= x_min;
    return (int) ((x-offset)*((float)((1<<bits)-1))/span);
}

float uint_to_float(uint16_t x, float x_min, float x_max)
{
    float span = x_max - x_min;
    return (float)x / 65535.0f * span + x_min;
}

class motor
{
    public:
    can &can_device_;
    int motor_id_;
    int master_id_;
    struct can_frame frames_{};
    string motor_;


    motor(can &can_, int motor_id, string motor):
    can_device_(can_),motor_id_(motor_id),master_id_(0x0000),motor_(motor)
    {
        
    }

    int make_can_id(int comm_type, int data_)
    {
        return{
            (comm_type & 0x1F) <<24 |
            (data_ & 0xFFFF)<<8 |
            (motor_id_ & 0xFF)

        };
    }

    void enter_mit()
    {

        int can_eframe_=make_can_id(CG_MOTOR_ENABLE, master_id_);
        frames_.can_id=can_eframe_;
        frames_.len=8;
        
        can_device_.send_can(frames_, true);
    }

    void exit_mit()
    {

        int can_eframe_=make_can_id(CG_MOTOR_DISABLE, master_id_);
        frames_.can_id=can_eframe_;
        frames_.len=8;
        frames_.data[0]=1;

        can_device_.send_can(frames_, true);
    }

    void cmd(float p, float v, float kp, float kd, float tq)
    {

        uint p_=float_to_uint(p, P_MIN, P_MAX, 16);
        uint v_=float_to_uint(v, V_MIN, V_MAX, 16);
        uint kp_=float_to_uint(kp, KP_MIN, KP_MAX, 16);
        uint kd_=float_to_uint(kd, KD_MIN, KD_MAX, 16);
        uint tq_=float_to_uint(tq, T_MIN, T_MAX, 16);

        int can_eframe_=make_can_id(CG_MOTOR_CONTROL,tq_);
        frames_.can_id=can_eframe_;
        frames_.len=8;
        frames_.data[0]=(p_ >> 8);
        frames_.data[1]=(0xFF & p_);

        frames_.data[2]=(v_ >> 8);
        frames_.data[3]=(0xFF & v_);

        frames_.data[4]=(kp_ >> 8);
        frames_.data[5]=(0xFF & kp_);

        frames_.data[6]=(kd_ >> 8);
        frames_.data[7]=(0xFF & kd_); 

        can_device_.send_can(frames_,true);

    }

    void set_can_id(int id)
    {
        int can_eframe_=make_can_id(CG_SET_CAN_ID, master_id_);
        can_eframe_=(id << 16) | can_eframe_;
        frames_.can_id=can_eframe_;
        frames_.len=8;
        frames_.data[0]=1;

        can_device_.send_can(frames_,true);
    }

    void set_mech_zero()
    {
        int can_eframe_=make_can_id(CG_MECH_ZERO_POSE, master_id_);
        frames_.can_id=can_eframe_;
        frames_.len=8;
        frames_.data[0]=1;

        can_device_.send_can(frames_,true);
    } 

    void read()
    {
        
        can_frame datas = can_device_.read_data();

        float position = uint_to_float((datas.data[0] <<8 | datas.data[1]), P_MIN, P_MAX);
        float velocity= uint_to_float((datas.data[2] <<8 | datas.data[3]), V_MIN, V_MAX);
        float torque= uint_to_float((datas.data[4] << 8| datas.data[5]), T_MIN, T_MAX);
        float temperature=(datas.data[6] << 8 | datas.data[7]) / 10.0;
        int fault= datas.can_id >> 16 & 0x3F;

        std::cout<< "pos:" << position << " "
         << "vel:" << velocity << " " 
         <<"torque:" << torque << " "
         << "temperature:" << temperature<< " " 
         <<"fault"<< fault<<endl;  
    
    }
    ~motor()
    {
        exit_mit();
    }

};

#endif