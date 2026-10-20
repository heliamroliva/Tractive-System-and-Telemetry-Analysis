

#include <iostream> 

enum class State
{
    Idle,
    CloseContactors,
    WaitR2D,
    StartInverter,
    Buzzer,
    Driving,
    Error
};

enum class ContactorState { Ready, Active, Error };
enum class SDCState { Closed, Open };
enum class InverterState { Stopped, Enabled };
enum class Brakes { Applied, Released };


class TS
{
private:
    // private 成员只能由类内部的方法直接访问。
    // State 是类型，currentState 是变量名，State::Idle 是初始值。
    State currentState = State::Idle;

    // 部件的当前条件与 TS 自身的状态分开保存。
    ContactorState contactorState = ContactorState::Ready;
    SDCState sdcState = SDCState::Open;
    InverterState inverterState = InverterState::Stopped;
    Brakes brakes = Brakes::Released;

    int inverterFault = 0; // 本次初始化产生的故障码。

    // 只供模拟：下一次初始化应返回什么结果。默认 0，即成功。

    int simulatedInitialisationFault = 0;

    int BUZZER_LENGTH = 1000;     // lasting time of buzzer in ms; can be changed by setBuzzerLength().
    int buzzerElapsedTime = 0;    // time elapsed since buzzer started; reset to 0 when entering Buzzer state.
    int SinceLastMessageTime = 0; // time elapse since the last message
    const int MESSAGE_TIMEOUT = 200; // const：cannot be changed after initialization

    // 函数声明，分号表示声明结束；具体函数体写在类外。
    void enterState(State nextState);
    void closeContactors();
    void openContactors();
    int inverterInit(); // return int fault code
    void stopInverter();
    void startBuzzer();
    void setR2DLedOn();

public:
    // public 方法可以由 main 或队友的代码调用。
    // 4 methods requested by the assignment
    void setBuzzerLength(int lengthMs);
    State getState() const; // return State；const means this method does not change any member variable.
    void closeShutdownCircuit();
    void openShutdownCircuit();

    // button pressed event handlers：
    void TSStartPressed();
    void startPressed();
    void setBrakes(bool applied);
    void setContactorState(ContactorState state);
    void receiveInverterMessage();
    void update(int dtMs);

    // 
    void setInitialisationFault(int faultCode);
}; 

// 
void TS::setBuzzerLength(int lengthMs)
{
    if (lengthMs >= 0) 
    {
        BUZZER_LENGTH = lengthMs;
    }
}

State TS::getState() const
{
    return currentState; 
}

void TS::closeShutdownCircuit()
{
    sdcState = SDCState::Closed;
}

void TS::openShutdownCircuit()
{
    sdcState = SDCState::Open;
    if (currentState != State::Error)
    {
        enterState(State::Idle);
    }
}

void TS::setBrakes(bool applied)
{
    if (applied) 
    {
        brakes = Brakes::Applied;
    }
    else
    {
        brakes = Brakes::Released;
    }
}

void TS::setContactorState(ContactorState state)
{
    contactorState = state;
    if (contactorState == ContactorState::Error)
    {
        enterState(State::Error);
    }
}

void TS::receiveInverterMessage()
{
    SinceLastMessageTime = 0; 
}

void TS::setInitialisationFault(int faultCode)
{
    simulatedInitialisationFault = faultCode;
}

void TS::TSStartPressed()
{
    
    if (currentState == State::Idle &&
        sdcState == SDCState::Closed &&
        contactorState == ContactorState::Ready)
    {
        enterState(State::CloseContactors);
    }
}

void TS::startPressed()
{
    if (currentState == State::WaitR2D &&
        contactorState == ContactorState::Active &&
        sdcState == SDCState::Closed &&
        brakes == Brakes::Applied)
    {
        enterState(State::StartInverter);
    }
}

void TS::closeContactors()
{
   
    contactorState = ContactorState::Active;
}

void TS::openContactors()
{
   
    if (contactorState != ContactorState::Error)
    {
        contactorState = ContactorState::Ready;
    }
}

int TS::inverterInit()
{
    
    int faultCode = simulatedInitialisationFault;
    if (faultCode == 0)
    {
        inverterState = InverterState::Enabled;
    }
    else
    {
        inverterState = InverterState::Stopped;
    }
    return faultCode; 
}

void TS::stopInverter()
{
    inverterState = InverterState::Stopped;
    std::cout << "Inverter stopped\n";
}

void TS::startBuzzer()
{

    std::cout << "Buzzer started\n";
}

