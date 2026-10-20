

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
    // << 将右侧内容送去输出；\n 表示换行。这里只打印启动动作。
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
        return; // 已经在目标状态，不重复执行入口动作。
    }
    if (currentState == State::Error)
    {
        return; // 本版本没有 Error 复位操作。
    }

    currentState = nextState; // 改变 TS 当前状态。

    // switch 按当前状态选择一个 case 分支。
    switch (currentState)
    {
        case State::Idle:
            openContactors();
            break; // 退出 switch，避免继续执行下一个 case。

        case State::CloseContactors:
            closeContactors();
            break;

        case State::WaitR2D:
            break; // 等待 startPressed()，入口不执行额外动作。

        case State::StartInverter:
            SinceLastMessageTime = 0; // 从初始化开始计算首次消息等待时间。
            inverterFault = inverterInit(); // 保存函数返回的故障码。
            if (inverterFault != 0)
            {
                // 再调用 enterState，立即进入 Error 并停止逆变器。
                enterState(State::Error);
            }
            break;

        case State::Buzzer:
            buzzerElapsedTime = 0;
            startBuzzer();
            // 不清零通信时间：进入 Buzzer 并不代表收到消息。
            break;

        case State::Driving:
            setR2DLedOn();
            // 同样不清零通信时间。
            break;

        case State::Error:
            stopInverter();
            break;
    }
}

void TS::update(int dtMs)
{
    if (dtMs < 0)
    {
        return; // 不接受负的时间间隔。
    }
    if (currentState == State::Error)
    {
        return; // 已经报错，不再推进正常流程。
    }
    if (contactorState == ContactorState::Error)
    {
        enterState(State::Error);
        return;
    }

    // || 表示“或者”：处于其中任意一种状态，都需要监测消息。
    if (currentState == State::StartInverter ||
        currentState == State::Buzzer ||
        currentState == State::Driving)
    {
        // 等价于“累计后是否 > 200”，但先比较可避免整数加法溢出。
        // 例如已等待 150 ms，则还能等 50 ms；再过 51 ms 就超时。
        if (dtMs > MESSAGE_TIMEOUT - SinceLastMessageTime)
        {
            enterState(State::Error);
            return; // 报错后不能继续执行正常转换。
        }
        SinceLastMessageTime += dtMs; // += 表示在原值上累加。
    }

    // 本模拟在同一轮遇到多种异常时先处理故障，题目未规定并发优先级。
    if (sdcState == SDCState::Open)
    {
        enterState(State::Idle);
        return;
    }

    switch (currentState)
    {
        case State::Idle:
            break; // 按钮转换已交给 TSStartPressed()。

        case State::CloseContactors:
            if (sdcState == SDCState::Closed &&
                contactorState == ContactorState::Active)
            {
                enterState(State::WaitR2D);
            }
            break;

        case State::WaitR2D:
            break; // 按钮转换已交给 startPressed()。

        case State::StartInverter:
            if (inverterFault != 0)
            {
                enterState(State::Error);
            }
            else if (inverterState == InverterState::Enabled)
            {
                // 前一个 if 不成立，说明 fault == 0。
                enterState(State::Buzzer);
            }
            break;

        case State::Buzzer:
            // 等价于“累计后是否 >= BUZZER_LENGTH”，先比较避免溢出。
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
            break; // 上面的公共检查仍然持续运行。

        case State::Error:
            break;
    }
}

// 将枚举转成文字，方便演示。这是辅助函数，不属于 TS 类。
// const char* 在这里表示返回一个固定文本的地址，可先按“文字”理解。
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

int main() // 程序从 main 开始执行。
{
    TS ts; // 创建一个名叫 ts 的对象，采用类中给定的初始值。
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

    // i 从 0 开始；只要 i < 10 就继续；每轮结束 ++i（加 1）。
    // 因此共执行 10 次，每次模拟 100 ms，总共 1000 ms。
    for (int i = 0; i < 10; ++i)
    {
        ts.update(100);
        ts.receiveInverterMessage(); // 模拟消息在本次时间推进后到达。
    }
    std::cout << "After buzzer: " << stateName(ts.getState()) << '\n';

    ts.update(200); // 不提供消息，恰好 200 ms 仍然允许。
    std::cout << "No message for 200 ms: " << stateName(ts.getState()) << '\n';
    ts.update(1); // 总共 201 ms，超时，停止逆变器并进入 Error。
    std::cout << "No message for 201 ms: " << stateName(ts.getState()) << '\n';
    ts.openShutdownCircuit();
    std::cout << "Open SDC after Error: " << stateName(ts.getState()) << '\n';

    // 创建第二个独立对象，演示初始化失败，不影响前一个对象。
    TS failedTS;
    failedTS.setInitialisationFault(7); // 任意非零故障码均表示失败。
    failedTS.closeShutdownCircuit();
    failedTS.TSStartPressed();
    failedTS.update(0);
    failedTS.setBrakes(true);
    failedTS.startPressed();
    std::cout << "Initialisation fault: " << stateName(failedTS.getState()) << '\n';

    return 0; // 返回 0，表示程序正常结束。
}