void TS::setR2DLedOn()
{
    std::cout << "R2D LED on\n";
}

void TS::enterState(State nextState)
{
    if (currentState == nextState)
    {
        return; 
    }
    if (currentState == State::Error)
    {
        return; 
    }

    currentState = nextState; 

   
    switch (currentState)
    {
        case State::Idle:
            openContactors();
            break; 

        case State::CloseContactors:
            closeContactors();
            break;

        case State::WaitR2D:
            break; 

        case State::StartInverter:
            SinceLastMessageTime = 0; 
            inverterFault = inverterInit(); 
            if (inverterFault != 0)
            {
                
                enterState(State::Error);
            }
            break;

        case State::Buzzer:
            buzzerElapsedTime = 0;
            startBuzzer();
            break;

        case State::Driving:
            setR2DLedOn();
            break;

        case State::Error:
            stopInverter();
            break;
    }
}

void TS::update(int dtMs)
{
   
    if (currentState == State::Error)
    {
        return; 
    }
    if (contactorState == ContactorState::Error)
    {
        enterState(State::Error);
        return;
    }

  
    if (currentState == State::StartInverter ||
        currentState == State::Buzzer ||
        currentState == State::Driving)
    {
        if (dtMs > MESSAGE_TIMEOUT - SinceLastMessageTime)
        {
            enterState(State::Error);
            return; 
        }
        SinceLastMessageTime += dtMs; 
    }

    if (sdcState == SDCState::Open)
    {
        enterState(State::Idle);
        return;
    }

    switch (currentState)
    {
        case State::Idle:
            break; 

        case State::CloseContactors:
            if (sdcState == SDCState::Closed &&
                contactorState == ContactorState::Active)
            {
                enterState(State::WaitR2D);
            }
            break;

        case State::WaitR2D:
            break; 

        case State::StartInverter:
            if (inverterFault != 0)
            {
                enterState(State::Error);
            }
            else if (inverterState == InverterState::Enabled)
            {

                enterState(State::Buzzer);
            }
            break;

        case State::Buzzer:

            if (dtMs >= BUZZER_LENGTH - buzzerElapsedTime)
            {
                buzzerElapsedTime = BUZZER_LENGTH;
                enterState(State::Driving);
            }
            else
            {
                buzzerElapsedTime += dtMs;
            }
            break;

        case State::Driving:
            break; 

        case State::Error:
            break;
    }
}


const char* stateName(State state)
{
    switch (state)
    {
        case State::Idle: return "Idle";
        case State::CloseContactors: return "CloseContactors";
        case State::WaitR2D: return "WaitR2D";
        case State::StartInverter: return "StartInverter";
        case State::Buzzer: return "Buzzer";
        case State::Driving: return "Driving";
        case State::Error: return "Error";
    }
    return "Unknown";
}

int main() 
{
    TS ts;
    std::cout << "Initial: " << stateName(ts.getState()) << '\n';

    ts.setBuzzerLength(1000);
    ts.closeShutdownCircuit();

    ts.TSStartPressed();
    std::cout << stateName(ts.getState()) << '\n'; // CloseContactors
    ts.update(0);
    std::cout << stateName(ts.getState()) << '\n'; // WaitR2D

    ts.setBrakes(true); // true 表示踩下刹车。
    ts.startPressed();
    std::cout << stateName(ts.getState()) << '\n'; // StartInverter
    ts.update(0);
    std::cout << stateName(ts.getState()) << '\n'; // Buzzer


    for (int i = 0; i < 10; ++i)
    {
        ts.update(100);
        ts.receiveInverterMessage(); 
    }
    std::cout << "After buzzer: " << stateName(ts.getState()) << '\n';

    ts.update(200);
    std::cout << "No message for 200 ms: " << stateName(ts.getState()) << '\n';
    ts.update(1); 
    std::cout << "No message for 201 ms: " << stateName(ts.getState()) << '\n';
    ts.openShutdownCircuit();
    std::cout << "Open SDC after Error: " << stateName(ts.getState()) << '\n';

  
    TS failedTS;
    failedTS.setInitialisationFault(7); // set initialisation fault code to 7 to simulate a failure
    failedTS.closeShutdownCircuit();
    failedTS.TSStartPressed();
    failedTS.update(0);
    failedTS.setBrakes(true);
    failedTS.startPressed();
    std::cout << "Initialisation fault: " << stateName(failedTS.getState()) << '\n';

    return 0; 
}
